#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <array>
#include <cstdint>
#include <algorithm>
#include "lib.h"

#ifdef _WIN32
    #include <windows.h>
#endif

#include <fstream>

void setupConsole() {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);   
        SetConsoleCP(CP_UTF8);          
        setlocale(LC_ALL, "Russian");   
        setlocale(LC_ALL, "ru_RU.UTF-8"); 
    #endif
    std::ios_base::sync_with_stdio(false); 
    std::cin.tie(nullptr);                 
}

// std::vector<std::string> readLinesFromFile(const std::string& filename) {
//     std::vector<std::string> lines;
//     std::ifstream file(filename);
//     if (!file.is_open()) {
//         throw std::runtime_error("Cannot open file: " + filename);
//     }
//     std::string line;
//     while (std::getline(file, line)) {
//         // Удаляем символ возврата каретки (\r) для Windows-файлов
//         if (!line.empty() && line.back() == '\r') {
//             line.pop_back();
//         }
//         lines.push_back(line);
//     }
//     return lines;
// }

void processInput(std::istream& in, std::vector<std::array<uint8_t, 4>>& ipVector) {
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;

        // Находим первую табуляцию, чтобы отсечь text2 и text3
        size_t tab_pos = line.find('\t');
        std::string ip_part = (tab_pos != std::string::npos) ? line.substr(0, tab_pos) : line;

        // Заменяем точки на пробелы для удобного чтения через stringstream
        std::replace(ip_part.begin(), ip_part.end(), '.', ' ');
        std::stringstream ss(ip_part);
        
        int a, b, c, d;
        if (ss >> a >> b >> c >> d) {
            ipVector.push_back({
                static_cast<uint8_t>(a),
                static_cast<uint8_t>(b),
                static_cast<uint8_t>(c),
                static_cast<uint8_t>(d)
            });
        }
    }
}

int main(int argc, char const *argv[])
{

    setupConsole();

    std::vector<std::array<uint8_t, 4>> ipVector;

    if (argc > 1) {
        // Режим отладки: передаем файл аргументом (./ip_filter test.tsv)
        std::ifstream file(argv[1]);
        if (!file) {
            std::cerr << "Error: " << argv[1] << " not found" << std::endl;
            return 1;
        }
        processInput(file, ipVector);
    } else {
        // Режим сдачи: читаем из pipe (cat data.tsv | ./ip_filter)
        processInput(std::cin, ipVector);
    }

    std::sort(ipVector.begin(), ipVector.end(), [](const auto& a, const auto& b) { return a > b; });

    auto print_ip = [](const auto& ip) 
    {
        std::cout << (int)ip[0] << "." << (int)ip[1] << "." 
                  << (int)ip[2] << "." << (int)ip[3] << "\n";
    };

    // for (const auto& ip : ipVector) {
    //     std::cout << (int)ip[0] << "." << (int)ip[1] << "." << (int)ip[2] << "." << (int)ip[3] << "\n";
    // }
    for (const auto& ip : ipVector) print_ip(ip);

    // // 2. Первый байт равен 1
    // for (const auto& ip : ipVector) {
    //     if (ip[0] == 1) {
    //         std::cout << (int)ip[0] << "." << (int)ip[1] << "." << (int)ip[2] << "." << (int)ip[3] << "\n";
    //     }
    // }

    // // 3. Первый байт 46, второй 70
    // for (const auto& ip : ipVector) {
    //     if (ip[0] == 46 && ip[1] == 70) {
    //         std::cout << (int)ip[0] << "." << (int)ip[1] << "." << (int)ip[2] << "." << (int)ip[3] << "\n";
    //     }
    // }

    // // 4. Любой байт равен 46 (используем std::any_of из C++11)
    // for (const auto& ip : ipVector) {
    //     if (std::any_of(ip.begin(), ip.end(), [](uint8_t byte){ return byte == 46; })) {
    //         std::cout << (int)ip[0] << "." << (int)ip[1] << "." << (int)ip[2] << "." << (int)ip[3] << "\n";
    //     }
    // }

    // 2. Первый байт равен 1
    for (const auto& ip : ipVector) {
        if (ip[0] == 1) print_ip(ip);
    }

    // 3. Первый байт 46, второй 70
    for (const auto& ip : ipVector) {
        if (ip[0] == 46 && ip[1] == 70) print_ip(ip);
    }

    // 4. Любой байт равен 46
    for (const auto& ip : ipVector) {
        if (std::any_of(ip.begin(), ip.end(), [](uint8_t byte){ return byte == 46; })) {
            print_ip(ip);
        }
    }

    // auto lines = readLinesFromFile("ip_filter.tsv");
    // std::cout << "Прочитано строк: " << lines.size() << "\n\n";

    // std::vector<std::array<uint8_t, 4>> ipVector;
    // ipVector.reserve(lines.size());

    // try
    // {
    //     for (const auto& line : lines) 
    //     {
    //         if (line.empty()) continue;
    //             std::stringstream ss(line);
    //             int part[4];
    //             char dot;
    //             int dummy; // Для остальных чисел в строке
    //             if (ss >> part[0] >> dot >> part[1] >> dot >> part[2] >> dot >> part[3]) {
    //                 ipVector.push_back({
    //                     static_cast<uint8_t>(part[0]),
    //                     static_cast<uint8_t>(part[1]),
    //                     static_cast<uint8_t>(part[2]),
    //                     static_cast<uint8_t>(part[3])
    //                 }); 
    //             }
    //     }
    //     std::cout << "распарсили файл, взяли ip адрес в вектор массивов" << "\n\n";

    //     std::sort(ipVector.begin(), ipVector.end(), [](const auto& a, const auto& b) {
    //         return a > b; 
    //     });

    // }
    // catch(const std::exception &e)
    // {
    //     std::cerr << e.what() << std::endl;
    //     return -1;
    // }

    return 0;
}
