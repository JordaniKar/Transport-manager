#include "DatabaseManager.h"
#include <iostream>
#include <stdexcept>
DatabaseManager::DatabaseManager() : db(nullptr) {}
DatabaseManager::~DatabaseManager() {
    if (db) {
        sqlite3_close(db); 
    }
}

void DatabaseManager::initialize() {
    int rc = sqlite3_open("transport.db", &db);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Cannot open database: " + std::string(sqlite3_errmsg(db)));
    }

    const char* sql =
        "CREATE TABLE IF NOT EXISTS Stops ("
        "code TEXT PRIMARY KEY, "
        "name TEXT NOT NULL, "
        "address TEXT, "
        "lat REAL, "
        "lon REAL);"

        "CREATE TABLE IF NOT EXISTS Lines ("
        "number TEXT PRIMARY KEY, "
        "type TEXT, "
        "interval INT, "
        "start_h INT, start_m INT, "
        "end_h INT, end_m INT);"

        "CREATE TABLE IF NOT EXISTS RouteStops ("
        "line_number TEXT, "
        "stop_code TEXT, "
        "stop_order INT, "
        "FOREIGN KEY(line_number) REFERENCES Lines(number), "
        "FOREIGN KEY(stop_code) REFERENCES Stops(code));";

    char* errMsg = nullptr;
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::string errorStr = errMsg;
        sqlite3_free(errMsg);
        throw std::runtime_error("SQL error: " + errorStr);
    }

    std::cout << "[Database] SQLite initialized and tables verified successfully.\n";
}
bool DatabaseManager::executeQuery(const std::string& query) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::string error = errMsg;
        sqlite3_free(errMsg);
        std::cerr << "[Database Error] " << error << " (Query: " << query << ")\n";
        return false;
    }
    return true;
}
std::vector<std::unordered_map<std::string, std::string>> DatabaseManager::selectQuery(const std::string& query) {
    std::vector<std::unordered_map<std::string, std::string>> results;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[Database Error] Failed to prepare statement: " << sqlite3_errmsg(db) << "\n";
        return results;
    }
    int cols = sqlite3_column_count(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::unordered_map<std::string, std::string> row;
        for (int i = 0; i < cols; ++i) {
            const char* colName = sqlite3_column_name(stmt, i);
            const char* colText = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            row[colName] = colText ? colText : "";
        }
        results.push_back(row);
    }
    sqlite3_finalize(stmt);
    return results;
}