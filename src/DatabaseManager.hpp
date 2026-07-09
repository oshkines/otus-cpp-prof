#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>
#include <memory>
#include <mutex>

struct Row {
    int id;
    std::string nameA;
    std::string nameB;
};

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();

    // Команды
    bool insert(const std::string& table, int id, const std::string& name, std::string& error);
    bool truncate(const std::string& table, std::string& error);

    // Операции
    std::vector<Row> intersection(std::string& error);
    std::vector<Row> symmetricDifference(std::string& error);

private:
    sqlite3* db_;
    std::mutex mutex_;

    bool executeQuery(const std::string& query, std::string& error);
    bool prepareTables(std::string& error);
};