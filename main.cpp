#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include "lib.h" 

// ============================================================
// Аллокатор с фиксированным количеством элементов (статический пул)
// ============================================================
template<typename T, size_t N>
class FixedAllocator {
    struct Pool {
        T* storage;
        std::vector<size_t> free_list;

        Pool() {
            storage = static_cast<T*>(::operator new(sizeof(T) * N));
            for (size_t i = N; i > 0; --i)
                free_list.push_back(i - 1);
        }
        ~Pool() {
            ::operator delete(storage);
        }
        Pool(const Pool&) = delete;
        Pool& operator=(const Pool&) = delete;
    };

    static Pool& get_pool() {
        static Pool pool;
        return pool;
    }

public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    FixedAllocator() = default;

    template<typename U>
    FixedAllocator(const FixedAllocator<U, N>&) {}

    pointer allocate(size_type n) {
        if (n != 1) throw std::bad_alloc();
        Pool& pool = get_pool();
        if (pool.free_list.empty()) throw std::bad_alloc();
        size_t idx = pool.free_list.back();
        pool.free_list.pop_back();
        return pool.storage + idx;
    }

    void deallocate(pointer p, size_type n) noexcept {
        if (p == nullptr || n == 0) return;
        Pool& pool = get_pool();
        size_t idx = p - pool.storage;
        pool.free_list.push_back(idx);
    }

    template<typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        ::new(static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U* p) {
        p->~U();
    }

    template<typename U>
    struct rebind {
        using other = FixedAllocator<U, N>;
    };

    bool operator==(const FixedAllocator&) const { return true; }
    bool operator!=(const FixedAllocator&) const { return false; }
};

// ============================================================
// Собственный контейнер (односвязный список) с аллокатором
// ============================================================
template<typename T, typename Allocator = std::allocator<T>>
class MyContainer {
public:
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = size_t;

private:
    struct Node {
        T value;
        Node* next;
    };

    using NodeAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    NodeAllocator node_alloc_;
    Node* head_;
    Node* tail_;
    size_type size_;

public:
    MyContainer() : node_alloc_(), head_(nullptr), tail_(nullptr), size_(0) {}
    explicit MyContainer(const Allocator& alloc) : node_alloc_(alloc), head_(nullptr), tail_(nullptr), size_(0) {}

    ~MyContainer() {
        clear();
    }

    void push_back(const T& value) {
        Node* new_node = node_alloc_.allocate(1);
        std::allocator_traits<NodeAllocator>::construct(node_alloc_, new_node, Node{value, nullptr});
        if (tail_) {
            tail_->next = new_node;
            tail_ = new_node;
        } else {
            head_ = tail_ = new_node;
        }
        ++size_;
    }

    size_type size() const { return size_; }
    bool empty() const { return size_ == 0; }

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator(Node* node) : node_(node) {}
        reference operator*() const { return node_->value; }
        pointer operator->() const { return &node_->value; }
        iterator& operator++() { node_ = node_->next; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
        bool operator==(const iterator& other) const { return node_ == other.node_; }
        bool operator!=(const iterator& other) const { return !(*this == other); }
    private:
        Node* node_;
    };

    iterator begin() { return iterator(head_); }
    iterator end() { return iterator(nullptr); }

private:
    void clear() {
        Node* cur = head_;
        while (cur) {
            Node* next = cur->next;
            std::allocator_traits<NodeAllocator>::destroy(node_alloc_, cur);
            node_alloc_.deallocate(cur, 1);
            cur = next;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }
};

int factorial(int n) {
    int result = 1;
    for (int i = 1; i <= n; ++i) result *= i;
    return result;
}

int main() {
    // 1. std::map с обычным аллокатором
    std::map<int, int> map1;
    for (int i = 0; i < 10; ++i) map1[i] = factorial(i);

    // 2. std::map с нашим аллокатором, ограниченным 11 элементами (учитываем служебный узел)
    FixedAllocator<std::pair<const int, int>, 11> alloc;
    std::map<int, int, std::less<int>, FixedAllocator<std::pair<const int, int>, 11>> map2(alloc);
    for (int i = 0; i < 10; ++i) map2[i] = factorial(i);

    std::cout << "std::map with default allocator:\n";
    for (const auto& p : map1) std::cout << p.first << " " << p.second << "\n";

    std::cout << "\nstd::map with FixedAllocator<11>:\n";
    for (const auto& p : map2) std::cout << p.first << " " << p.second << "\n";

    // 3. Свой контейнер с обычным аллокатором
    MyContainer<int> cont1;
    for (int i = 0; i < 10; ++i) cont1.push_back(i);

    // 4. Свой контейнер с нашим аллокатором для int (10 элементов)
    FixedAllocator<int, 10> int_alloc;
    MyContainer<int, FixedAllocator<int, 10>> cont2(int_alloc);
    for (int i = 0; i < 10; ++i) cont2.push_back(i);

    std::cout << "\nMyContainer with default allocator:\n";
    for (auto val : cont1) std::cout << val << " ";
    std::cout << "\n";

    std::cout << "\nMyContainer with FixedAllocator<int,10>:\n";
    for (auto val : cont2) std::cout << val << " ";
    std::cout << "\n";

    return 0;
}