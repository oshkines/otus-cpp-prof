#include "matrix.hpp"
#include <algorithm>


template<typename T, T DefaultValue>
Matrix<T, DefaultValue>::CellProxy::CellProxy(Matrix& matrix, int x, int y)
    : matrix_(matrix), x_(x), y_(y) {}

template<typename T, T DefaultValue>
typename Matrix<T, DefaultValue>::CellProxy& 
Matrix<T, DefaultValue>::CellProxy::operator=(const T& value) {
    if (value == matrix_.default_value_) {
        // Присваивание значения по умолчанию – удаляем ячейку
        matrix_.cells_.erase({x_, y_});
    } else {
        // Сохраняем значение
        matrix_.cells_[{x_, y_}] = value;
    }
    return *this;
}

template<typename T, T DefaultValue>
Matrix<T, DefaultValue>::CellProxy::operator T() const {
    auto it = matrix_.cells_.find({x_, y_});
    if (it != matrix_.cells_.end()) {
        return it->second;
    }
    return matrix_.default_value_;
}

template<typename T, T DefaultValue>
typename Matrix<T, DefaultValue>::CellProxy& 
Matrix<T, DefaultValue>::CellProxy::operator=(const CellProxy& other) {
    *this = static_cast<T>(other);
    return *this;
}

template<typename T, T DefaultValue>
Matrix<T, DefaultValue>::RowProxy::RowProxy(Matrix& matrix, int x)
    : matrix_(matrix), x_(x) {}

template<typename T, T DefaultValue>
typename Matrix<T, DefaultValue>::CellProxy 
Matrix<T, DefaultValue>::RowProxy::operator[](int y) {
    return CellProxy(matrix_, x_, y);
}

template<typename T, T DefaultValue>
typename Matrix<T, DefaultValue>::RowProxy 
Matrix<T, DefaultValue>::operator[](int x) {
    return RowProxy(*this, x);
}

template<typename T, T DefaultValue>
T Matrix<T, DefaultValue>::at(int x, int y) const {
    auto it = cells_.find({x, y});
    if (it != cells_.end()) {
        return it->second;
    }
    return default_value_;
}

template<typename T, T DefaultValue>
size_t Matrix<T, DefaultValue>::size() const {
    return cells_.size();
}

template<typename T, T DefaultValue>
Matrix<T, DefaultValue>::Iterator::Iterator(MapIterator it) : it_(it) {}

template<typename T, T DefaultValue>
bool Matrix<T, DefaultValue>::Iterator::operator!=(const Iterator& other) const {
    return it_ != other.it_;
}

template<typename T, T DefaultValue>
typename Matrix<T, DefaultValue>::Iterator& 
Matrix<T, DefaultValue>::Iterator::operator++() {
    ++it_;
    return *this;
}

template<typename T, T DefaultValue>
std::tuple<int, int, T> Matrix<T, DefaultValue>::Iterator::operator*() const {
    const auto& [key, value] = *it_;
    return {key.first, key.second, value};
}

template<typename T, T DefaultValue>
typename Matrix<T, DefaultValue>::Iterator Matrix<T, DefaultValue>::begin() const {
    return Iterator(cells_.begin());
}

template<typename T, T DefaultValue>
typename Matrix<T, DefaultValue>::Iterator Matrix<T, DefaultValue>::end() const {
    return Iterator(cells_.end());
}

template class Matrix<int, 0>;