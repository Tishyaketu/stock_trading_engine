### **README for Stock Trading Engine**  

# **Stock Trading Engine**  

## **Overview**  
This is a lock-free, multi-threaded stock trading engine that simulates real-time buy and sell transactions for stocks. The system dynamically handles stockbroker activity with **randomized broker entry times and order volumes**, ensuring a **realistic trading simulation**.  

## **Features**  

- Supports up to **1024 unique stocks (tickers)**  
- Implements a **lock-free order book** using atomic pointers  
- **Simulates real-time stock transactions** with a **random number of brokers (1-20)**  
- **Brokers arrive at random intervals** (0-2000ms) to simulate market dynamics  
- Each broker places a **random number of orders (1 to 100)**  
- Orders at the **same price level follow FIFO (First-In-First-Out) priority**  
- Ensures **race-condition handling** in a **multi-threaded environment**  
- Efficient **O(n) order matching complexity**  
- Dynamically loads company names from an **external file**  

---  

## **Requirements**  

- **C++17** or later  
- `g++` compiler with `pthread` support  
- A valid **company list file (`company_list.txt`)** containing 1024 stock names  

---  

## **Installation**  

1. Clone this repository or download the source files.  
2. Ensure that **g++** is installed on your system.  

---  

## **Compilation**  

To compile the program, use the following command:  

```sh
g++ -std=c++17 -o stock_engine stock_engine.cpp -pthread
```  

---  

## **Running the Program**  

Execute the compiled binary:  

```sh
./stock_engine
```  

> **Note:** Ensure that `company_list.txt` exists in the same directory as the executable.  

---  

## **File Structure**  

- `stock_engine.cpp` → The main source file containing the trading engine logic  
- `company_list.txt` → A list of **1024 company names** used for stock tickers  
- `trade_log.txt` → The **output file** that logs all order executions  

---  

## **Usage**  

- The program **automatically generates stock transactions** using multiple brokers.  
- Brokers arrive at **random times** and place a **random number of orders**.  
- Orders are stored in a **lock-free order book** using **atomic pointers**.  
- **FIFO priority** ensures older orders are executed first at the same price level.  
- Order placement, matching, and completed trades are printed to the console.  
- **Completed trades are recorded in `trade_log.txt`**.  

---  

## **Order Matching Logic**  

### **Randomized Broker Behavior:**  
- The number of brokers is **randomly determined** (1 to 20).  
- Each broker arrives **at a random time** (0-2000ms delay).  
- Brokers place a **random number of orders (1 to 100)**.  

### **Order Execution:**  
1. **Orders are stored in a lock-free order book** using atomic pointers.  
2. **Orders are matched if a buy price ≥ lowest sell price.**  
3. **FIFO priority:** If multiple orders exist at the same price, the earliest order is executed first.  
4. **Successful trades are logged before execution.**  
5. **Completed trades are recorded in `trade_log.txt`.**  

---  

## **Example Output**  

```
[INFO] Broker 1 joined after 1200ms
[ORDER] BUY  | Company: TechCorp | Price: 150.50 | Quantity: 10
[INFO] Broker 2 joined after 500ms
[ORDER] SELL | Company: TechCorp | Price: 140.00 | Quantity: 10
[INFO] Attempting to match orders for Company TechCorp...
[MATCH] Company: TechCorp | Matching Buy @150.50 with Sell @140.00 | Quantity: 10
[TRADE] Company: TechCorp | Buy @150.50 | Sell @140.00 | Quantity: 10
```  

---  

## **Limitations**  

- **Stock tickers remain constant** and do not change dynamically.  
- **No order expiration or advanced trading strategies** (e.g., stop-loss, limit orders).  
- **Order matching within a price level is FIFO, but not across different price levels.**  

---  

## **Future Enhancements**  

- Add **market depth visualization** to show active buy/sell orders.  
- Implement **order expiration** (e.g., unfulfilled orders expire after a set time).  
- Support **advanced order types** (e.g., limit orders, stop-loss).  
- Optimize performance for **high-frequency trading scenarios**.  

---  

**This simulation provides a fast, realistic, and lock-free trading engine for concurrent stock trading!**  