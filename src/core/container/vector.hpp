//
// Created by Anya on 2023/3/12.
//

#ifndef ANYA_STL_VECTOR_HPP
#define ANYA_STL_VECTOR_HPP

#include "allocator/memory.hpp"
#include "iterator/iterator.hpp"
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

    template<class InputIt>
    constexpr vector(InputIt first, InputIt last) {
        using iterator_tag = anya::iter_category_t<InputIt>;
        if constexpr (std::is_same_v<iterator_tag, anya::input_iterator_tag>) {
            // TODO: 待完成 emplace_back() 可补全
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

#pragma region 迭代器
public:
    constexpr iterator
    begin() noexcept { return iterator(start); }

    constexpr const_iterator
    begin() const noexcept { return const_iterator(start); }

    constexpr const_iterator
    cbegin() const noexcept { return const_iterator(start); }

#pragma endregion

#pragma region 修改器
public:
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
        // TODO: 未完成
        return begin() + index;
    };

    template<class... Args>
    constexpr reference
    emplace_back(Args&&... args) {

    };

#pragma endregion


#pragma region 容量
public:
    [[nodiscard]] constexpr size_type
    capacity() const noexcept { return end_of_storage - start; }


#pragma endregion


#pragma region 工具函数
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

    // 插入的准备工作
    void
    prepare_to_insert(size_t pos, size_t n) {
        if (n == 0) return;
        // TODO: 未完成
    }

#pragma endregion

};

}


#endif //ANYA_STL_VECTOR_HPP
