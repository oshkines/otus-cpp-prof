#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <map>
#include <tuple>
#include <cstddef>

// Шаблонный класс бесконечной разреженной матрицы
template<typename T, T DefaultValue = T{}>
class Matrix {
private:
    // Внутреннее хранилище занятых ячеек
    std::map<std::pair<int, int>, T> cells_;
    T default_value_ = DefaultValue;

public:
    // Прокси для ячейки (возвращается при обращении к элементу)
    class CellProxy {
        Matrix& matrix_;
        int x_;
        int y_;
    public:
        CellProxy(Matrix& matrix, int x, int y);
        
        // Оператор присваивания – сохраняет значение
        CellProxy& operator=(const T& value);
        
        // Оператор преобразования к T – чтение значения
        operator T() const;
        
        // Для поддержки каскадного присваивания
        CellProxy& operator=(const CellProxy& other);
    };

    // Прокси для строки (возвращается при обращении matrix[x])
    class RowProxy {
        Matrix& matrix_;
        int x_;
    public:
        RowProxy(Matrix& matrix, int x);
        
        // Оператор доступа к столбцу – возвращает прокси для ячейки
        CellProxy operator[](int y);
    };

    // Оператор доступа к строке
    RowProxy operator[](int x);
    
    // Константный доступ (для чтения)
    T at(int x, int y) const;
    
    // Количество занятых ячеек
    size_t size() const;

    class Iterator {
        using MapIterator = typename std::map<std::pair<int, int>, T>::const_iterator;
        MapIterator it_;
    public:
        Iterator(MapIterator it);
        bool operator!=(const Iterator& other) const;
        Iterator& operator++();
        std::tuple<int, int, T> operator*() const;
    };

    Iterator begin() const;
    Iterator end() const;
};


#endif