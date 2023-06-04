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
        : public anya::iterator<anya::bidirectional_iterator_tag, Tp> {
    private:
        friend class list;
        list_base_node* current;
    public:
        using Self     = list_iterator<Tp>;
        using Node     = list_node<T>;
        using Iterator = list_base_node*;
    public:
        list_iterator() = default;

        list_iterator(const Self&) = default;

        explicit list_iterator(list_base_node* it) noexcept : current(it) {}

        // 这个约束是为了实现 iterator 能转化为 const_iterator，但反之不行
        template<typename U>
        requires std::same_as<U*, T*>
        list_iterator(const list_iterator<U>& other)
            noexcept : current(const_cast<Iterator>(other.base())) {}

    public:
        Tp&
        operator*() const noexcept {
            return reinterpret_cast<Node*>(current)->data;
        }

        Tp*
        operator->() const noexcept {
            return std::addressof(reinterpret_cast<Node*>(current)->data);
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

    allocator_type alloc{};      // 普通内存分配器
    base_alloc_type base_alloc;  // base节点分配器
    node_alloc_type node_alloc;  // 标准节点分配器
    list_root root{};            // 虚拟根节点资源

#pragma region 构造 && 析构
public:
    list() { init_end(); }

    list(size_type count, const T& value) {
        init_end();
        auto it = cend();
        while (count--) emplace(it, value);
    }

    explicit list(size_type count) {
        init_end();
        auto it = cend();
        while (count--) emplace(it);
    }

    template<class InputIt>
    requires std::derived_from<typename InputIt::iterator_category, anya::input_iterator_tag>
    list(InputIt first, InputIt last) {
        init_end();
        auto it = cend();
        insert(it, first, last);
    }

    list(const list& other) {
        init_end();
        auto it = cend();
        insert(it, other.begin(), other.end());
    }

    list(list&& other) {
        move_storage(other);
    }

    list(std::initializer_list<T> init) {
        init_end();
        auto it = cend();
        insert(it, init);
    }

    ~list() {
        destroy_all();
        base_alloc.deallocate(root.tail, 1);
    }
#pragma endregion


#pragma region 赋值
public:
    list&
    operator=(const list& other) {
        if (&other == this) return *this;
        assign_copy(other.begin(), other.end());
        return *this;
    }

    list&
    operator=(list&& other) noexcept {
        if (&other == this) return *this;
        move_storage(other);
        return *this;
    }

    list&
    operator=(std::initializer_list<T> ilist) {
        assign_copy(ilist.begin(), ilist.end());
        return *this;
    }

    void
    assign(size_type count, const T& value) {
        assign_fill(count, value);
    }

    template<class InputIt>
    requires std::derived_from<typename InputIt::iterator_category, anya::input_iterator_tag>
    void
    assign(InputIt first, InputIt last) {
        assign_copy(first, last);
    }

    void
    assign(std::initializer_list<T> ilist) {
        assign_copy(ilist.begin(), ilist.end());
    }

    allocator_type
    get_allocator() const noexcept { return alloc; }
#pragma endregion


#pragma region 访问
public:
    reference
    front() { return *begin(); }

    const_reference
    front() const { return *cbegin(); }

    reference
    back() { return *rbegin(); }

    const_reference
    back() const { return *crbegin(); }

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


#pragma region 容量
public:
    [[nodiscard]] bool
    empty() const noexcept { return root.size == 0; }

    [[nodiscard]] size_type
    size() const noexcept { return root.size; };

    [[nodiscard]] size_type
    max_size() const noexcept { return alloc.max_size(); }

#pragma endregion


#pragma region 修改器
public:
    void
    clear() noexcept { destroy_all(); };

    /*!
     * @param pos    将内容插入到它前面的迭代器。pos 可以是 end() 迭代器
     * @param value  要插入的元素值
     * @return       指向被插入 value 的迭代器
     */
    iterator
    insert(const_iterator pos, const T& value) {
        return insert_front(pos, make_node(value));
    };

    /*!
     * @param pos    将内容插入到它前面的迭代器。pos 可以是 end() 迭代器
     * @param value  要插入的元素值
     * @return       指向被插入 value 的迭代器
     */
    iterator
    insert(const_iterator pos, T&& value) {
        return insert_front(pos, make_node(std::move(value)));
    }

    /*!
     * @param pos    将内容插入到它前面的迭代器。pos 可以是 end() 迭代器
     * @param count  要插入的元素个数
     * @param value  要插入的元素值
     * @return       指向首个被插入元素的迭代器，或者在 count == 0 时返回 pos
     */
    iterator
    insert(const_iterator pos, size_type count, const T& value) {
        if (count == 0) return iterator(pos.current);
        --count;
        --pos;
        iterator ret = insert_back(pos, make_node(value)), cur = ret;
        while (count--) cur = insert_back(cur, make_node(value));
        return ret;
    }

    /*!
     * @tparam InputIt
     * @param pos    将内容插入到它前面的迭代器。pos 可以是 end() 迭代器
     * @param first  要插入的元素范围，
     * @param last   不能是指向调用 insert 所用的容器中的迭代器
     * @return       指向首个被插入元素的迭代器，或者在 first == last 时返回 pos
     */
    template<class InputIt>
    requires std::derived_from<typename InputIt::iterator_category, anya::input_iterator_tag>
    iterator
    insert(const_iterator pos, InputIt first, InputIt last) {
        if (first == last) return iterator(pos.current);
        --pos;
        iterator ret = insert_back(pos, make_node(*first++)), cur = ret;
        while (first != last) cur = insert_back(cur, make_node(*first++));
        return ret;
    }

    /*!
     * @param pos    将内容插入到它前面的迭代器。pos 可以是 end() 迭代器
     * @param ilist  要插入的值来源的 initializer_list
     * @return       指向首个被插入元素的迭代器，或者在 ilist 为空时返回 pos
     */
    iterator
    insert(const_iterator pos, std::initializer_list<T> ilist) {
        auto first = ilist.begin(), last = ilist.end();
        if (first == last) return iterator(pos.current);
        --pos;
        iterator ret = insert_back(pos, make_node(*first++)), cur = ret;
        while (first != last) cur = insert_back(cur, make_node(*first++));
        return ret;
    }

    template<class... Args>
    iterator
    emplace(const_iterator pos, Args&&... args) {
        return insert_front(pos, make_node(std::forward<Args>(args)...));
    };

    /*!
     * @param pos  指向要移除的元素的迭代器
     * @return     最后移除元素之后的迭代器
     */
    iterator
    erase(const_iterator pos) {
        list_base_node* cur = pos.current;
        list_base_node* pre = cur->prev;
        list_base_node* next = cur->next;
        pre->next = next, next->prev = pre;
        destroy_node(cur);
        return iterator(next);
    }

    /*!
     * @param first  要移除的元素范围
     * @param last   要移除的元素范围
     * @return       最后移除元素之后的迭代器
     */
    iterator
    erase(const_iterator first, const_iterator last) {
        if (first == last) return iterator(last.current);
        list_base_node* pre = first.current->prev;
        list_base_node* end = last.current;
        for (list_base_node* cur = pre->next, *temp; cur != end; ) {
            temp = cur->next;
            destroy_node(cur);
            cur = temp;
        }
        pre->next = end, end->prev = pre;
        return iterator(end);
    }

    template<class... Args>
    reference
    emplace_back(Args&&... args) {
        return *emplace(cend(), std::forward<Args>(args)...);
    }

    template<class... Args>
    reference
    emplace_front(Args&&... args) {
        return *emplace(cbegin(), std::forward<Args>(args)...);
    }

    void
    push_back(const T& value) {
        emplace_back(value);
    }

    void
    push_back(T&& value) {
        emplace_back(std::move(value));
    }

    void
    push_front(const T& value) {
        emplace_front(value);
    }

    void
    push_front(T&& value) {
        emplace_front(std::move(value));
    }

    void
    pop_back() {
        erase(--cend());
    }

    void
    pop_front() {
        erase(cbegin());
    }

    void
    resize(size_type count) {
        resize(count, value_type());
    }

    void
    resize(size_type count, const value_type& value) {
        size_t size = this->size();
        if (count > size) {
            insert(cend(), count - size, value);
        }
        else {
            erase(at(count), cend());
        }
    }

    void
    swap(list& other) noexcept {
        std::swap(root, other.root);
    }

#pragma endregion


#pragma region 操作
public:
    void
    merge(list& other) {
        merge(std::move(other));
    }

    void
    merge(list&& other) {
        merge(std::move(other), std::less<>());
    }

    template <class Compare>
    void
    merge(list& other, Compare comp) {
        merge(std::move(other), comp);
    }

    template <class Compare>
    void
    merge(list&& other, Compare comp) {
        if (this->begin() == other.begin())
            return;
        iterator insert_it = this->begin(), insert_end = this->end();
        iterator input_it = other.begin(), input_end = other.end();
        while (input_it != input_end) {
            // 保证*this的稳定性，<= 的元素都保持不变
            if (insert_it != insert_end && comp(*insert_it, *input_it)) {
                ++insert_it;
            }
            else if (insert_it != insert_end && *insert_it == *input_it) {
                ++insert_it;
            }
            else {
                insert_it = insert_front(insert_it, (input_it++).current);
            }
        }
        other.root.next = other.root.tail;
        this->root.size += other.root.size;
        other.root.size = 0;
    }

    void
    splice(const_iterator pos, list& other) {
        splice(pos, std::move(other), other.cbegin(), other.cend());
    }

    void
    splice(const_iterator pos, list&& other) {
        splice(pos, std::move(other), other.cbegin(), other.cend());
    }

    void
    splice(const_iterator pos, list& other, const_iterator it) {
        splice(pos, std::move(other), it, other.cend());
    }

    void
    splice(const_iterator pos, list&& other, const_iterator it) {
        splice(pos, std::move(other), it, other.cend());
    }

    void
    splice(const_iterator pos, list& other,
           const_iterator first, const_iterator last) {
        splice(pos, std::move(other), first, last);
    }

    void
    splice(const_iterator pos, list&& other,
           const_iterator first, const_iterator last) {
        if (first == last) return;
        size_t len = anya::distance(first, last);
        other.root.size -= len;
        list_base_node* pre = first.current->prev;
        list_base_node* next = last.current;
        // 缝合other链表
        connect(pre, next);
        insert(pos, first, last);
    }

    size_type
    remove(const T& value) {
        size_type ret = 0;
        for (auto it = begin(); it != end(); ) {
            if (*it == value) it = erase(it), ++ret;
            else ++it;
        }
        return ret;
    }

    template<class UnaryPredicate>
    size_type
    remove_if(UnaryPredicate p) {
        size_type ret = 0;
        for (auto it = begin(); it != end(); ) {
            if (p(*it)) it = erase(it), ++ret;
            else ++it;
        }
        return ret;
    }

    void
    reverse() noexcept {
        if (size() <= 1) return;
        list_base_node* cur = root.next, *next, *temp;
        root.next = root.tail;
        while (cur != root.tail) {
            temp = cur->next, next = root.next;
            connect(&root, cur);
            connect(cur, next);
            cur = temp;
        }
    }

    size_type
    unique() {
        if (size() <= 1) return 0;
        size_t ret = 0;
        for (auto it = begin(), end = this->end(), next = anya::next(it); next != end; ) {
            if (*it == *next) {
                next = erase(next), ++ret;
            }
            else {
                it = next;
                next = anya::next(it);
            }
        }
        return ret;
    }

    template<class BinaryPredicate>
    size_type
    unique(BinaryPredicate p) {
        if (size() <= 1) return 0;
        size_t ret = 0;
        for (auto it = begin(), end = this->end(), next = anya::next(it); next != end; ) {
            if (p(*it, *next)) {
                next = erase(next), ++ret;
            }
            else {
                it = next;
                next = anya::next(it);
            }
        }
        return ret;
    }

    void
    sort() {
        sort(std::less<>());
    }

    // 归并排序
    template<class Compare>
    void
    sort(Compare comp) {
        if (size() <= 1) return;
        list half;
        half.splice(half.cend(), *this, at(size() / 2), end());
        this->sort(comp);
        half.sort(comp);
        this->merge(half, comp);
    }

#pragma endregion

#pragma region 友元比较函数
public:
    friend bool
    operator==(const anya::list<T, Allocator>& lhs,
               const anya::list<T, Allocator>& rhs) {
        if (lhs.size() != rhs.size())
            return false;
        if (&lhs == &rhs || lhs.begin() == rhs.begin())
            return true;
        for (auto lit = lhs.begin(), rit = rhs.begin();
             lit != lhs.end(); ++lit, ++rit) {
            if (*lit != *rit)
                return false;
        }
        return true;
    }

    friend bool
    operator!=(const anya::list<T, Allocator>& lhs,
               const anya::list<T, Allocator>& rhs) {
        return !(lhs == rhs);
    }

    friend bool
    operator<(const anya::list<T, Allocator>& lhs,
               const anya::list<T, Allocator>& rhs) {
        // DONE: 将来替换成 anya::lexicographical_compare()
        return anya::lexicographical_compare(
            lhs.begin(), lhs.end(),
            rhs.begin(), rhs.end());
    }

    friend bool
    operator>(const anya::list<T, Allocator>& lhs,
              const anya::list<T, Allocator>& rhs) {
        return rhs < lhs;
    }

    friend bool
    operator<=(const anya::list<T, Allocator>& lhs,
              const anya::list<T, Allocator>& rhs) {
        return !(rhs < lhs);
    }

    friend bool
    operator>=(const anya::list<T, Allocator>& lhs,
              const anya::list<T, Allocator>& rhs) {
        return !(lhs < rhs);
    }

#pragma endregion

#pragma region storage
private:
    // end()是不变的
    void
    init_end() {
        auto* node = base_alloc.allocate(1);
        root.next = root.tail = node;
        node->next = nullptr;
        node->prev = &root;
    };

    // 析构并回收所有链表节点
    void
    destroy_all() {
        list_base_node* cur = root.next;
        list_base_node* next;
        while (cur != root.tail) {
            next = cur->next;
            destroy_node(cur);
            cur = next;
        }
        root.next = root.tail;
    }

    // 析构并回收单个链表节点
    void
    destroy_node(list_base_node *node) {
        node_alloc.template destroy(reinterpret_cast<list_node<T>*>(node));
        node_alloc.deallocate(reinterpret_cast<list_node<T>*>(node), 1);
        --root.size;
    }

    // 创建实体结点
    template<class... Args>
    list_node<T>*
    make_node(Args&&... args) {
        list_node<T>* node = node_alloc.allocate(1);
        alloc.template construct(alloc.address(node->data), std::forward<Args>(args)...);
        ++root.size;
        return node;
    }

    // 移动对象
    void move_storage(list& other) {
        root = other.root, other.root = {}, other.init_end();
    }

#pragma endregion


#pragma region 辅助函数
private:
    iterator
    insert_front(const_iterator it, list_base_node *node) {
        // pre->node->pos
        auto* pos = it.current;
        auto* pre = pos->prev;
        connect(pre, node), connect(node, pos);
        return iterator(node);
    }

    iterator
    insert_back(const_iterator it, list_base_node *node) {
        // pos->node->next
        auto* pos = it.current;
        auto* next = pos->next;
        connect(pos, node), connect(node, next);
        return iterator(node);
    };

    void
    connect(list_base_node* pre, list_base_node* next) {
        pre->next = next, next->prev = pre;
    }

    // 拷贝赋值
    template<typename InputIt>
    void
    assign_copy(InputIt first, InputIt last) {
        iterator cur = this->begin(), end = this->end();
        while (first != last && cur != end) {
            *cur++ = *first++;
        }
        while (first != last) {
            emplace(end, *first++);
        }
        erase(cur, end);
    }

    // 拷贝填充
    void
    assign_fill(size_type count, const T& value) {
        iterator cur = this->begin(), end = this->end();
        while (cur != end && count--) {
            *cur++ = value;
        }
        while (count--) {
            emplace(end, value);
        }
        erase(cur, end);
    }

    // 获取第i个元素的迭代器
    iterator
    at(size_type i) {
        return i < size() / 2
               ? anya::next(begin(), difference_type(i))
               : anya::prev(end(), difference_type(size() - i));
    }

#pragma endregion
};

// 特化 anya::swap 算法
template<class T, class Alloc>
constexpr void
swap(anya::list<T, Alloc>& lhs, anya::list<T, Alloc>& rhs) noexcept {
    lhs.swap(rhs);
}

}

#endif //ANYA_STL_LIST_HPP
