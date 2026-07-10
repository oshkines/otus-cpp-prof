markdown
# sqlite3server – асинхронный SQL-сервер для операций над множествами

## Описание

Сервер обрабатывает SQL-подобные команды для двух таблиц `A` и `B` с фиксированной структурой:

```sql
{
    int id;
    std::string name;
}
Поддерживаются операции:

INSERT – добавление записи в таблицу.

TRUNCATE – очистка таблицы.

INTERSECTION – пересечение двух таблиц (INNER JOIN).

SYMMETRIC_DIFFERENCE – симметрическая разность (элементы, присутствующие только в одной из таблиц).

Сервер написан на C++17 с использованием Boost.Asio (асинхронный ввод-вывод) и SQLite3 (встроенная БД в памяти).

Установка
Из DEB-пакета (рекомендуется)
bash
sudo dpkg -i join-server_5.2.0_amd64.deb
Сборка из исходников
bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
Запуск
bash
./join-server 9000
Сервер запустится на порту 9000 и будет принимать команды от клиентов.

Протокол
Команды отправляются по TCP, каждая команда завершается символом \n.

Команды
Команда	Формат	Пример	Ответ
INSERT	INSERT table id name	INSERT A 0 lean	OK или ERR duplicate <id>
TRUNCATE	TRUNCATE table	TRUNCATE A	OK
INTERSECTION	INTERSECTION	INTERSECTION	Строки вида id,nameA,nameB и OK в конце
SYMMETRIC_DIFFERENCE	SYMMETRIC_DIFFERENCE	SYMMETRIC_DIFFERENCE	Строки вида id,nameA,nameB и OK в конце
Пример сессии
bash
$ echo "INSERT A 0 lean" | nc localhost 9000
OK

$ echo "INSERT A 1 sweater" | nc localhost 9000
OK

$ echo "INTERSECTION" | nc localhost 9000
0,lean,
1,sweater,
OK

$ echo "SYMMETRIC_DIFFERENCE" | nc localhost 9000
0,lean,
1,sweater,
2,frank,
6,,flour
7,,wonder
8,,selection
OK
Тестовые данные
Таблица A

id	name
0	lean
1	sweater
2	frank
3	violation
4	quality
5	precision
Таблица B

id	name
3	proposal
4	example
5	lake
6	flour
7	wonder
8	selection
Результат INTERSECTION
id	A	B
3	violation	proposal
4	quality	example
5	precision	lake
Результат SYMMETRIC_DIFFERENCE
id	A	B
0	lean	
1	sweater	
2	frank	
6		flour
7		wonder
8		selec
Сборка DEB-пакета через Docker
bash
docker build -t join-builder .
docker run --rm -v ${PWD}:/host join-builder cp /output/*.deb /host/
Полученный файл join-server_5.2.0_amd64.deb можно установить.

Генерация документации
bash
doxygen Doxyfile
Документация появится в папке docs/html/.

Структура проекта
text
.
├── CMakeLists.txt
├── Dockerfile
├── Doxyfile
├── README.md
├── src/
│   ├── main.cpp
│   ├── DatabaseManager.hpp
│   ├── DatabaseManager.cpp
│   ├── CommandParser.hpp
│   └── CommandParser.cpp
├── debian/
│   ├── templates/
│   │   ├── control
│   │   ├── changelog
│   │   └── rules
│   └── source/
│       └── format
└── docs/
    └── html/
        └── index.html
Автор
oshkines oshkines@mail.ru

Лицензия
Учебный проект.

