#pragma once

#include "FileHasher.hpp"
#include <map>
#include <vector>
#include <string>

class DuplicateFinder {
public:
    using HashSeq = FileHasher::HashSeq;

    void addFile(const std::string& path, size_t blockSize, const std::string& algo);

    // После добавления всех файлов вызывать этот метод для вывода результатов
    void printResults() const;

    // Очистить внутренние данные
    void clear();

private:
    // Ключ – последовательность хэшей (представленная строкой), значение – список файлов
    std::map<HashSeq, std::vector<std::string>> groups_;
};