#pragma once
#include <string>
#include "sqlite3.h"
#include <vector>
#include <unordered_map>

class DatabaseManager {
private:
    sqlite3* db;
public:
    DatabaseManager();
    ~DatabaseManager();
    void initialize();
    bool executeQuery(const std::string& query);
    std::vector<std::unordered_map<std::string, std::string>> selectQuery(const std::string& query);
};

