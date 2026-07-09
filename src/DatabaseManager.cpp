#include "DatabaseManager.hpp"
#include <iostream>
#include <sstream>

DatabaseManager::DatabaseManager() : db_(nullptr) {
    std::string error;
    if (sqlite3_open(":memory:", &db_) != SQLITE_OK) {
        throw std::runtime_error("Не удалось открыть базу данных в памяти");
    }
    if (!prepareTables(error)) {
        throw std::runtime_error("Не удалось создать таблицы: " + error);
    }
}

DatabaseManager::~DatabaseManager() {
    if (db_) {
        sqlite3_close(db_);
    }
}

bool DatabaseManager::prepareTables(std::string& error) {
    const std::string query =
        "CREATE TABLE A (id INTEGER PRIMARY KEY, name TEXT);"
        "CREATE TABLE B (id INTEGER PRIMARY KEY, name TEXT);";
    return executeQuery(query, error);
}

bool DatabaseManager::executeQuery(const std::string& query, std::string& error) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, query.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        error = errMsg ? errMsg : "Unknown error";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool DatabaseManager::insert(const std::string& table, int id, const std::string& name, std::string& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream query;
    query << "INSERT INTO " << table << " (id, name) VALUES (" << id << ", '" << name << "');";
    return executeQuery(query.str(), error);
}

bool DatabaseManager::truncate(const std::string& table, std::string& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream query;
    query << "DELETE FROM " << table << ";";
    return executeQuery(query.str(), error);
}

std::vector<Row> DatabaseManager::intersection(std::string& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<Row> result;
    const std::string query =
        "SELECT A.id, A.name, B.name FROM A INNER JOIN B ON A.id = B.id ORDER BY A.id;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        error = sqlite3_errmsg(db_);
        return result;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Row row;
        row.id = sqlite3_column_int(stmt, 0);
        row.nameA = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        row.nameB = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        result.push_back(row);
    }

    sqlite3_finalize(stmt);
    return result;
}

std::vector<Row> DatabaseManager::symmetricDifference(std::string& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<Row> result;
    
    // Получаем id, которые есть только в A или только в B
    const std::string query =
        "SELECT id, name, '' FROM A WHERE id NOT IN (SELECT id FROM B) "
        "UNION "
        "SELECT id, '', name FROM B WHERE id NOT IN (SELECT id FROM A) "
        "ORDER BY id;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        error = sqlite3_errmsg(db_);
        return result;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Row row;
        row.id = sqlite3_column_int(stmt, 0);
        const char* nameA = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* nameB = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        row.nameA = nameA ? nameA : "";
        row.nameB = nameB ? nameB : "";
        result.push_back(row);
    }

    sqlite3_finalize(stmt);
    return result;
}