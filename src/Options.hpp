#pragma once

#include <boost/program_options.hpp>
#include <string>
#include <vector>

namespace po = boost::program_options;

struct Options {
    std::vector<std::string> dirs;           // директории для сканирования
    std::vector<std::string> exclude;        // директории для исключения
    int depth = 0;                           // глубина (0 – без рекурсии)
    size_t min_size = 1;                     // минимальный размер файла
    std::string mask = "*";                  // маска имени (glob)
    size_t block_size = 4096;                // размер блока для чтения S
    std::string hash_algo = "crc32";         // crc32 или md5

    bool parse(int argc, char* argv[]);
    void printHelp() const;
};