#define NOMINMAX
#pragma comment(lib, "ws2_32.lib") 
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>
#include <vector>
#include <limits>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "TransportManager.h"
#include "BusStop.h"
#include "TransitLine.h"
#include "TransportExceptions.h"

std::mutex dbMutex;

void clearScreen() {
    system("cls"); 
}


void handleClient(SOCKET clientSocket, TransportManager* manager) {
    char buffer[4096];
    bool isAdmin = false; 

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0) {
            std::cout << "[Server] A client disconnected.\n";
            break;
        }

        std::string request(buffer);
        request.erase(request.find_last_not_of(" \n\r\t") + 1);
        std::stringstream response;
        std::lock_guard<std::mutex> lock(dbMutex);

        try {
            if (request.find("TIMETABLE ") == 0) {
                std::string code = request.substr(10);
                response << manager->getTimetable(code);
            }
            else if (request.find("ROUTE ") == 0) {
                std::string lineNum = request.substr(6);
                response << manager->getLine(lineNum) << "\n";
            }
            else if (request.find("LOGIN ") == 0) {
                std::string pass = request.substr(6);
                if (pass == "admin123") {
                    isAdmin = true;
                    response << "SUCCESS";
                }
                else {
                    response << "ERROR: Invalid password.";
                }
            }
            else if (request.find("ADD_STOP ") == 0) {
                if (!isAdmin) {
                    response << "ERROR: Unauthorized. Admin access required.";
                }
                else {
                    std::string data = request.substr(9);
                    std::stringstream ss(data);
                    std::string code, name, address, latStr, lonStr;

                    std::getline(ss, code, '|');
                    std::getline(ss, name, '|');
                    std::getline(ss, address, '|');
                    std::getline(ss, latStr, '|');
                    std::getline(ss, lonStr, '|');

                    manager->addStop(BusStop(code, name, address, std::stod(latStr), std::stod(lonStr)));
                    response << "SUCCESS: Stop " << name << " added to server database.";
                }
            }
            else if (request.find("ADD_LINE ") == 0) {
                if (!isAdmin) {
                    response << "ERROR: Unauthorized. Admin access required.";
                }
                else {
                    std::string data = request.substr(9);
                    std::stringstream ss(data);
                    std::string num, type, intStr, shStr, smStr, ehStr, emStr, stopsStr;

                    std::getline(ss, num, '|');
                    std::getline(ss, type, '|');
                    std::getline(ss, intStr, '|');
                    std::getline(ss, shStr, '|');
                    std::getline(ss, smStr, '|');
                    std::getline(ss, ehStr, '|');
                    std::getline(ss, emStr, '|');
                    std::getline(ss, stopsStr, '|');

                    TransitLine newLine(num, type, std::stoi(intStr), std::stoi(shStr),
                        std::stoi(smStr), std::stoi(ehStr), std::stoi(emStr));

                    std::stringstream stopsStream(stopsStr);
                    std::string stopCode;
                    while (std::getline(stopsStream, stopCode, ',')) {
                        newLine.addStop(manager->getStop(stopCode)); 
                    }

                    manager->addLine(newLine);
                    response << "SUCCESS: Line " << num << " added to server database.";
                }
            }
            else if (request.find("APPEND_STOP ") == 0) {
                if (!isAdmin) {
                    response << "ERROR: Unauthorized. Admin access required.";
                }
                else {
                    std::string data = request.substr(12);
                    std::stringstream ss(data);
                    std::string lineNum, stopCode;

                    std::getline(ss, lineNum, '|');
                    std::getline(ss, stopCode, '|');
                    manager->addStopToLine(lineNum, stopCode);
                    response << "SUCCESS: Stop " << stopCode << " appended to line " << lineNum << ".";
                }
            }
            else if (request == "SAVE") {
                if (!isAdmin) {
                    response << "ERROR: Unauthorized.";
                }
                else {
                    manager->saveToCSV("stops.csv", "lines.csv");
                    response << "SUCCESS: Database saved to server files.";
                }
            }
            else if (request == "QUIT") {
                break;
            }
            else {
                response << "ERROR: Unknown command.";
            }
        }
        catch (const MissingDataException& e) {
            response << "ERROR: Missing data - " << e.what();
        }
        catch (const std::exception& e) {
            response << "SERVER ERROR: " << e.what();
        }

        std::string resStr = response.str();
        send(clientSocket, resStr.c_str(), resStr.size(), 0);
    }
    closesocket(clientSocket);
}

void runServer() {
    TransportManager manager;
    try {
        manager.loadFromCSV("stops.csv", "lines.csv");
        std::cout << "[Server] Database loaded successfully!\n";
    }
    catch (...) {
        std::cout << "[Server] Starting with empty database.\n";
    }

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);

    std::cout << "[Server] Listening for clients on port 8080...\n";

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket != INVALID_SOCKET) {
            std::cout << "[Server] New client connected!\n";
            std::thread clientThread(handleClient, clientSocket, &manager);
            clientThread.detach();
        }
    }
    closesocket(serverSocket);
    WSACleanup();
}


std::string sendRequest(SOCKET sock, const std::string& request) {
    send(sock, request.c_str(), request.size(), 0);
    char buffer[8192];
    memset(buffer, 0, sizeof(buffer));
    int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) return std::string(buffer);
    return "ERROR: Server disconnected.";
}

void runClient() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[Client] Connection failed! Make sure the Server is running first.\n";
        return;
    }

    bool running = true;
    int choice = 0;
    bool isAdmin = false;

    while (running) {
        std::cout << "\n=========================================\n";
        std::cout << "       URBAN TRANSPORT - MAIN MENU       \n";
        std::cout << "=========================================\n";
        std::cout << "1. Show electronic timetable for a stop\n";
        std::cout << "2. Show route of a line\n";

        if (!isAdmin) {
            std::cout << "3. [Dispatcher / Admin Login]\n";
        }
        else {
            std::cout << "4. [Admin] Add new bus stop\n";
            std::cout << "5. [Admin] Create new transit line\n";
            std::cout << "6. [Admin] Save database to server files\n";
            std::cout << "7. [Admin] Add existing stop to existing line\n";
            std::cout << "8. [Admin] Logout\n";
        }
        std::cout << "0. Exit\n";
        std::cout << "-----------------------------------------\n";
        std::cout << "Your choice: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "\n[Error] Please enter a valid number!\n";
            continue;
        }

        std::string inputCode, response;

        switch (choice) {
        case 1:
            std::cout << "\nEnter stop code (e.g., S001): ";
            std::cin >> inputCode;
            clearScreen();
            std::cout << sendRequest(clientSocket, "TIMETABLE " + inputCode) << "\n";
            break;

        case 2:
            std::cout << "\nEnter line number (e.g., 94): ";
            std::cin >> inputCode;
            clearScreen();
            std::cout << sendRequest(clientSocket, "ROUTE " + inputCode) << "\n";
            break;

        case 3:
            if (!isAdmin) {
                std::cout << "\nEnter admin password: ";
                std::cin >> inputCode;
                response = sendRequest(clientSocket, "LOGIN " + inputCode);
                if (response == "SUCCESS") {
                    isAdmin = true;
                    clearScreen();
                    std::cout << "\n[Success] Logged in as Administrator!\n";
                }
                else {
                    std::cout << "\n[Error] Incorrect password!\n";
                }
            }
            break;

        case 4:
            if (isAdmin) {
                std::string code, name, addr;
                double lat, lon;
                std::cout << "\n--- ADD NEW STOP ---\n";
                std::cout << "Code (e.g., S005): "; std::cin >> code;
                std::cin.ignore();
                std::cout << "Name: "; std::getline(std::cin, name);
                std::cout << "Address: "; std::getline(std::cin, addr);
                std::cout << "Latitude: "; std::cin >> lat;
                std::cout << "Longitude: "; std::cin >> lon;

              
                std::stringstream req;
                req << "ADD_STOP " << code << "|" << name << "|" << addr << "|" << lat << "|" << lon;
                std::cout << "\nServer response: " << sendRequest(clientSocket, req.str()) << "\n";
            }
            break;

        case 5:
            if (isAdmin) {
                std::string num, type, stops = "";
                int interval, sh, sm, eh, em;

                std::cout << "\n--- CREATE NEW LINE ---\n";
                std::cout << "Number (e.g., 280): "; std::cin >> num;
                std::cout << "Type (bus/tram): "; std::cin >> type;
                std::cout << "Interval (minutes): "; std::cin >> interval;
                std::cout << "Start Hour & Minute (e.g., 5 30): "; std::cin >> sh >> sm;
                std::cout << "End Hour & Minute (e.g., 23 0): "; std::cin >> eh >> em;

                std::cout << "\nEnter stop codes for the route. Enter '0' to finish.\n";
                while (true) {
                    std::string stopCode;
                    std::cout << "Next stop code (or 0): "; std::cin >> stopCode;
                    if (stopCode == "0") break;
                    if (!stops.empty()) stops += ",";
                    stops += stopCode;
                }

                std::stringstream req;
                req << "ADD_LINE " << num << "|" << type << "|" << interval << "|" << sh << "|"
                    << sm << "|" << eh << "|" << em << "|" << stops;
                std::cout << "\nServer response: " << sendRequest(clientSocket, req.str()) << "\n";
            }
            break;

        case 6:
            if (isAdmin) {
                std::cout << "\nServer response: " << sendRequest(clientSocket, "SAVE") << "\n";
            }
            break;
        case 7:
            if (isAdmin) {
                std::string lineNum, stopCode;
                std::cout << "\n--- ADD STOP TO LINE ---\n";
                std::cout << "Line Number (e.g., 280): ";
                std::cin >> lineNum;
                std::cout << "Stop Code (e.g., S101): ";
                std::cin >> stopCode;

                std::stringstream req;
                req << "APPEND_STOP " << lineNum << "|" << stopCode;
                std::cout << "\nServer response: " << sendRequest(clientSocket, req.str()) << "\n";
            }
            break;
        case 8:
            if (isAdmin) {
                isAdmin = false;
                clearScreen();
                std::cout << "\n[System] Logged out of admin panel.\n";
            }
            break;

        case 0:
            sendRequest(clientSocket, "QUIT");
            std::cout << "[System] Disconnecting from server. Goodbye!\n";
            running = false;
            break;

        default:
            std::cout << "\n[Error] Invalid option!\n";
            break;
        }
    }

    closesocket(clientSocket);
    WSACleanup();
}


int main() {
    int mode;
    std::cout << "=== TRANSPORT SYSTEM CLIENT-SERVER ===\n";
    std::cout << "1. Start Server (Backend)\n";
    std::cout << "2. Start Client (Frontend UI)\n";
    std::cout << "Select mode: ";
    if (!(std::cin >> mode)) return 1;

    std::cin.ignore();

    if (mode == 1) {
        runServer();
    }
    else if (mode == 2) {
        runClient();
    }
    else {
        std::cout << "Invalid mode selected.\n";
    }

    return 0;
}