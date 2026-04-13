#include <iostream>
#include <type_traits>
#include <vector>
#include <list>
#include <string>
#include <tuple>
#include <utility>
#include <cstdint>

// Определяем, является ли тип целочисленным (исключаем bool)
template<typename T>
using is_integer_for_ip = std::integral_constant<bool,
    std::is_integral<T>::value && !std::is_same<T, bool>::value
>;

// Определяем, является ли тип строкой std::string
template<typename T>
using is_string = std::is_same<std::decay_t<T>, std::string>;

// Определяем, является ли тип контейнером (имеет begin/end, не является строкой)
template<typename T, typename = void>
struct is_container : std::false_type {};
template<typename T>
struct is_container<T, std::void_t<
    decltype(std::declval<T>().begin()),
    decltype(std::declval<T>().end()),
    typename T::value_type
>> : std::true_type {};

// Определяем, является ли тип кортежем std::tuple
template<typename T>
struct is_tuple : std::false_type {};
template<typename... Ts>
struct is_tuple<std::tuple<Ts...>> : std::true_type {};

// Проверяем, что все типы в кортеже одинаковы
template<typename Tuple>
struct tuple_types_are_equal : std::false_type {};
template<typename T>
struct tuple_types_are_equal<std::tuple<T>> : std::true_type {};
template<typename T, typename... Ts>
struct tuple_types_are_equal<std::tuple<T, Ts...>> {
    static constexpr bool value = std::conjunction_v<std::is_same<T, Ts>...>;
};

// Перегрузки функции print_ip

// 1. Для целочисленных типов
template<typename T, typename std::enable_if<is_integer_for_ip<T>::value, int>::type = 0>
void print_ip(const T& value) {
    using U = std::make_unsigned_t<T>;
    U val = static_cast<U>(value);
    constexpr size_t bytes = sizeof(T);
    for (size_t i = bytes; i > 0; --i) {
        std::cout << static_cast<unsigned>((val >> (8 * (i - 1))) & 0xFF);
        if (i > 1) std::cout << '.';
    }
    std::cout << std::endl;
}

// 2. Для строк
template<typename T, typename std::enable_if<is_string<T>::value, int>::type = 0>
void print_ip(const T& str) {
    std::cout << str << std::endl;
}

// 3. Для контейнеров (исключая строки)
template<typename T, typename std::enable_if<is_container<T>::value && !is_string<T>::value, int>::type = 0>
void print_ip(const T& container) {
    bool first = true;
    for (const auto& elem : container) {
        if (!first) std::cout << '.';
        first = false;
        std::cout << elem;
    }
    std::cout << std::endl;
}

// 4. Для кортежей с одинаковыми типами
template<typename T, typename std::enable_if<is_tuple<T>::value && tuple_types_are_equal<T>::value, int>::type = 0>
void print_ip(const T& tup) {
    [&]<std::size_t... I>(std::index_sequence<I...>) {
        bool first = true;
        ((std::cout << (first ? "" : ".") << std::get<I>(tup), first = false), ...);
    }(std::make_index_sequence<std::tuple_size<T>::value>{});
    std::cout << std::endl;
}

int main() {
    print_ip(int8_t{-1});                         // 255
    print_ip(int16_t{0});                         // 0.0
    print_ip(int32_t{2130706433});                // 127.0.0.1
    print_ip(int64_t{8875824491850138409});       // 123.45.67.89.101.112.131.41

    print_ip(std::string{"Hello, World!"});       // Hello, World!

    print_ip(std::vector<int>{100, 200, 300, 400});   // 100.200.300.400
    print_ip(std::list<short>{400, 300, 200, 100});   // 400.300.200.100

    print_ip(std::make_tuple(123, 456, 789, 0));      // 123.456.789.0

    // print_ip(std::make_tuple(1, "two", 3.0)); // ошибка компиляции

    return 0;
}