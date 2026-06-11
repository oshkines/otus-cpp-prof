#include "Options.hpp"
#include <iostream>

bool Options::parse(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "показать справку")
        ("dirs", po::value<std::vector<std::string>>(&dirs)->multitoken()->required(), "директории для сканирования")
        ("exclude", po::value<std::vector<std::string>>(&exclude)->multitoken(), "директории для исключения")
        ("depth", po::value<int>(&depth)->default_value(0), "глубина рекурсии")
        ("min-size", po::value<size_t>(&min_size)->default_value(1), "минимальный размер файла")
        ("mask", po::value<std::string>(&mask)->default_value("*"), "маска имени файла")
        ("block-size", po::value<size_t>(&block_size)->default_value(4096), "размер блока")
        ("hash", po::value<std::string>(&hash_algo)->default_value("crc32"), "алгоритм хэша (crc32 или md5)")
    ;

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        if (vm.count("help")) {
            printHelp();
            return false;
        }
        po::notify(vm);
    } catch (std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
        printHelp();
        return false;
    }

    if (hash_algo != "crc32" && hash_algo != "md5") {
        std::cerr << "Ошибка: алгоритм хэширования должен быть crc32 или md5\n";
        return false;
    }
    return true;
}

void Options::printHelp() const {
    std::cout << "bayan – утилита поиска дубликатов файлов\n"
              << "Использование:\n"
              << "  bayan --dirs path1 path2 ... [--exclude excl1 excl2 ...] [--depth N] [--min-size bytes] [--mask pattern] [--block-size S] [--hash crc32|md5]\n"
              << "Пример:\n"
              << "  bayan --dirs ./test ./data --exclude ./test/temp --depth 2 --min-size 1024 --mask \"*.txt\" --block-size 4096 --hash md5\n";
}