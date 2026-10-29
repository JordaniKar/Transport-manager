# 🚌 Urban Transit Management System

A robust, multi-threaded Client-Server application built in C++ for managing urban public transport networks. This project demonstrates low-level network programming, concurrent client handling, and custom text-based protocol parsing.

## 🚀 Key Features

* **Multi-threaded Architecture:** The server utilizes `std::thread` to handle multiple client connections simultaneously without blocking.
* **TCP/IP Sockets:** Direct integration with the Windows Sockets API (Winsock2) for reliable client-server communication.
* **Thread-Safe Data Management:** Data integrity is ensured using `std::mutex` to prevent race conditions when multiple clients access or modify transport data.
* **Role-Based Access Control:** 
  * **Standard Users:** Can query bus stops, view timetables, and check line routes.
  * **Administrators:** Secure login (custom authorization) to add new stops, create transport lines, and push updates to the live server.
* **Data Persistence:** In-memory `std::unordered_map` data structures are serialized and saved to CSV files (`stops.csv`, `lines.csv`) for persistent storage.
* **Robust Input Validation:** "Foolproof" console input handling and custom exception management (e.g., `MissingDataException`) to prevent server crashes on invalid client requests.

## 🛠️ Technology Stack

* **Language:** Standard C++ (C++11/14 features)
* **Networking:** Winsock2 API
* **Concurrency:** `<thread>`, `<mutex>`
* **Architecture:** Client-Server Pattern, Proxy Pattern concepts

## 📂 Project Structure

The repository contains two main execution modes:
1. **Server:** Initializes the database from CSV, binds to port `8080`, and listens for incoming connections in a detached thread loop.
2. **Client:** Connects to the server (localhost) and provides an interactive CLI menu for the user. Features an auto-reconnect loop if the server is offline.

## ⚙️ How to Run

*Currently, the project is compiled via Visual Studio. (CMake support coming soon!)*

1. Clone the repository: `git clone https://github.com/JordaniKar/UrbanTransit-CPP.git`
2. Open the solution in Visual Studio.
3. Build the project.
4. Run the executable. Select option `1` to start the Server.
5. Open a second terminal/console and run the executable again. Select option `2` to start the Client.

## 🗺️ Roadmap (Future Enhancements)

- [ ] Migrate build system from Visual Studio to **CMake** for cross-platform support.
- [ ] Replace CSV persistence with a lightweight **SQLite** database.
- [ ] Add **Unit Tests** (e.g., GoogleTest) for core business logic.