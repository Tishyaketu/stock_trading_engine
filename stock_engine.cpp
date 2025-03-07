#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <random>

// Constants
constexpr int MAX_ORDERS = 100;  // Max orders per broker
constexpr int MAX_TICKERS = 1024;  // Maximum stocks being traded
constexpr int MAX_PRICE = 1000;  // Maximum stock price

// Order Types
enum class OrderType { BUY, SELL };

// Struct for Order
struct Order {
    OrderType type;
    int quantity;
    double price;
    std::chrono::steady_clock::time_point timestamp; // Arrival time
    Order* next;
};

// Lock-Free Order Book using Atomic Pointers
std::atomic<Order*> buy_orders[MAX_TICKERS][MAX_PRICE] = {nullptr};
std::atomic<Order*> sell_orders[MAX_TICKERS][MAX_PRICE] = {nullptr};

// Array for storing company names
std::string company_names[MAX_TICKERS];

// Load Company Names from File into Array
void load_company_names(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Unable to open company list file: " << filename << std::endl;
        exit(1);
    }

    for (int i = 0; i < MAX_TICKERS && std::getline(file, company_names[i]); i++);
    file.close();
}

// File for Logging Trades
std::ofstream trade_log("trade_log.txt");

// Function to Log Orders
void log_order(int ticker_index, OrderType type, int quantity, double price) {
    std::string order_type = (type == OrderType::BUY) ? "BUY" : "SELL";
    
    std::cout << "[ORDER] " << order_type 
              << " | Company: " << company_names[ticker_index] 
              << " | Price: " << price 
              << " | Quantity: " << quantity << std::endl;

    trade_log << "[ORDER] " << order_type 
              << " | Company: " << company_names[ticker_index] 
              << " | Price: " << price 
              << " | Quantity: " << quantity << std::endl;
}

// Function to Log Trade Execution
void log_trade(int ticker_index, double buy_price, double sell_price, int quantity) {
    std::cout << "[TRADE] Company: " << company_names[ticker_index]
              << " | Buy @" << buy_price 
              << " | Sell @" << sell_price 
              << " | Quantity: " << quantity << std::endl;

    trade_log << "[TRADE] Company: " << company_names[ticker_index]
              << " | Buy @" << buy_price 
              << " | Sell @" << sell_price 
              << " | Quantity: " << quantity << std::endl;
}

// Lock-Free Order Matching with Pre-Trade Logging
void match_orders(int ticker_index) {
    std::cout << "\n[INFO] Attempting to match orders for Company " << company_names[ticker_index] << "...\n";

    int highest_buy = MAX_PRICE - 1;
    while (highest_buy >= 0 && buy_orders[ticker_index][highest_buy].load() == nullptr) {
        highest_buy--;
    }

    int lowest_sell = 0;
    while (lowest_sell < MAX_PRICE && sell_orders[ticker_index][lowest_sell].load() == nullptr) {
        lowest_sell++;
    }

    while (highest_buy >= lowest_sell && highest_buy >= 0 && lowest_sell < MAX_PRICE) {
        Order* buy_order = buy_orders[ticker_index][highest_buy].load();
        Order* sell_order = sell_orders[ticker_index][lowest_sell].load();

        if (buy_order && sell_order && buy_order->price >= sell_order->price) {
            int match_quantity = std::min(buy_order->quantity, sell_order->quantity);
            
            // Log the match BEFORE execution
            std::cout << "[MATCH] Company: " << company_names[ticker_index]
                      << " | Matching Buy @" << buy_order->price 
                      << " with Sell @" << sell_order->price
                      << " | Quantity: " << match_quantity << std::endl;

            trade_log << "[MATCH] Company: " << company_names[ticker_index]
                      << " | Matching Buy @" << buy_order->price 
                      << " with Sell @" << sell_order->price
                      << " | Quantity: " << match_quantity << std::endl;

            // Execute trade
            log_trade(ticker_index, buy_order->price, sell_order->price, match_quantity);

            buy_order->quantity -= match_quantity;
            sell_order->quantity -= match_quantity;

            if (buy_order->quantity == 0) {
                buy_orders[ticker_index][highest_buy].compare_exchange_weak(buy_order, buy_order->next);
                delete buy_order;
            }

            if (sell_order->quantity == 0) {
                sell_orders[ticker_index][lowest_sell].compare_exchange_weak(sell_order, sell_order->next);
                delete sell_order;
            }
        } else {
            break;
        }
    }
}

// Function to Add Order (Lock-Free)
void add_order(int ticker_index, OrderType type, int quantity, double price) {
    int price_bucket = static_cast<int>(price);
    if (price_bucket >= MAX_PRICE) return;

    Order* new_order = new Order{type, quantity, price, std::chrono::steady_clock::now(), nullptr};
    std::atomic<Order*>* order_list = (type == OrderType::BUY) ? &buy_orders[ticker_index][price_bucket] : &sell_orders[ticker_index][price_bucket];

    Order* expected = nullptr;
    
    do {
        expected = order_list->load();
        if (!expected) {
            new_order->next = nullptr;
            if (order_list->compare_exchange_weak(expected, new_order)) {
                log_order(ticker_index, type, quantity, price);
                return;
            }
        } else {
            // FIFO Order Insertion (Preserving Time Order)
            Order* last = expected;
            while (last->next) {
                last = last->next;
            }
            last->next = new_order;
            log_order(ticker_index, type, quantity, price);
            return;
        }
    } while (true);
}

// Broker Simulation with Logging
void broker_activity(int broker_id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> order_count_dist(1, MAX_ORDERS); // Random order count
    std::uniform_int_distribution<> type_dist(0, 1);
    std::uniform_int_distribution<> ticker_dist(0, MAX_TICKERS - 1);
    std::uniform_int_distribution<> quantity_dist(1, 50);
    std::uniform_real_distribution<> price_dist(0.0, MAX_PRICE);

    int num_orders = order_count_dist(gen); // Random number of orders per broker

    for (int i = 0; i < num_orders; i++) {
        OrderType type = static_cast<OrderType>(type_dist(gen));
        int ticker_index = ticker_dist(gen);
        int quantity = quantity_dist(gen);
        double price = price_dist(gen);

        add_order(ticker_index, type, quantity, price);
        match_orders(ticker_index);

        std::this_thread::sleep_for(std::chrono::milliseconds(1 + (rand() % 10)));  // Random delay 1-10ms
    }
}

// Main Function
int main() {
    std::cout << "[INFO] Loading company names...\n";
    load_company_names("company_list.txt"); // Load company names from file

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> broker_count_dist(3, 20); // Random brokers (3 to 20)
    std::uniform_int_distribution<> broker_delay_dist(0, 2000); // Random broker arrival time (0-2000ms)

    int num_brokers = broker_count_dist(gen);
    std::cout << "[INFO] Starting Lock-Free Stock Trading Simulation with " << num_brokers << " brokers...\n";

    std::vector<std::thread> brokers;
    
    for (int i = 0; i < num_brokers; i++) {
        int arrival_delay = broker_delay_dist(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(arrival_delay)); // Simulate broker arrival
        brokers.emplace_back(broker_activity, i);
        std::cout << "[INFO] Broker " << i + 1 << " joined after " << arrival_delay << "ms\n";
    }

    for (auto &t : brokers) {
        t.join();
    }

    trade_log.close();
    std::cout << "[INFO] Trading simulation complete. Check 'trade_log.txt' for full details.\n";
    return 0;
}