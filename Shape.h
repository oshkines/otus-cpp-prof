#pragma once
#include <iostream>
#include <memory>

// Базовый класс для всех фигур (Интерфейс)
class Shape {
public:
    virtual ~Shape() = default; // Виртуальный деструктор обязателен для полиморфизма!
    virtual void draw() const = 0; // Чисто виртуальный метод (делает класс абстрактным)
};

// Конкретный пример фигуры: Линия
class Line : public Shape {
public:
    void draw() const override { std::cout << "  [Рисуем линию]\n"; }
};

// Конкретный пример фигуры: Круг
class Circle : public Shape {
public:
    void draw() const override { std::cout << "  [Рисуем круг]\n"; }
};