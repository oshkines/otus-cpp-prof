#include "DuplicateFinder.hpp"
#include <iostream>

void DuplicateFinder::addFile(const std::string& path, size_t blockSize, const std::string& algo) {
    FileHasher newHasher(path, blockSize, algo);
    for (auto& entry : groups_) {
        // Для представителя группы не вычисляем хэши заранее – они вычислятся при сравнении
        FileHasher repHasher(entry.second.front(), blockSize, algo);
        if (newHasher.compareTo(repHasher)) {
            entry.second.push_back(path);
            return;
        }
    }
    // Файл уникален – нужно вычислить все хэши, чтобы использовать как ключ
    newHasher.getFullHashSeq();
    groups_[newHasher.getFullHashSeq()].push_back(path);
}


void DuplicateFinder::printResults() const {
    bool firstGroup = true;
    for (const auto& entry : groups_) {
        if (entry.second.size() < 2) continue;
        if (!firstGroup) std::cout << "\n";
        firstGroup = false;
        for (const auto& file : entry.second) {
            std::cout << file << "\n";
        }
    }
}