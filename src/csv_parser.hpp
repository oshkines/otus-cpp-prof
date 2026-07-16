#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace csv {

    /**
     * @brief Парсит CSV строку с учётом кавычек
     * @param line Строка CSV
     * @return Вектор полей
     */
    inline std::vector<std::string> parseLine(const std::string& line) {
        std::vector<std::string> fields;
        std::string field;
        bool in_quotes = false;
        
        for (char c : line) {
            if (c == '"') {
                in_quotes = !in_quotes;
            } else if (c == ',' && !in_quotes) {
                fields.push_back(field);
                field.clear();
            } else {
                field += c;
            }
        }
        fields.push_back(field);
        
        return fields;
    }

    /**
     * @brief Проверяет, является ли строка заголовком
     */
    inline bool isHeader(const std::string& line) {
        return line.find("id") == 0 && line.find("price") != std::string::npos;
    }

} // namespace csv