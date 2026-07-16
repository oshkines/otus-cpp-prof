#pragma once

#include <string>
#include <sstream>
#include <cmath>

/**
 * @brief Структура для хранения статистических данных
 */
struct Stats {
    double sum = 0.0;
    size_t count = 0;
    double sum_sq = 0.0; // Сумма квадратов для дисперсии
    
    /**
     * @brief Добавляет значение к статистике
     */
    void add(double value);
    
    /**
     * @brief Объединяет две статистики
     */
    void merge(const Stats& other);
    
    /**
     * @brief Возвращает среднее значение
     */
    double mean() const;
    
    /**
     * @brief Возвращает дисперсию
     */
    double variance() const;
    
    /**
     * @brief Возвращает среднеквадратическое отклонение
     */
    double stddev() const;
    
    /**
     * @brief Сериализует статистику в строку
     */
    std::string serialize() const;
    
    /**
     * @brief Десериализует статистику из строки
     */
    static Stats deserialize(const std::string& str);
};

/**
 * @brief Структура для хранения данных между маппером и редуктором
 */
struct KeyValue {
    std::string key;
    Stats stats;
    
    std::string serialize() const;
    static KeyValue deserialize(const std::string& line);
};