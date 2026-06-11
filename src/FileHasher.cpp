#include "FileHasher.hpp"
#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <cstring>

FileHasher::FileHasher(const std::string& path, size_t blockSize, const std::string& algo)
    : path_(path), blockSize_(blockSize), algo_(algo), fullyHashed_(false) {
    file_.open(path_, std::ios::binary);
}

FileHasher::HashValue FileHasher::hashBlock(const char* data, size_t len) {
    if (algo_ == "crc32") {
        boost::crc_32_type crc;
        crc.process_bytes(data, len);
        return crc.checksum();
    } else {
        boost::uuids::detail::md5 md5;
        md5.process_bytes(data, len);
        boost::uuids::detail::md5::digest_type digest;
        md5.get_digest(digest);
        return *reinterpret_cast<HashValue*>(&digest);
    }
}

void FileHasher::ensureBlock(size_t blockIndex) {
    if (blockIndex < hashSeq_.size()) return;   // уже есть
    if (fullyHashed_) return;                  // файл полностью прочитан
    if (!file_.is_open()) return;

    char* buffer = new char[blockSize_];
    file_.seekg(blockIndex * blockSize_);
    if (file_.read(buffer, blockSize_) || file_.gcount() > 0) {
        size_t bytes = file_.gcount();
        if (bytes == 0) {
            fullyHashed_ = true;
            delete[] buffer;
            return;
        }
        if (bytes < blockSize_) {
            std::memset(buffer + bytes, 0, blockSize_ - bytes);
            fullyHashed_ = true;
        }
        hashSeq_.push_back(hashBlock(buffer, blockSize_));
    } else {
        fullyHashed_ = true;  // ошибка или конец файла
    }
    delete[] buffer;
}

bool FileHasher::compareTo(FileHasher& other) {
    size_t i = 0;
    while (true) {
        ensureBlock(i);
        other.ensureBlock(i);

        bool iEnd = (i >= hashSeq_.size());
        bool jEnd = (i >= other.hashSeq_.size());

        if (iEnd && jEnd) return true;   // оба закончились – одинаковы
        if (iEnd || jEnd) return false;  // один закончился раньше – разные

        if (hashSeq_[i] != other.hashSeq_[i]) return false;
        ++i;
    }
}

const FileHasher::HashSeq& FileHasher::getFullHashSeq() {
    while (!fullyHashed_) {
        ensureBlock(hashSeq_.size());
    }
    return hashSeq_;
}