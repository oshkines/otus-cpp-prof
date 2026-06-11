#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <fstream>

class FileHasher {
public:
    using HashValue = uint32_t;
    using HashSeq = std::vector<HashValue>;

    FileHasher(const std::string& path, size_t blockSize, const std::string& algo);
    ~FileHasher() = default;

    // Сравнить с другим файлом поблочно; возвращает true, если файлы идентичны.
    bool compareTo(FileHasher& other);

    // Получить полную последовательность (если понадобится – для отладки, но не для сравнения)
    const HashSeq& getFullHashSeq();

    const std::string& getPath() const { return path_; }

private:
    std::string path_;
    size_t blockSize_;
    std::string algo_;
    mutable HashSeq hashSeq_;          // кэш вычисленных хэшей (заполняется по мере чтения)
    mutable bool fullyHashed_ = false;
    mutable std::ifstream file_;

    HashValue hashBlock(const char* data, size_t len);
    void ensureBlock(size_t blockIndex);   // читает блок с индексом blockIndex, если ещё не прочитан
};