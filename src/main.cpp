#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <iomanip>
#include <cstdlib>
#include "csv_parser.hpp"
#include "statistics.hpp"

// Объявляем функции из mapper.cpp и reducer.cpp
int mapper_main();
int reducer_main();

enum class Mode {
    MAPPER,
    REDUCER,
    LOCAL,
    STATS,
    HELP
};

Mode parseMode(const std::string& mode) {
    if (mode == "mapper") return Mode::MAPPER;
    if (mode == "reducer") return Mode::REDUCER;
    if (mode == "local") return Mode::LOCAL;
    if (mode == "stats") return Mode::STATS;
    return Mode::HELP;
}

void runLocal(const std::string& input_file) {
    std::ifstream file(input_file);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << input_file << std::endl;
        return;
    }
    
    std::vector<KeyValue> mapped;
    std::string line;
    bool is_first = true;
    size_t processed = 0;
    
    while (std::getline(file, line)) {
        if (is_first) {
            is_first = false;
            continue;
        }
        
        auto fields = csv::parseLine(line);
        if (fields.size() > 9 && !fields[9].empty()) {
            try {
                double price = std::stod(fields[9]);
                if (price >= 0) {
                    Stats stats;
                    stats.add(price);
                    mapped.push_back({"price", stats});
                    processed++;
                }
            } catch (...) {
                // Игнорируем ошибки
            }
        }
    }
    file.close();
    
    // Shuffle - сортировка по ключу
    std::sort(mapped.begin(), mapped.end(), 
        [](const KeyValue& a, const KeyValue& b) {
            return a.key < b.key;
        });
    
    // Reduce - агрегация
    std::map<std::string, Stats> reduced;
    for (const auto& kv : mapped) {
        reduced[kv.key].merge(kv.stats);
    }
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n=== MapReduce Results ===" << std::endl;
    std::cout << "Processed entries: " << processed << std::endl;
    std::cout << "------------------------" << std::endl;
    
    for (const auto& pair : reduced) {
        const std::string& key = pair.first;
        const Stats& stats = pair.second;
        std::cout << "Key: " << key << std::endl;
        std::cout << "  Mean:      " << stats.mean() << std::endl;
        std::cout << "  Variance:  " << stats.variance() << std::endl;
        std::cout << "  Std Dev:   " << stats.stddev() << std::endl;
        std::cout << "  Count:     " << stats.count << std::endl;
    }
}

void computeStats(const std::string& input_file) {
    std::ifstream file(input_file);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << input_file << std::endl;
        return;
    }
    
    Stats stats;
    std::string line;
    bool is_first = true;
    size_t processed = 0;
    
    while (std::getline(file, line)) {
        if (is_first) {
            is_first = false;
            continue;
        }
        
        auto fields = csv::parseLine(line);
        if (fields.size() > 9 && !fields[9].empty()) {
            try {
                double price = std::stod(fields[9]);
                if (price >= 0) {
                    stats.add(price);
                    processed++;
                }
            } catch (...) {
                // Игнорируем ошибки
            }
        }
    }
    file.close();
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n=== Direct Statistics (Verification) ===" << std::endl;
    std::cout << "Processed entries: " << processed << std::endl;
    std::cout << "------------------------" << std::endl;
    std::cout << "Mean:      " << stats.mean() << std::endl;
    std::cout << "Variance:  " << stats.variance() << std::endl;
    std::cout << "Std Dev:   " << stats.stddev() << std::endl;
    std::cout << "Count:     " << stats.count << std::endl;
    std::cout << "Sum:       " << stats.sum << std::endl;
    std::cout << "Sum Sq:    " << stats.sum_sq << std::endl;
}

void printHelp() {
    std::cout << "MapReduce - Airbnb Price Statistics" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  mapreduce <mode> [input_file]" << std::endl;
    std::cout << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "  mapper           - Run as mapper (reads from stdin)" << std::endl;
    std::cout << "  reducer          - Run as reducer (reads from stdin)" << std::endl;
    std::cout << "  local <file>     - Run full pipeline locally" << std::endl;
    std::cout << "  stats <file>     - Compute stats directly (verification)" << std::endl;
    std::cout << "  help             - Show this help" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  cat input.csv | mapreduce mapper | sort | mapreduce reducer" << std::endl;
    std::cout << "  mapreduce local input/AB_NYC_2019.csv" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return 1;
    }
    
    Mode mode = parseMode(argv[1]);
    
    switch (mode) {
        case Mode::MAPPER:
            return mapper_main();
            
        case Mode::REDUCER:
            return reducer_main();
            
        case Mode::LOCAL:
            if (argc < 3) {
                std::cerr << "Error: Missing input file" << std::endl;
                return 1;
            }
            runLocal(argv[2]);
            break;
            
        case Mode::STATS:
            if (argc < 3) {
                std::cerr << "Error: Missing input file" << std::endl;
                return 1;
            }
            computeStats(argv[2]);
            break;
            
        default:
            printHelp();
            return 1;
    }
    
    return 0;
}