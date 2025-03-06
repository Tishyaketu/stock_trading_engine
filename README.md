Stock Trading Engine

Overview
This is a lock-free, multi-threaded stock trading engine that simulates real-time buy and sell transactions for stocks. The system is designed to handle concurrent stockbroker operations while ensuring efficient order matching.

Features

Supports up to 1024 unique stocks (tickers)

Implements a lock-free order book using atomic pointers

Simulates real-time stock transactions with multiple brokers

Ensures race-condition handling in a multi-threaded environment

Matches buy and sell orders efficiently with O(n) complexity

Dynamically loads company names from an external file

Requirements

C++17 or later

g++ compiler with pthread support

A valid company list file (company_list.txt) with 1024 stock names

Installation

Clone this repository or download the source files.

Ensure that g++ is installed on your system.

Compilation
To compile the program, use the following command:

g++ -std=c++17 -o stock_engine stock_engine.cpp -pthread

Running the Program
Execute the compiled binary:

./stock_engine

Make sure company_list.txt exists in the same directory as the executable.

File Structure

stock_engine.cpp: The main source file containing the trading engine logic

company_list.txt: A list of 1024 company names used for stock tickers

trade_log.txt: The output file that logs order executions

Usage
The program automatically generates stock transactions and logs them in trade_log.txt. It prints order placement, order matches, and completed trades to the console as well.

Order Matching Logic

Orders are randomly generated by multiple broker threads.

Orders are stored in a lock-free order book using atomic pointers.

Orders are matched based on price and availability.

Successful matches are logged before trade execution.

Completed trades are recorded in trade_log.txt.

Example Output

[ORDER] BUY | Company: TechCorp | Price: 150.50 | Quantity: 10
[ORDER] SELL | Company: TechCorp | Price: 140.00 | Quantity: 10
[INFO] Attempting to match orders for Company TechCorp...
[MATCH] Company: TechCorp | Matching Buy @150.50 with Sell @140.00 | Quantity: 10
[TRADE] Company: TechCorp | Buy @150.50 | Sell @140.00 | Quantity: 10

Limitations

The system assumes that stock tickers remain constant and do not change dynamically.

The simulation does not include order expiration or advanced trading strategies.

Orders are processed sequentially within each price bucket.