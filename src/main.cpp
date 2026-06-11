#include "Options.hpp"
#include "DuplicateFinder.hpp"
#include <boost/filesystem.hpp>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = boost::filesystem;

void scanDirectory(const fs::path& dir, const Options& opt, DuplicateFinder& finder, int currentDepth) {
    if (currentDepth > opt.depth) return;
    if (std::find(opt.exclude.begin(), opt.exclude.end(), dir.string()) != opt.exclude.end()) return;

    for (auto& entry : fs::directory_iterator(dir)) {
        if (fs::is_directory(entry.path())) {
            scanDirectory(entry.path(), opt, finder, currentDepth + 1);
        } else if (fs::is_regular_file(entry.path())) {
            auto size = fs::file_size(entry.path());
            if (size < opt.min_size) continue;
            // Проверка маски (упрощённо, можно использовать boost::algorithm::wildcard)
            // Для простоты пропускаем реализацию маски – можно добавить позже
            finder.addFile(entry.path().string(), opt.block_size, opt.hash_algo);
        }
    }
}

int main(int argc, char* argv[]) {

    // Настройка кодировки для Windows
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif

    Options opt;
    if (!opt.parse(argc, argv)) return 1;

    DuplicateFinder finder;
    for (const auto& dir : opt.dirs) {
        if (!fs::exists(dir) || !fs::is_directory(dir)) {
            std::cerr << "Предупреждение: директория " << dir << " не существует или не является папкой\n";
            continue;
        }
        scanDirectory(dir, opt, finder, 0);
    }
    finder.printResults();
    return 0;
}