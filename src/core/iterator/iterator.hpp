//
// Created by Anya on 2023/3/7.
//

#ifndef ANYA_STL_ITERATOR_HPP
#define ANYA_STL_ITERATOR_HPP

#include <cstddef>
#include <iterator>

namespace anya {

#pragma region 迭代器标签
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag       : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};
struct contiguous_iterator_tag    : public random_access_iterator_tag {};
#pragma endregion

#pragma region 迭代器的默认声明
template<
    class Category,
    class T,
    class Distance  = std::ptrdiff_t,
    class Pointer   = T*,
    class Reference = T&>
struct iterator {
    using iterator_category = Category;
    using value_type        = T;
    using difference_type   = Distance;
    using pointer           = Pointer;
    using reference         = Reference;
};

#pragma endregion

#pragma region 迭代器的类型萃取器
// 主模板
template<class Iter>
struct iterator_traits {
    using iterator_category = typename Iter::iterator_category;
    using value_type        = typename Iter::value_type;
    using difference_type   = typename Iter::difference_type;
    using pointer           = typename Iter::pointer;
    using reference         = typename Iter::reference;
};

// 对原生指针做偏特化
template<class T>
struct iterator_traits<T*> {
    using iterator_category = random_access_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;
};

template<class T>
struct iterator_traits<const T*> {
    using iterator_category = random_access_iterator_tag;
    using value_type        = const T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const T*;
    using reference         = const T&;
};

// 定义一组便捷的别名模板
template<class T>
using iter_category_t   = typename iterator_traits<T>::iterator_category;

template<class T>
using iter_value_t      = typename iterator_traits<T>::value_type;

template<class T>
using iter_difference_t = typename iterator_traits<T>::difference_type;

template<class T>
using iter_pointer_t    = typename iterator_traits<T>::pointer;

template<class T>
using iter_reference_t  = typename iterator_traits<T>::reference;

#pragma endregion

#pragma region 通用迭代器实现
template<class Iterator, class Container>
class normal_iterator {
protected:
    Iterator current;
    using iterator_type = Iterator;

public:
    using iterator_category = iter_category_t   <iterator_type>;
    using value_type        = iter_value_t      <iterator_type>;
    using difference_type   = iter_difference_t <iterator_type>;
    using reference         = iter_reference_t  <iterator_type>;
    using pointer           = iter_pointer_t    <iterator_type>;

public:
    normal_iterator() noexcept : current{} {}

    normal_iterator(const normal_iterator& it) noexcept : current(it.current) {}

    normal_iterator(normal_iterator&&) noexcept = default;

    explicit normal_iterator(const Iterator& it) noexcept : current(it) {}

    normal_iterator& operator=(const normal_iterator&) noexcept = default;

    ~normal_iterator() = default;

public:
    constexpr reference
    operator*() const noexcept { return *current; }

    constexpr pointer
    operator->() const noexcept { return current; }

    constexpr normal_iterator&
    operator++() noexcept {
        return ++current, *this;
    }

    constexpr normal_iterator
    operator++(int) noexcept {
        return normal_iterator(current++);
    }

    constexpr normal_iterator&
    operator--() noexcept {
        return --current, *this;
    }

    constexpr normal_iterator
    operator--(int) noexcept {
        return normal_iterator(current--);
    }

    constexpr reference
    operator[](difference_type n) const noexcept {
        return *(current + n);
    }

    constexpr normal_iterator&
    operator+=(difference_type n) noexcept {
        return current += n, *this;
    }

    constexpr normal_iterator
    operator+(difference_type n) const noexcept {
        return normal_iterator(current + n);
    }

    constexpr normal_iterator&
    operator-=(difference_type n) noexcept {
        return current -= n, *this;
    }

    constexpr normal_iterator
    operator-(difference_type n) const noexcept {
        return normal_iterator(current - n);
    }

    constexpr const Iterator&
    base() const noexcept { return current; }

    friend bool
    operator==(const normal_iterator& lhs, const normal_iterator& rhs) {
        return lhs.current == rhs.current;
    }

    friend bool
    operator!=(const normal_iterator& lhs, const normal_iterator& rhs) {
        return rhs.current != lhs.current;
    }

    friend bool
    operator<(const normal_iterator& lhs, const normal_iterator& rhs) {
        return lhs.current < rhs.current;
    }

    friend bool
    operator>(const normal_iterator& lhs, const normal_iterator& rhs) {
        return lhs.current > rhs.current;
    }

    friend bool
    operator<=(const normal_iterator& lhs, const normal_iterator& rhs) {
        return lhs.current <= rhs.current;
    }

    friend bool
    operator>=(const normal_iterator& lhs, const normal_iterator& rhs) {
        return lhs.current >= rhs.current;
    }

    friend difference_type
    operator-(const normal_iterator& lhs, const normal_iterator& rhs) {
        return lhs.current - rhs.current;
    }

    friend normal_iterator
    operator+(difference_type n, const normal_iterator& rhs) {
        return rhs + n;
    }
};

#pragma endregion


#pragma region 迭代器操作

// advance函数组辅助函数
template<class InputIt, class Distance>
constexpr void
advance_aux(InputIt& it, Distance n, input_iterator_tag) {
    while (n--) ++it;
}

template<class BidirectionalIt, class Distance>
constexpr void
advance_aux(BidirectionalIt& it, Distance n, bidirectional_iterator_tag) {
    if (n >= 0)
        while (n--) ++it;
    else
        while (n++) --it;
}

template<class RandomAccessIt, class Distance>
constexpr void
advance_aux(RandomAccessIt& it, Distance n, random_access_iterator_tag) {
    it += n;
}

/*!
 * @tparam InputIt
 * @tparam Distance
 * @param it 迭代器起始位置
 * @param n  迭代器位移步数
 * @return
 */
template<class InputIt, class Distance>
constexpr void
advance(InputIt& it, Distance n) {
    advance_aux(it, n, iter_category_t<InputIt>());
}

// distance函数组辅助函数
template<class InputIt>
constexpr iter_difference_t<InputIt>
distance_aux(InputIt first, InputIt last, input_iterator_tag) {
    iter_difference_t<InputIt> n = 0;
    while (first != last) {
        ++first, ++n;
    }
    return n;
}

template<class RandomAccessIt>
constexpr iter_difference_t<RandomAccessIt>
distance_aux(RandomAccessIt first, RandomAccessIt last, random_access_iterator_tag) {
    return last - first;
}

/*!
 * @tparam InputIt
 * @param first  迭代器起始位置
 * @param last   迭代器结束位置
 * @return       迭代器之间的距离
 */
template<class InputIt>
constexpr iter_difference_t<InputIt>
distance(InputIt first, InputIt last) {
    return distance_aux(first, last, iter_category_t<InputIt>());
}

#pragma endregion

}

#endif //ANYA_STL_ITERATOR_HPP
