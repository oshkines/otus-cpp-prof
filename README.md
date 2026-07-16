# MapReduce - Airbnb Price Statistics

Проект для вычисления статистических характеристик (среднее, дисперсия) цен на недвижимость в Нью-Йорке в парадигме MapReduce.

## Структура проекта
.
├── CMakeLists.txt # Конфигурация сборки
├── Dockerfile # Docker образ
├── Doxyfile # Конфигурация Doxygen
├── README.md # Этот файл
├── .gitignore # Игнорируемые файлы
├── debian/ # Debian пакет
│ └── (файлы для debian-пакета)
├── docs/ # Документация
├── src/ # Исходный код
│ ├── main.cpp # Главная программа
│ ├── mapper.cpp # Mapper
│ ├── reducer.cpp # Reducer
│ ├── csv_parser.hpp # Парсер CSV
│ ├── statistics.hpp # Статистические функции
│ └── statistics.cpp # Реализация статистики
├── input/ # Входные данные
│ └── AB_NYC_2019.csv # Датасет Airbnb
├── bin/ # Собранные бинарники
├── scripts/ # Скрипты
│ ├── run_locally.sh # Локальный запуск
│ └── run_hadoop.sh # Запуск в Hadoop
└── tests/ # Тесты (опционально)


## Сборка

### Локальная сборка

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)


## Сборка

### Локальная сборка

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)

docker build -t mapreduce .
docker run --rm -v $(pwd)/input:/app/input -v $(pwd)/output:/app/output mapreduce

Использование
1. Основные режимы

# Запуск mapper (читает из stdin)
cat input.csv | ./bin/mapreduce mapper

# Запуск reducer (читает из stdin)
./bin/mapreduce reducer

# Полный пайплайн локально
./bin/mapreduce local input/AB_NYC_2019.csv

# Прямой подсчёт статистики (для верификации)
./bin/mapreduce stats input/AB_NYC_2019.csv

2. MapReduce пайплайн

# Полный пайплайн вручную
cat input/AB_NYC_2019.csv | \
    ./bin/mapreduce mapper | \
    sort -k1 | \
    ./bin/mapreduce reducer

# Использование скрипта
./scripts/run_locally.sh

3. Отдельные бинарники

# Собрать отдельные бинарники
make mapper reducer

# Использовать
cat input.csv | ./bin/mapper | sort | ./bin/reducer

Результаты
Программа выводит:

Mean - средняя цена

Variance - дисперсия цены

Std Dev - среднеквадратическое отклонение

Count - количество записей

Пример вывода:

=== Statistics for: price ===
Mean:      152.72
Variance:  28765.81
Std Dev:   169.60
Count:     48895

Тестирование
Проверка корректности
Программа имеет режим stats, который вычисляет статистику напрямую (без MapReduce) для верификации результатов.

./bin/mapreduce stats input/AB_NYC_2019.csv

Dependencies
CMake 3.10+

C++17 compiler (GCC 7+, Clang 5+)

(Опционально) Docker для контейнеризации

Автор
Студент курса "C++ Профессионал" от OTUS

Лицензия
MIT License

## Использование

### Сборка и запуск

```bash
# Сборка
mkdir build && cd build
cmake ..
make

# Запуск полного пайплайна
cd ..
./bin/mapreduce local input/AB_NYC_2019.csv

# Или используя скрипт
./scripts/run_locally.sh

Проверка результатов
# Прямой подсчёт статистики для верификации
./bin/mapreduce stats input/AB_NYC_2019.csv

# Сравнение с результатами MapReduce
cat output/result.txt

