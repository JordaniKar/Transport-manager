# 🚌 Urban Transit Management System

A robust, multi-threaded Client-Server application built in C++ for managing urban public transport networks. This project demonstrates low-level network programming, concurrent client handling, and custom text-based protocol parsing.

## 🚀 Key Features

* **Multi-threaded Architecture:** The server utilizes `std::thread` to handle multiple client connections simultaneously without blocking.
* **TCP/IP Sockets:** Direct integration with the Windows Sockets API (Winsock2) for reliable client-server communication.
* **Thread-Safe Data Management:** Data integrity is ensured using `std::mutex` to prevent race conditions when multiple clients access or modify transport data.
* **Role-Based Access Control:** 
  * **Standard Users:** Can query bus stops, view timetables, and check line routes.
  * **Administrators:** Secure login (custom authorization) to add new stops, create transport lines, and push updates to the live server.
* **Data Persistence (Upgraded):** Migrated from legacy CSV files to a real-time, embedded **SQLite** database (`transport.db`) via a custom `DatabaseManager` wrapper.
* **Robust Input Validation:** "Foolproof" console input handling and custom exception management (e.g., `MissingDataException`) to prevent server crashes on invalid client requests.

## 🛠️ Technology Stack

* **Language:** Standard C++ (C++17)
* **Database:** Embedded SQLite3
* **Build System:** CMake (Cross-platform)
* **Networking:** Winsock2 API
* **Concurrency:** `<thread>`, `<mutex>`
* **Architecture:** Client-Server Pattern, Proxy Pattern concepts

## 📂 Project Structure

The repository contains two main execution components:
1. **Server:** Initializes the SQLite database, binds to port `8080`, and listens for incoming connections in a detached thread loop.
2. **Client:** Connects to the server (localhost) and provides an interactive CLI menu for the user. Features an auto-reconnect loop if the server is offline.

## ⚙️ How to Build and Run (Cross-Platform)

The project now uses CMake, making it fully independent of Visual Studio and ready to be compiled on any OS.

1. **Clone the repository:** 
   git clone https://github.com/JordaniKar/Transport-manager.git

2. **Generate the build files and compile:**
   cd Transport-manager
   mkdir build
   cd build
   cmake ..
   cmake --build .

3. **Run the application:**
   * Start the Server executable (`Debug\CityTransport.exe` on Windows).
   * Open a second terminal and start the Client executable to connect.

## 🗺️ Roadmap & Milestones

- [x] Migrate build system from Visual Studio to **CMake** for cross-platform support. *(Completed)*
- [x] Replace CSV persistence with a lightweight **SQLite** database. *(Completed)*
