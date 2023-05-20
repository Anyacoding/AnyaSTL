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
        using value_type        = typename deque_iterator::value_type;
        using difference_type   = typename deque_iterator::difference_type;
        using pointer           = typename deque_iterator::pointer;
        using reference         = typename deque_iterator::reference;

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
                       last   (const_cast<T*>(other.last)),
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
    deque() { initialize_map_node(0); }

    explicit deque(size_type count) { fill_initialize(count, T()); }

    deque(size_type count, const T& value) {
        fill_initialize(count, value);
    }

    template<class InputIt>
    requires std::derived_from<typename InputIt::iterator_category, anya::input_iterator_tag>
    deque(InputIt first, InputIt last) {
        using iterator_tag = anya::iter_category_t<InputIt>;
        if constexpr (std::is_same_v<iterator_tag, anya::input_iterator_tag>) {
            initialize_map_node(0);
            while (first != last) insert(finish, *first++);
        }
        else {
            size_t count = anya::distance(first, last);
            initialize_map_node(count);
            anya::uninitialized_copy_n(first, count, start);
        }
    }

    deque(const deque& other) {
        size_t count = other.size();
        initialize_map_node(count);
        anya::uninitialized_copy_n(other.begin(), count, start);
    }

    deque(deque&& other) {
        initialize_map_node(0);
        this->swap(other);
    }

    deque(std::initializer_list<T> init) {
        size_t count = anya::distance(init.begin(), init.end());
        initialize_map_node(count);
        anya::uninitialized_copy_n(init.begin(), count, start);
    }

    ~deque() {
        destroy_all_node();
        dealloc_node(start.first);
        map_alloc.deallocate(map_buffer, map_size);
        map_buffer = nullptr, map_size = 0;
    }

#pragma endregion

#pragma region 赋值
public:
    deque&
    operator=(const deque& other) {
        if (this != &other) assign(other.begin(), other.end());
        return *this;
    }

    deque&
    operator=(deque&& other) noexcept {
        if (this != &other) this->swap(other);
        return *this;
    }

    deque&
    operator=(std::initializer_list<T> ilist) {
        assign(ilist);
        return *this;
    }

    void
    assign(size_type count, const T& value) {
        clear();
        insert(end(), count, value);
    }

    template< class InputIt >
    void
    assign(InputIt first, InputIt last) {
        clear();
        insert(end(), first, last);
    }

    void
    assign(std::initializer_list<T> ilist) {
        clear();
        insert(end(), ilist);
    }

#pragma endregion



#pragma region 访问
public:
    [[nodiscard]] reference
    at(size_type pos) {
        if (pos >= size()) throw std::out_of_range("deque::at(size_type pos)");
        return start[pos];
    }

    [[nodiscard]] const_reference
    at(size_type pos) const {
        if (pos >= size()) throw std::out_of_range("deque::at(size_type pos)");
        return start[pos];
    }

    [[nodiscard]] reference
    operator[](size_type pos) { return start[pos]; }

    [[nodiscard]] const_reference
    operator[]( size_type pos ) const { return start[pos]; }

    [[nodiscard]] reference
    front() { return *start; }

    [[nodiscard]] const_reference
    front() const { return *start; }

    [[nodiscard]] reference
    back() { return *--iterator(finish); }

    [[nodiscard]] const_reference
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

    // 选择不予处理（摆烂）
    void
    shrink_to_fit() { }

#pragma endregion


#pragma region 修改器
public:
    void
    clear() noexcept { destroy_all_node(); }

    /*!
     * @param pos     将内容插入到它前面的迭代器, pos 可以是 end() 迭代器
     * @param value   要插入的元素值
     * @return        指向被插入 value 的迭代器
     */
    iterator
    insert(const_iterator pos, const T& value) {
        auto it = prepare_at(pos - start, 1);
        *it = value;
        return it;
    }

    /*!
     * @param pos     将内容插入到它前面的迭代器, pos 可以是 end() 迭代器
     * @param value   要插入的元素值
     * @return        指向被插入 value 的迭代器
     */
    iterator
    insert(const_iterator pos, T&& value) {
        auto it = prepare_at(pos - start, 1);
        *it = std::move(value);
        return it;
    }

    /*!
     * @param pos      将内容插入到它前面的迭代器, pos 可以是 end() 迭代器
     * @param count    副本个数
     * @param value    要插入的元素值
     * @return         指向首个被插入元素的迭代器，或者在 count == 0 时返回 pos
     */
    iterator
    insert(const_iterator pos, size_type count, const T& value) {
        auto it = prepare_at(pos - start, count);
        auto ret = it;
        while (count--) *it++ = value;
        return ret;
    }

    /*!
     * @tparam InputIt
     * @param pos       将内容插入到它前面的迭代器, pos 可以是 end() 迭代器
     * @param first     要插入的元素范围，不能是指向调用 insert 所用的容器中的迭代器
     * @param last      要插入的元素范围，不能是指向调用 insert 所用的容器中的迭代器
     * @return          指向首个被插入元素的迭代器，或者在 first == last 时返回 pos
     */
    template<class InputIt>
    requires std::derived_from<typename InputIt::iterator_category, anya::input_iterator_tag>
    iterator
    insert(const_iterator pos, InputIt first, InputIt last) {
        size_t index = pos - start;
        using iterator_tag = anya::iter_category_t<InputIt>;
        if constexpr (std::is_same_v<iterator_tag, anya::input_iterator_tag>) {
            while (first != last) insert(start + index, *first++), ++index;
        }
        else {
            size_t count = anya::distance(first, last);
            auto it = prepare_at(index, count);
            // 不能采用构造的方法，因为 prepare_at() 已经采用了构造
            // 重复构造会内存泄漏
            // anya::uninitialized_copy(first, last, it);
            anya::copy_n(first, count, it);
        }
        return start + index;
    }

    /*!
     * @param pos       将内容插入到它前面的迭代器, pos 可以是 end() 迭代器
     * @param ilist     要插入的值来源的 initializer_list
     * @return          指向首个被插入元素的迭代器，或者在 ilist 为空时返回 pos
     */
    iterator
    insert(const_iterator pos, std::initializer_list<T> ilist) {
        size_t index = pos - start;
        auto it = prepare_at(index, ilist.size());
        anya::copy_n(ilist.begin(), ilist.size(), it);
        return start + index;
    }

    template<class... Args>
    iterator
    emplace(const_iterator pos, Args&&... args) {
        auto index = pos - start;
        auto it = prepare_at(index, 1);
        *it = std::move(value_type(std::forward<Args>(args)...));
        return it;
    }

    iterator
    erase(const_iterator pos) {
        auto next = pos + 1;
        difference_type index = pos - start;
        if (index < size() / 2) {
            anya::move_backward(start, cast_to_iterator(pos), cast_to_iterator(next));
            pop_front();
        }
        else {
            anya::move(cast_to_iterator(next), finish, cast_to_iterator(pos));
            pop_back();
        }
        return start + index;
    }

    iterator
    erase(const_iterator first, const_iterator last) {
        if (first == start && last == finish) {
            return clear(), finish;
        }
        difference_type n = last - first;
        difference_type front_elem = first - last;
        if (front_elem < (size() - n) / 2) {
            anya::move_backward(start, cast_to_iterator(first), cast_to_iterator(last));
            iterator new_start = start + n;
            anya::destroy(start, new_start);
            for (map_pointer cur = start.node; cur < new_start.node; ++cur) {
                dealloc_node(*cur);
            }
            start = new_start;
        }
        else {
            anya::move(cast_to_iterator(last), finish, cast_to_iterator(first));
            iterator new_finish = finish - n;
            anya::destroy(new_finish, finish);
            for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur) {
                dealloc_node(*cur);
            }
            finish = new_finish;
        }
        return start + front_elem;
    }

    void
    push_back(const T& value) {
        emplace_back(value);
    }

    void
    push_back(T&& value) {
        emplace_back(std::forward<T>(value));
    }

    template<class... Args>
    reference
    emplace_back(Args&&... args) {
        return *emplace(end(), std::forward<Args>(args)...);
    }

    void
    pop_back() {
        if (finish.current != finish.first) {
            default_alloc.template destroy(--finish.current);
        }
        else {
            dealloc_node(finish.first);
            finish.set_node(finish.node - 1);
            finish.current = finish.last - 1;
            default_alloc.template destroy(finish.current);
        }
    }

    void
    push_front(const T& value) {
        emplace_front(value);
    }

    void
    push_front(T&& value) {
        emplace_front(std::forward<T>(value));
    }

    template<class... Args>
    reference
    emplace_front(Args&&... args) {
        return *emplace(begin(), std::forward<Args>(args)...);
    }

    void
    pop_front() {
        if (start.current != start.last - 1) {
            default_alloc.template destroy(start.current++);
        }
        else {
            default_alloc.template destroy(start.current);
            dealloc_node(start.first);
            start.set_node(start.node + 1);
            start.current = start.first;
        }
    }

    void
    resize(size_type count) {
        resize(count, value_type());
    }

    void
    resize(size_type count, const value_type& value) {
        size_type cur_size = size();
        if (count < cur_size) {
            erase(start + difference_type(count), finish);
        }
        else {
            insert(finish, count - cur_size, value);
        }
    }

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

    bool friend
    operator!=(const anya::deque<T, Allocator>& lhs,
               const anya::deque<T, Allocator>& rhs) {
        return !(lhs == rhs);
    };

    friend bool
    operator<(const anya::deque<T, Allocator>& lhs,
              const anya::deque<T, Allocator>& rhs) {
        // DONE: 将来替换成 anya::lexicographical_compare()
        return anya::lexicographical_compare(
            lhs.begin(), lhs.end(),
            rhs.begin(), rhs.end());
    }

    friend bool
    operator>(const anya::deque<T, Allocator>& lhs,
              const anya::deque<T, Allocator>& rhs) {
        return rhs < lhs;
    }

    friend bool
    operator<=(const anya::deque<T, Allocator>& lhs,
               const anya::deque<T, Allocator>& rhs) {
        return !(rhs < lhs);
    }

    friend bool
    operator>=(const anya::deque<T, Allocator>& lhs,
               const anya::deque<T, Allocator>& rhs) {
        return !(lhs < rhs);
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

    // 析构并回收所有结点,但是保留一个头结点维护容器的合法性
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
            // 头结点需要保留
            anya::destroy(start.current, finish.current);
        }
        finish = start;
    }

    void
    initialize_map_node(size_type element_count) {
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
    update_map_node(size_t count, bool at_front) {
        size_t old_nodes = finish.node - start.node + 1;
        size_t new_nodes = old_nodes + count;

        // 扩容策略为最少两倍，并且头尾都加1
        map_pointer new_start;
        size_t new_map_size = map_size + anya::max(map_size, count) + 2;
        map_pointer new_map = map_alloc.allocate(new_map_size);
        new_start = new_map + (new_map_size - new_nodes) / 2 + (at_front ? count : 0);
        // 将原来的node拷贝过去
        anya::copy(start.node, finish.node + 1, new_start);
        map_alloc.deallocate(map_buffer, map_size);
        map_buffer = new_map, map_size = new_map_size;
        // 重设迭代器的结点位置即可
        start.set_node(new_start);
        finish.set_node(new_start + old_nodes - 1);
    }

    void
    fill_initialize(size_type count, const value_type &value) {
        initialize_map_node(count);
        for (map_pointer cur = start.node; cur < finish.node; ++cur) {
            anya::uninitialized_fill_n(*cur, buffer_size, value);
        }
        anya::uninitialized_fill(finish.first, finish.current, value);
    }
#pragma endregion

#pragma region 工具函数
private:
    // 当前区间前面有几个空余元素位置
    size_type
    front_leave() {
        return difference_type(buffer_size) * (start.node - map_buffer) + (start.current - start.first);
    }

    // 当前区间后面有几个空余元素位置
    size_type
    back_leave() {
        return difference_type(buffer_size) * ((map_buffer + (map_size - 1)) - finish.node) + (finish.last - finish.current);
    }

    // 插入的准备工作
    iterator
    prepare_at(size_type index, size_type count) {
        size_t size = this->size();
        // 根据插入点位置选择左移或者右移
        if (index < size / 2) {
            // 左移front
            prepare_at_front(count);
            anya::move_n(start + difference_type(count), index, start);
        }
        else {
            // 右移back
            prepare_at_back(count);
            anya::move_n_backward(finish - difference_type(count), size - index, finish);
        }
        return start + difference_type(index);
    }

    // 在front腾出位置，并更新头迭代器
    void
    prepare_at_front(size_t count) {
        size_t leave = front_leave();
        // map_size 不够大，更新 map_size
        if (leave < count) {
            update_map_node((count - leave) / buffer_size + 1, true);
        }
        size_t node_leave = start.current - start.first;
        if (node_leave < count) {
            // 当前 node 剩余的 slot 不够装，申请分配新的 node 指向的内存
            size_t nodes = (count - node_leave) / buffer_size + 1;
            map_pointer cur = start.node - 1;
            while (nodes--) *cur-- = alloc_node();
        }
        start -= difference_type(count);
        anya::uninitialized_default_construct_n(start, count);
    }

    // 在back腾出位置，并更新尾迭代器
    void
    prepare_at_back(size_t count) {
        size_t leave = back_leave();
        // map_size 不够大，更新 map_size
        if (leave <= count) {
            update_map_node((count - leave) / buffer_size + 1, false);
        }
        size_t node_leave = finish.last - finish.current;
        if (node_leave < count) {
            // 当前 node 剩余的 slot 不够装，申请分配新的 node 指向的内存
            size_t nodes = (count - node_leave) / buffer_size + 1;
            map_pointer cur = finish.node + 1;
            while (nodes--) *cur++ = alloc_node();
        }
        anya::uninitialized_default_construct_n(finish, count);
        finish += difference_type(count);
    }

    iterator
    cast_to_iterator(const_iterator other) {
        iterator ret;
        ret.current = const_cast<T*>(other.current);
        ret.first   = const_cast<T*>(other.current);
        ret.last    = const_cast<T*>(other.last);
        ret.node    = const_cast<T**>(other.node);
        return ret;
    }
#pragma endregion

};

// 特化 anya::swap 算法
template<class T, class Alloc>
constexpr void
swap(anya::deque<T, Alloc>& lhs, anya::deque<T, Alloc>& rhs) noexcept {
    lhs.swap(rhs);
}

}

#endif //ANYA_STL_DEQUE_HPP
