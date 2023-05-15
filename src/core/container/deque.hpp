//
// Created by Anya on 2023/5/9.
//

#ifndef ANYA_STL_DEQUE_HPP
#define ANYA_STL_DEQUE_HPP

#include "allocator/memory.hpp"
#include "iterator/iterator.hpp"
#include "algorithm/algorithm.h"

namespace anya {

template<class T,
         class Allocator = anya::allocator<T>>
class deque {
private:
    static_assert(std::is_same<typename std::remove_cv<T>::type, T>::value,
                  "anya::deque must have a non-const, non-volatile value_type");
    static_assert(std::is_same<typename Allocator::value_type, T>::value,
                  "anya::deque must have the same value_type as its allocator");

private:
    using map_pointer = T**;
    using map_alloc_type = anya::allocator<T*>;

private:
    static constexpr size_t buffer_size = sizeof(T) < 512 ? size_t(512 / sizeof(T)) : size_t(1);
    static constexpr size_t default_map_size = 8;

    anya::allocator<T>  default_alloc{}; // 普通内存分配器
    map_alloc_type      map_alloc{};     // 中控节点分配器

private:
    template<class Tp>
    class deque_iterator
        : public anya::iterator<anya::random_access_iterator_tag, Tp> {
    private:
        friend class deque;

    public:
        using iterator_category = typename deque_iterator::iterator_category;
        using value_type        = typename deque_iterator::value_type ;
        using difference_type   = typename deque_iterator::difference_type ;
        using pointer           = typename deque_iterator::pointer ;
        using reference         = typename deque_iterator::reference ;

    private:
        pointer current{};      // 当前元素
        pointer first{};        // cur所在缓冲区的头指针
        pointer last{};         // cur所在缓冲区的尾指针
        map_pointer node{};     // cur所在缓冲区

    public:
        deque_iterator() = default;

        deque_iterator(const deque_iterator&) = default;

        // 这个约束是为了实现 iterator 能转化为 const_iterator，但反之不行
        template<typename U>
        requires std::same_as<U*, T*>
        deque_iterator(const deque_iterator<U>& other)
            noexcept : current(const_cast<T*>(other.current)),
                       first  (const_cast<T*>(other.first)),
                       last   (const_cast<T*>(other.first)),
                       node   (const_cast<T**>(other.node))
        {}

    public:
        reference
        operator*() const { return *current; }

        pointer
        operator->() const { return std::addressof(*current); }

        difference_type
        operator-(const deque_iterator& other) const {
            return difference_type(buffer_size) * (node - other.node - 1) + (current - first) + (other.last - other.current);
        }

        deque_iterator&
        operator++() {
            ++current;
            if (current == last) {
                set_node(node + 1);
                current = first;
            }
            return *this;
        }

        deque_iterator
        operator++(int) {
            deque_iterator temp = *this;
            return ++*this, temp;
        }

        deque_iterator&
        operator--() {
            if (current == first) {
                set_node(node - 1);
                current = last;
            }
            --current;
            return *this;
        }

        deque_iterator
        operator--(int) {
            deque_iterator temp = *this;
            return --*this, temp;
        }

        deque_iterator&
        operator+=(difference_type n) {
            difference_type offset = n + (current - first);
            if (offset >= 0 && offset < difference_type(buffer_size)) {
                current += n;
            }
            else {
                difference_type node_offset = 0;
                node_offset = offset > 0
                              ? offset / difference_type(buffer_size)
                              : -difference_type((-offset - 1) / buffer_size) - 1;
                // 切换到不同缓冲区
                set_node(node + node_offset);
                current = first + (offset - node_offset * difference_type(buffer_size));
            }
            return *this;
        }

        deque_iterator
        operator+(difference_type n) const {
            return deque_iterator(*this) += n;
        }

        deque_iterator&
        operator-=(difference_type n) {
            return (*this) += (-n);
        }

        deque_iterator
        operator-(difference_type n) const {
            return deque_iterator(*this) += -n;
        }

        reference
        operator[](size_t n) const {
            return *(*this + difference_type(n));
        }

        bool
        operator==(const deque_iterator &rhs) const {
            return current == rhs.current;
        }

        bool
        operator!=(const deque_iterator &rhs) const {
            return !(rhs == *this);
        }

        bool
        operator<(const deque_iterator &rhs) const {
            return (node == rhs.node) ? current < rhs.current : node < rhs.node;
        }

        bool
        operator>(const deque_iterator &rhs) const {
            return rhs < *this;
        }

        bool
        operator<=(const deque_iterator &rhs) const {
            return !(rhs < *this);
        }

        bool
        operator>=(const deque_iterator &rhs) const {
            return !(*this < rhs);
        }

    private:
        void set_node(map_pointer new_node) {
            node = new_node;
            first = *new_node;
            last = first + difference_type(buffer_size);
        }
    };

public:
    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using allocator_type  = anya::allocator<T>;

public:
    using iterator               = deque_iterator<value_type>;
    using const_iterator         = deque_iterator<const value_type>;
    using reverse_iterator       = anya::reverse_iterator<iterator>;
    using const_reverse_iterator = anya::reverse_iterator<const_iterator>;

private:
    map_pointer map_buffer{};  // 中控缓冲区列表
    size_type   map_size{};    // map_buffer的大小
    iterator start, finish;    // [start.node , finish.node]

#pragma region 构造 && 析构
public:
    deque() { initialize_map(0); }

    explicit deque(size_type count) { fill_initialize(count); }

    deque(size_type count, const T& value) {
        fill_initialize(count, value);
    }

    template<class InputIt>
    requires std::derived_from<typename InputIt::iterator_category, anya::input_iterator_tag>
    deque(InputIt first, InputIt last) {
        using iterator_tag = anya::iter_category_t<InputIt>;
        if constexpr (std::is_same_v<iterator_tag, anya::input_iterator_tag>) {
            // TODO: 待完成 insert()
            initialize_map(0);
            throw std::runtime_error("deque(InputIt first, InputIt last) 未完成");
        }
        else {
            size_t count = anya::distance(first, last);
            initialize_map(count);
            anya::uninitialized_copy_n(first, count, start);
        }
    }

    deque(const deque& other) {
        size_t count = other.size();
        initialize_map(count);
        anya::uninitialized_copy_n(other.begin(), count, start);
    }

    deque(deque&& other) {
        initialize_map(0);
        this->swap(other);
    }

    deque(std::initializer_list<T> init) {
        size_t count = anya::distance(init.begin(), init.end());
        initialize_map(count);
        anya::uninitialized_copy_n(init.begin(), count, start);
    }

    ~deque() {
        destroy_all_node();
        map_alloc.deallocate(map_buffer, map_size);
        map_buffer = nullptr, map_size = 0;
    }

#pragma endregion

#pragma region 访问
public:
    reference
    at(size_type pos) {
        if (pos >= size()) throw std::out_of_range("deque::at(size_type pos)");
        return start[pos];
    }

    const_reference
    at(size_type pos) const {
        if (pos >= size()) throw std::out_of_range("deque::at(size_type pos)");
        return start[pos];
    }

    reference
    operator[](size_type pos) { return start[pos]; }

    const_reference
    operator[]( size_type pos ) const { return start[pos]; }

    reference
    front() { return *start; }

    const_reference
    front() const { return *start; }

    reference
    back() { return *--iterator(finish); }

    const_reference
    back() const { return *--iterator(finish); }
#pragma endregion


#pragma region 迭代器
public:
    [[nodiscard]] iterator
    begin() noexcept { return start; }

    [[nodiscard]] const_iterator
    begin() const noexcept { return start; }

    [[nodiscard]] const_iterator
    cbegin() const noexcept { return start; }

    [[nodiscard]] iterator
    end() noexcept { return finish; }

    [[nodiscard]] const_iterator
    end() const noexcept { return finish; }

    [[nodiscard]] const_iterator
    cend() const noexcept { return finish; }

    [[nodiscard]] reverse_iterator
    rbegin() noexcept { return reverse_iterator(end()); }

    [[nodiscard]] const_reverse_iterator
    rbegin() const noexcept { return const_reverse_iterator(cend()); }

    [[nodiscard]] const_reverse_iterator
    crbegin() const noexcept { return const_reverse_iterator(cend()); }

    [[nodiscard]] reverse_iterator
    rend() noexcept { return reverse_iterator(begin()); }

    [[nodiscard]] const_reverse_iterator
    rend() const noexcept { return const_reverse_iterator(cbegin()); }

    [[nodiscard]] const_reverse_iterator
    crend() const noexcept { return const_reverse_iterator(cbegin()); }

#pragma endregion


#pragma region 容量
public:
    [[nodiscard]] bool
    empty() const noexcept { return finish == start; }

    [[nodiscard]] size_type
    size() const noexcept { return finish - start; }

    [[nodiscard]] size_type
    max_size() const noexcept { return default_alloc.max_size(); }

    // TODO: 未完成
    void
    shrink_to_fit() { throw std::runtime_error("deque::shrink_to_fit()未完成"); }
#pragma endregion


#pragma region 修改器
public:
    void
    swap(deque& other) noexcept {
        std::swap(map_buffer, other.map_buffer);
        std::swap(map_size, other.map_size);
        std::swap(start, other.start);
        std::swap(finish, other.finish);
    }

#pragma endregion

#pragma region 友元比较函数
public:
    bool friend
    operator==(const anya::deque<T, Allocator>& lhs,
               const anya::deque<T, Allocator>& rhs) {
        return lhs.size() == rhs.size() && anya::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

#pragma endregion

#pragma region storage
private:
    // 开辟结点
    pointer
    alloc_node() {
        return default_alloc.allocate(buffer_size);
    }

    // 回收结点
    void
    dealloc_node(pointer buffer) {
        default_alloc.deallocate(buffer, buffer_size);
    }

    // 析构并回收所有结点
    void
    destroy_all_node() {
        for (map_pointer node = start.node + 1; node < finish.node; ++node) {
            anya::destroy(*node, *node + buffer_size);
            dealloc_node(*node);
        }
        if (start.node != finish.node) {
            anya::destroy(start.current, start.last);
            anya::destroy(finish.first, finish.current);
            dealloc_node(finish.first);
        }
        else {
            anya::destroy(start.current, finish.current);
        }
        dealloc_node(start.first);
    }

    void
    initialize_map(size_type element_count) {
        size_t node_count = element_count / buffer_size + 1;
        this->map_size = anya::max(default_map_size, node_count + 2);
        this->map_buffer = map_alloc.allocate(map_size);
        // 当 map_size 为 default_map_size 时，让起始位置位于中间
        map_pointer m_start = map_buffer + (map_size - node_count) / 2;
        map_pointer m_finish = m_start + node_count - 1;
        for (map_pointer cur = m_start; cur <= m_finish; ++cur) {
            *cur = alloc_node();
        }
        // 设置迭代器
        start.set_node(m_start);
        start.current = start.first;
        finish.set_node(m_finish);
        finish.current = finish.first + element_count % buffer_size;
    }

    void
    fill_initialize(size_type count) {
        initialize_map(count);
        for (map_pointer cur = start.node; cur < finish.node; ++cur) {
            anya::uninitialized_default_construct_n(*cur, buffer_size);
        }
        anya::uninitialized_default_construct(finish.first, finish.current);
    }

    void
    fill_initialize(size_type count, const value_type &value) {
        initialize_map(count);
        for (map_pointer cur = start.node; cur < finish.node; ++cur) {
            anya::uninitialized_fill_n(*cur, buffer_size, value);
        }
        anya::uninitialized_fill(finish.first, finish.current, value);
    }
#pragma endregion

};

}

#endif //ANYA_STL_DEQUE_HPP
