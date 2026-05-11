#include "matrix.hpp"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    // Настройка кодировки для Windows
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif

    // Создаём матрицу с типом int и значением по умолчанию 0
    Matrix<int, 0> matrix;

    // Заполнение главной диагонали [0,0]..[9,9] значениями от 0 до 9
    for (int i = 0; i < 10; ++i) {
        matrix[i][i] = i;
    }

    // Заполнение второстепенной диагонали [0,9]..[9,0] значениями от 9 до 0
    for (int i = 0; i < 10; ++i) {
        matrix[i][9 - i] = 9 - i;
    }

    // Вывод фрагмента от [1,1] до [8,8]
    std::cout << "Фрагмент матрицы [1,1]..[8,8]:\n";
    for (int i = 1; i <= 8; ++i) {
        for (int j = 1; j <= 8; ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }

    // Количество занятых ячеек
    std::cout << "\nКоличество занятых ячеек: " << matrix.size() << "\n";

    // Вывод всех занятых ячеек
    std::cout << "\nЗанятые ячейки (x y value):\n";
    for (const auto& cell : matrix) {
        int x, y, value;
        std::tie(x, y, value) = cell;
        std::cout << x << " " << y << " " << value << "\n";
    }

    // Проверка каскадного присваивания
    Matrix<int, 0> m;
    ((m[10][20] = 5) = 6) = 7;
    std::cout << "\nm[10][20] = " << m[10][20] << " (должно быть 7)\n";

    return 0;
}