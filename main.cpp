#include <iostream>
#include <map>
#include <tuple>
#include <cassert>
#ifdef _WIN32
    #include <windows.h>
#endif

template<typename T, T DefaultValue = T{}>
class Matrix {
private:
    std::map<std::pair<int, int>, T> cells_;
    T default_value_ = DefaultValue;

public:
    class RowProxy {
        Matrix& matrix_;
        int x_;
    public:
        RowProxy(Matrix& matrix, int x) : matrix_(matrix), x_(x) {}

        class CellProxy {
            Matrix& matrix_;
            int x_;
            int y_;
        public:
            CellProxy(Matrix& matrix, int x, int y) : matrix_(matrix), x_(x), y_(y) {}

            CellProxy& operator=(const T& value) {
                if (value == matrix_.default_value_) {
                    matrix_.cells_.erase({x_, y_});
                } else {
                    matrix_.cells_[{x_, y_}] = value;
                }
                return *this;
            }

            operator T() const {
                auto it = matrix_.cells_.find({x_, y_});
                if (it != matrix_.cells_.end()) {
                    return it->second;
                }
                return matrix_.default_value_;
            }

            CellProxy& operator=(const CellProxy& other) {
                *this = static_cast<T>(other);
                return *this;
            }
        };

        CellProxy operator[](int y) {
            return CellProxy(matrix_, x_, y);
        }
    };

    RowProxy operator[](int x) {
        return RowProxy(*this, x);
    }

    T at(int x, int y) const {
        auto it = cells_.find({x, y});
        return (it != cells_.end()) ? it->second : default_value_;
    }

    size_t size() const { return cells_.size(); }

    class Iterator {
        using MapIterator = typename std::map<std::pair<int, int>, T>::const_iterator;
        MapIterator it_;
    public:
        Iterator(MapIterator it) : it_(it) {}
        bool operator!=(const Iterator& other) const { return it_ != other.it_; }
        Iterator& operator++() { ++it_; return *this; }
        std::tuple<int, int, T> operator*() const {
            const auto& [key, value] = *it_;
            return {key.first, key.second, value};
        }
    };

    Iterator begin() const { return Iterator(cells_.begin()); }
    Iterator end() const { return Iterator(cells_.end()); }
};

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    Matrix<int, 0> matrix;

    // Заполнение главной диагонали
    for (int i = 0; i < 10; ++i) {
        matrix[i][i] = i;
    }

    // Заполнение второстепенной диагонали
    for (int i = 0; i < 10; ++i) {
        int j = 9 - i;
        matrix[i][j] = j;  // значение = j (9-i)
    }

    std::cout << "Фрагмент матрицы [1,1]..[8,8]:\n";
    for (int i = 1; i <= 8; ++i) {
        for (int j = 1; j <= 8; ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\nКоличество занятых ячеек: " << matrix.size() << "\n";

    std::cout << "\nЗанятые ячейки (x y value):\n";
    for (const auto& cell : matrix) {
        int x, y, value;
        std::tie(x, y, value) = cell;
        std::cout << x << " " << y << " " << value << "\n";
    }

    Matrix<int, 0> m;
    ((m[10][20] = 5) = 6) = 7;
    std::cout << "\nm[10][20] = " << m[10][20] << " (должно быть 7)\n";
    assert(m[10][20] == 7);
    assert(m.size() == 1);

    return 0;
}