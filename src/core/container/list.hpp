//
// Created by Anya on 2023/4/15.
//

#ifndef ANYA_STL_LIST_HPP
#define ANYA_STL_LIST_HPP

#include "allocator/memory.hpp"
#include "iterator/iterator.hpp"
#include "algorithm/algorithm.h"

namespace anya {

template<class T,
         class Allocator = anya::allocator<T>>
class list {
private:
    static_assert(std::is_same<typename std::remove_cv<T>::type, T>::value,
                  "anya::list must have a non-const, non-volatile value_type");
    static_assert(std::is_same<typename Allocator::value_type, T>::value,
                  "anya::list must have the same value_type as its allocator");

#pragma region 内部辅助类
private:
    struct list_base_node {
        list_base_node *next;
        list_base_node *prev;
    };

    template<class Tp>
    struct list_node : list_base_node {
        Tp data;
    };

    template<class Tp>
    class list_iterator
        : public anya::iterator<anya::bidirectional_iterator_tag, T> {
    private:
        list_base_node* current;
    public:
        using Self     = list_iterator<Tp>;
        using Node     = list_node<Tp>;
        using Iterator = list_base_node*;
    public:
        list_iterator() = default;

        list_iterator(const Self&) = default;

        explicit list_iterator(list_base_node* it) noexcept : current(it) {}

        // 这个约束是为了实现 iterator 能转化为 const_iterator，但反之不行
        template<typename U>
        requires std::same_as<U*, T*>
        list_iterator(const list_iterator<U> other)
            noexcept : current(const_cast<Iterator>(other.base())) {}

    public:
        T&
        operator*() const noexcept {
            return reinterpret_cast<Node*>(current)->data;
        }

        T*
        operator->() const noexcept {
            return alloc.address(reinterpret_cast<Node*>(current)->data);
        }

        // forward
        Self&
        operator++() noexcept {
            return current = current->next, *this;
        }

        Self
        operator++(int) noexcept {
            Self tmp = *this;
            return current = current->next, tmp;
        }

        // backward
        Self&
        operator--() noexcept {
            return current = current->prev, *this;
        }

        Self
        operator--(int) noexcept {
            Self tmp = *this;
            return current = current->prev, tmp;
        }

        constexpr const Iterator&
        base() const noexcept { return current; }

        friend bool
        operator==(const Self& lhs, const Self& rhs) {
            return lhs.current == rhs.current;
        }

        friend bool
        operator!=(const Self& lhs, const Self& rhs) {
            return lhs.current != rhs.current;
        }
    };

    // 虚拟根节点, root->next == list.head
    struct list_root : list_base_node {
        size_t size;
        list_base_node* tail;
    };

#pragma endregion

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
    using iterator               = list_iterator<value_type>;
    using const_iterator         = list_iterator<const value_type>;
    using reverse_iterator       = anya::reverse_iterator<iterator>;
    using const_reverse_iterator = anya::reverse_iterator<const_iterator>;

private:
    using base_alloc_type = anya::allocator<list_base_node>;
    using node_alloc_type = anya::allocator<list_node<T>>;
    list_root root{};            // 虚拟根节点资源
    allocator_type alloc{};      // 普通内存分配器
    base_alloc_type base_alloc;  // base节点分配器
    node_alloc_type node_alloc;  // 标准节点分配器

#pragma region 构造 && 析构
public:
    list() { init_end(); }

    list(size_type count, const T& value) {
        init_end();
    }
#pragma endregion


#pragma region 迭代器
public:
    iterator
    begin() noexcept { return iterator(root.next); }

    const_iterator
    begin() const noexcept { return const_iterator(root.next); }

    const_iterator
    cbegin() const noexcept { return const_iterator(root.next); }

    iterator
    end() noexcept { return iterator(root.tail); }

    const_iterator
    end() const noexcept { return const_iterator(root.tail); }

    const_iterator
    cend() const noexcept { return const_iterator(root.tail); }

    reverse_iterator
    rbegin() noexcept { return reverse_iterator(end()); }

    const_reverse_iterator
    rbegin() const noexcept { return const_reverse_iterator(cend()); }

    const_reverse_iterator
    crbegin() const noexcept { return const_reverse_iterator(cend()); }

    reverse_iterator
    rend() noexcept { return reverse_iterator(begin()); }

    const_reverse_iterator
    rend() const noexcept { return const_reverse_iterator(cbegin()); }

    const_reverse_iterator
    crend() const noexcept { return const_reverse_iterator(cbegin()); }

#pragma endregion

#pragma region 辅助函数
private:
    void init_end() {
        auto* node = base_alloc.allocate(1);
        root.next = root.tail = node;
        node->next = nullptr;
        node->prev = &root;
    };

#pragma endregion

};

}

#endif //ANYA_STL_LIST_HPP
