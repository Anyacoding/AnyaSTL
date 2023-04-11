//
// Created by Anya on 2023/3/12.
//

#ifndef ANYA_STL_VECTOR_HPP
#define ANYA_STL_VECTOR_HPP

#include "allocator/memory.hpp"
#include "iterator/iterator.hpp"
#include "algorithm/algorithm.h"
#include <concepts>
#include <type_traits>

namespace anya {

template<class T,
         class Allocator = anya::allocator<T>>
class vector {
private:
    static_assert(std::is_same<typename std::remove_cv<T>::type, T>::value,
                  "std::vector must have a non-const, non-volatile value_type");
    static_assert(std::is_same<typename Allocator::value_type, T>::value,
                  "std::vector must have the same value_type as its allocator");
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
    using iterator               = anya::normal_iterator<pointer, vector>;
    using const_iterator         = anya::normal_iterator<const_pointer, vector>;
    using const_reverse_iterator = anya::reverse_iterator<const_iterator>;
    using reverse_iterator       = anya::reverse_iterator<iterator>;

private:
    T* start{};             // 起始指针
    T* finish{};            // 逻辑上的结尾指针
    T* end_of_storage{};    // 内存实际分配的末尾指针
    allocator_type alloc{}; // 内存分配器

#pragma region 构造 && 析构
public:
    constexpr vector() noexcept(noexcept(Allocator())) = default;

    constexpr vector(size_type count, const T& value) {
        alloc_storage(count);
        finish = anya::uninitialized_fill_n(start, count, value);
    }

    constexpr explicit vector(size_type count) {
        alloc_storage(count);
        finish = anya::uninitialized_default_construct_n(start, count);
    }

    // 这里需要约束确实是迭代器类型，否则会产生歧义
    template<class InputIt>
    requires std::derived_from<typename InputIt::iterator_category, anya::input_iterator_tag>
    constexpr vector(InputIt first, InputIt last) {
        using iterator_tag = anya::iter_category_t<InputIt>;
        if constexpr (std::is_same_v<iterator_tag, anya::input_iterator_tag>) {
            while (first != last) emplace_back(*first++);
        }
        else {
            size_t n = anya::distance(first, last);
            alloc_storage(n);
            finish = anya::uninitialized_copy_n(first, n, start);
        }
    }

    constexpr vector(const vector& other) {
        alloc_storage(other.size());
        finish = anya::uninitialized_copy(other.begin(), other.end(), start);
    }

    constexpr vector(vector&& other) noexcept {
        move_storage(other);
    }

    constexpr vector(std::initializer_list<T> init) {
        alloc_storage(init.size());
        finish = anya::uninitialized_copy(init.begin(), init.end(), start);
    }

    constexpr ~vector() {
        destroy_storage();
        deallocate_storage();
    }

#pragma endregion

#pragma region 赋值
    constexpr vector&
    operator=(const vector& other) {
        if (this == &other)
            return *this;
        assign_aux(other.begin(), other.end(), other.size());
        return *this;
    }

    constexpr vector&
    operator=(vector&& other) noexcept {
        if (this == &other)
            return *this;
        move_storage(other);
        return *this;
    }

    constexpr vector&
    operator=(std::initializer_list<T> ilist) {
        assign_aux(ilist.begin(), ilist.end(), ilist.size());
        return *this;
    }

    constexpr void
    assign(size_type count, const T& value) {
        destroy_storage();
        if (count > capacity()) { // 需要扩容
            deallocate_storage();
            update_capacity(count);
        }
        finish = anya::uninitialized_fill_n(start, count, value);
    }

    // 其中有任何一个迭代器是指向 *this 中的迭代器时行为未定义
    template<class InputIt>
    requires std::derived_from<typename InputIt::iterator_category, anya::input_iterator_tag>
    constexpr void
    assign(InputIt first, InputIt last) {
        using iterator_tag = anya::iter_category_t<InputIt>;
        if constexpr (std::is_same_v<iterator_tag, anya::input_iterator_tag>) {
            destroy_storage();
            while (first != last) emplace_back(*first++);
            shrink_to_fit();
        }
        else {
            assign_aux(first, last, anya::distance(first, last));
        }
    }

    constexpr void
    assign(std::initializer_list<T> ilist) {
        assign_aux(ilist.begin(), ilist.end(), ilist.size());
    }

#pragma endregion

#pragma region 元素访问
public:
    constexpr allocator_type
    get_allocator() const noexcept { return alloc; };

    constexpr reference
    at(size_type pos) {
        if (pos >= size())
            throw std::out_of_range("pos out of range of the vector");
        return start[pos];
    };

    constexpr const_reference
    at(size_type pos) const {
        if (pos >= size())
            throw std::out_of_range("index out of range of the vector");
        return start[pos];
    }

    constexpr reference
    operator[](size_type pos) { return start[pos]; }

    constexpr const_reference
    operator[](size_type pos) const { return start[pos]; }

    constexpr reference
    front() { return *start; }

    constexpr const_reference
    front() const { return *start; }

    constexpr reference
    back() { return *(finish - 1); }

    constexpr const_reference
    back() const { return *(finish - 1); }

    constexpr T*
    data() noexcept { return start; }

    constexpr const T*
    data() const noexcept { return start; }

#pragma endregion

#pragma region 迭代器
public:
    constexpr iterator
    begin() noexcept { return iterator(start); }

    constexpr const_iterator
    begin() const noexcept { return const_iterator(start); }

    constexpr const_iterator
    cbegin() const noexcept { return const_iterator(start); }

    constexpr reverse_iterator
    rbegin() noexcept { return reverse_iterator(end()); }

    constexpr const_reverse_iterator
    rbegin() const noexcept { return const_reverse_iterator(cend()); }

    constexpr const_reverse_iterator
    crbegin() const noexcept { return const_reverse_iterator(cend()); }

    constexpr iterator
    end() noexcept { return iterator(finish); }

    constexpr const_iterator
    end() const noexcept { return const_iterator(finish); }

    constexpr const_iterator
    cend() const noexcept { return const_iterator(finish); }

    constexpr reverse_iterator
    rend() noexcept { reverse_iterator(begin()); }

    constexpr const_reverse_iterator
    rend() const noexcept { const_reverse_iterator(cbegin()); }

    constexpr const_reverse_iterator
    crend() const noexcept { const_reverse_iterator(cbegin()); }

#pragma endregion

#pragma region 容量
public:
    [[nodiscard]] constexpr size_type
    capacity() const noexcept { return end_of_storage - start; }

    [[nodiscard]] constexpr size_type
    size() const noexcept { return finish - start; }

    [[nodiscard]] constexpr bool
    empty() const noexcept { return start == finish; }

    [[nodiscard]] constexpr size_type
    max_size() const noexcept { return alloc.max_size(); }

    constexpr void
    reserve(size_type new_cap) {
        if (new_cap > capacity()) {
            update_capacity(new_cap);
        }
    }

    constexpr void
    shrink_to_fit() {
        if (finish < end_of_storage) {
            update_capacity(size());
        }
    }

#pragma endregion


#pragma region 修改器
public:
    constexpr void
    clear() noexcept { destroy_storage(); }

    /*!
     * @param pos    将内容插入到它前面的迭代器。pos可以是end()迭代器
     * @param value  要插入的元素值
     * @return       指向被插入value的迭代器
     */
    constexpr iterator
    insert(const_iterator pos, const T& value) {
        size_t index = pos - cbegin();
        prepare_to_insert(index, 1);
        alloc.construct(start + index, value);
        return begin() + index;
    }

    /*!
     * @param pos    将内容插入到它前面的迭代器。pos可以是end()迭代器
     * @param value  要插入的元素值
     * @return       指向被插入value的迭代器
     */
    constexpr iterator
    insert(const_iterator pos, T&& value) {
        size_t index = pos - cbegin();
        prepare_to_insert(index, 1);
        alloc.construct(start + index, std::move(value));
        return begin() + index;
    }

    /*!
     * @param pos    将内容插入到它前面的迭代器。pos可以是end()迭代器
     * @param count  要插入的元素个数
     * @param value  要插入的元素值
     * @return       指向首个被插入元素的迭代器，或者在count == 0时返回pos
     */
    constexpr iterator
    insert(const_iterator pos, size_type count, const T& value) {
        size_t index = pos - cbegin();
        prepare_to_insert(index, count);
        anya::uninitialized_fill_n(begin() + index, count, value);
        return begin() + index;
    }

    /*!
     * @tparam InputIt
     * @param pos    将内容插入到它前面的迭代器。pos可以是end()迭代器
     * @param first  要插入的元素范围，
     * @param last   不能是指向调用insert所用的容器中的迭代器
     * @return       指向首个被插入元素的迭代器，或者在 first == last 时返回 pos
     */
    template<class InputIt>
    requires std::derived_from<typename InputIt::iterator_category, anya::input_iterator_tag>
    constexpr iterator
    insert(const_iterator pos, InputIt first, InputIt last) {
        size_t index = pos - cbegin();
        using iterator_tag = anya::iter_category_t<InputIt>;
        if constexpr (std::is_same_v<iterator_tag, anya::input_iterator_tag>) {
            vector temp(first, last);
            size_t n = temp.size();
            prepare_to_insert(index, n);
            anya::uninitialized_copy(temp.begin(), temp.end(), begin() + index);
        }
        else {
            size_t n = anya::distance(first, last);
            prepare_to_insert(index, n);
            anya::uninitialized_copy(first, last, begin() + index);
        }
        return begin() + index;
    }

    /*!
     * @param pos   将内容插入到它前面的迭代器。pos 可以是 end() 迭代器
     * @param ilist 要插入的值来源的 initializer_list
     * @return      指向首个被插入元素的迭代器，或者在 ilist 为空时返回 pos
     */
    constexpr iterator
    insert(const_iterator pos, std::initializer_list<T> ilist) {
        size_t index = pos - cbegin();
        prepare_to_insert(index, ilist.size());
        anya::uninitialized_copy(ilist.begin(), ilist.end(), begin() + index);
        return begin() + index;
    }


    /*!
     * @tparam Args
     * @param pos   将构造新元素到其前的迭代器
     * @param args  转发给元素构造函数的参数
     * @return      转发给元素构造函数的参数
     */
    template<class... Args>
    constexpr iterator
    emplace(const_iterator pos, Args&&... args) {
        size_t index = pos - cbegin();
        prepare_to_insert(index, 1);
        alloc.template construct(start + index, std::forward<Args>(args)...);
        return begin() + index;
    };

    template<class... Args>
    constexpr reference
    emplace_back(Args&&... args) {
        return *emplace(end(), std::forward<Args>(args)...);
    };

#pragma endregion

#pragma region 友元比较函数
public:
    bool friend
    operator==(const anya::vector<T, Allocator>& lhs,
               const anya::vector<T, Allocator>& rhs) {
        if (lhs.size() != rhs.size())
            return false;
        if (&lhs == & rhs || lhs.start == rhs.start)
            return true;
        for (size_t i = 0; i < lhs.size(); ++i) {
            if (lhs[i] != rhs[i])
                return false;
        }
        return true;
    };

    bool friend
    operator!=(const anya::vector<T, Allocator>& lhs,
               const anya::vector<T, Allocator>& rhs) {
        return !(lhs == rhs);
    };

    friend bool
    operator<(const anya::vector<T, Allocator>& lhs,
              const anya::vector<T, Allocator>& rhs) {
        // TODO: 将来替换成 anya::lexicographical_compare()
        return std::lexicographical_compare(
            lhs.begin(), lhs.end(),
            rhs.begin(), rhs.end());
    }

    friend bool
    operator>(const anya::vector<T, Allocator>& lhs,
              const anya::vector<T, Allocator>& rhs) {
        return rhs < lhs;
    }

    friend bool
    operator<=(const anya::vector<T, Allocator>& lhs,
               const anya::vector<T, Allocator>& rhs) {
        return !(rhs < lhs);
    }

    friend bool
    operator>=(const anya::vector<T, Allocator>& lhs,
               const anya::vector<T, Allocator>& rhs) {
        return !(lhs < rhs);
    }

#pragma endregion


#pragma region storage
private:
    // 开辟内存但不构造
    void
    alloc_storage(size_t n) {
        start = finish = alloc.allocate(n);
        end_of_storage = start + n;
    };

    // 移动左值已有的内容
    void
    move_storage(vector &x) {
        anya::destroy(start, finish);
        alloc.deallocate(start, capacity());
        start = x.start;
        finish = x.finish;
        end_of_storage = x.end_of_storage;
        x.start = x.end_of_storage = x.finish = nullptr;
    }

    // 析构对象，但不回收内存
    void
    destroy_storage() {
        anya::destroy(start, finish);
        finish = start;
    }

    // 回收内存,不负责销毁
    void deallocate_storage() {
        alloc.deallocate(start, capacity());
        start = end_of_storage = finish = nullptr;
    }

#pragma endregion


#pragma region 工具函数
    // 插入的准备工作
    void
    prepare_to_insert(size_t pos, size_t n) {
        if (n == 0) return;
        size_t new_size = this->size() + n;
        if (new_size > capacity()) {
            update_capacity(anya::max(new_size, dilatation(capacity())));
        }
        pointer insert_pos = start + pos;
        anya::uninitialized_default_construct_n(finish, n);
        // 从后往前挪动空出n个位置，并析构原来的元素
        anya::move_backward(insert_pos, finish, finish + n);
        anya::destroy(insert_pos, insert_pos + n);
        finish += n;
    }

    // 更新容器容量
    void
    update_capacity(size_type new_cap) {
        size_type old_cap = capacity();
        if (new_cap > old_cap) {
            auto new_start = alloc.allocate(new_cap);
            auto new_finish = anya::uninitialized_move(start, finish, new_start);
            anya::destroy(start, finish);
            alloc.deallocate(start, old_cap);
            start = new_start, finish = new_finish;
            end_of_storage = start + new_cap;
        }
        // TODO: 检查正确性
        else if (new_cap <= old_cap && new_cap >= size()) {
            alloc.deallocate(start + new_cap, old_cap - new_cap);
            end_of_storage = start + new_cap;
        }
        else if (new_cap <= old_cap && new_cap < size()) {
            anya::destroy(start + new_cap, finish);
            alloc.deallocate(start + new_cap, old_cap - new_cap);
            finish = start + new_cap;
            end_of_storage = start + new_cap;
        }
    }

    // 销毁旧对象,以新对象初始化
    template<class InputIt>
    void
    assign_aux(InputIt first, InputIt last, size_t n) {
        // 析构原来的元素
        destroy_storage();
        if (n > capacity()) {
            deallocate_storage();
            alloc_storage(n);
        }
        finish = anya::uninitialized_copy(first, last, start);
    }

    // 扩容策略
    constexpr static size_t
    dilatation(size_t cur) {
        return (size_type)(2 * cur);
    }

#pragma endregion

};

}


#endif //ANYA_STL_VECTOR_HPP
