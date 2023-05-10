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
            noexcept : current(const_cast<pointer>(other.current)),
                       first(const_cast<pointer>(other.first)),
                       last(const_cast<pointer>(other.first)),
                       node(const_cast<pointer>(other.node))
        {}

    public:
        reference
        operator*() const { return *current; }

        pointer
        operator->() const { return default_alloc.address(*current); }

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

    explicit deque(size_type count) { initialize_map(count); }

#pragma endregion


#pragma region storage
private:
    pointer
    alloc_node() {
        return default_alloc.allocate(buffer_size);
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

#pragma endregion
};

}

#endif //ANYA_STL_DEQUE_HPP
