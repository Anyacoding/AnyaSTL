//
// Created by Anya on 2023/4/4.
//

#ifndef ANYA_STL_ALGORITHM_H
#define ANYA_STL_ALGORITHM_H

#include <utility>

namespace anya {

#pragma region 最小/最大操作
template<class T>
constexpr T min(const T &a, const T &b) {
    return a < b ? a : b;
}

template<class T>
constexpr T max(const T &a, const T &b) {
    return a < b ? b : a;
}
#pragma endregion


#pragma region 修改序列的操作
template<class ForwardIt, class OutputIt>
constexpr OutputIt
copy(ForwardIt first, ForwardIt last, OutputIt result) {
    while (first != last) *result++ = *first++;
    return result;
}

template<class ForwardIt, class OutputIt>
constexpr OutputIt
copy_n(ForwardIt first, size_t n, OutputIt result) {
    while (n--) *result++ = *first++;
    return result;
}

template<class InputIt, class OutputIt>
constexpr OutputIt
move(InputIt first, InputIt last, OutputIt d_first) {
    while (first != last) {
        *d_first++ = std::move(*first++);
    }
    return d_first;
}

template<class ForwardIt, class OutputIt>
constexpr OutputIt
move_n(ForwardIt first, size_t n, OutputIt result) {
    while (n--) *result++ = std::move(*first++);
    return result;
}

template<class BidirIt1, class BidirIt2>
constexpr BidirIt2
move_backward(BidirIt1 first, BidirIt1 last, BidirIt2 result_back) {
    while (first != last) *(--result_back) = std::move(*(--last));
    return result_back;
}

template<class BidirIt1, class BidirIt2>
constexpr BidirIt2
move_n_backward(BidirIt1 last, size_t n, BidirIt2 result_back) {
    while (n--) *(--result_back) = std::move(*(--last));
    return result_back;
}

#pragma endregion


#pragma region 比较操作
template<class InputIt1, class InputIt2>
constexpr bool
equal(InputIt1 first1, InputIt1 last1, InputIt2 first2) {
    for (; first1 != last1; ++first1, ++first2) {
        if (*first1 != *first2) {
            return false;
        }
    }
    return true;
}


template<class InputIt1, class InputIt2>
constexpr bool
equal(InputIt1 first1, InputIt1 last1,
           InputIt2 first2, InputIt2 last2) {
    if (anya::distance(first1,last1) != anya::distance(first2,last2))
        return false;
    for (; first1 != last1, first2 != last2; ++first1, ++first2) {
        if (*first1 != *first2) {
            return false;
        }
    }
    return true;
}

template<typename InputIt1, typename InputIt2>
constexpr bool
lexicographical_compare(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2) {
    for (; (first1 != last1) && (first2 != last2); ++first1, ++first2) {
        if (*first1 < *first2) return true;
        if (*first2 < *first1) return false;
    }
    return (first1 == last1) && (first2 != last2);
}

#pragma endregion

#pragma region 堆操作
using heap_index = std::size_t;

// pos > 0 时有parent
constexpr heap_index
parent_index(heap_index pos) {
    return (pos - 1) >> 1;
}

constexpr heap_index
left_index(heap_index pos) {
    return (pos << 1) | 1;
}

constexpr heap_index
right_index(heap_index pos) {
    return (pos + 1) << 1;
}

// 上浮最后一个元素
template<class RandomIt, class Compare>
constexpr void
swim_heap(RandomIt first, heap_index pos, Compare comp) {
    heap_index parent = parent_index(pos);
    // pos == 0 时说明已经到根节点了，不需要再上浮了
    while (pos && comp(first[parent], first[pos])) {
        std::swap(first[parent], first[pos]);
        pos = parent, parent = parent_index(pos);
    }
}

// 从 left, right, parent 中选取一个最大的下标
template<class RandomIt, class Compare>
constexpr heap_index
select_pos(RandomIt first, heap_index parent, heap_index heap_size, Compare comp) {
    heap_index ret = parent, l = left_index(parent), r = right_index(parent);
    if (l < heap_size && comp(first[ret], first[l])) ret = l;
    if (r < heap_size && comp(first[ret], first[r])) ret = r;
    return ret;
}

// 下沉第一个元素
template<class RandomIt, class Compare>
constexpr void
sink_heap(RandomIt first, heap_index parent, heap_index heap_size, Compare comp) {
    heap_index pos;
    while ((pos = select_pos(first, parent, heap_size, comp)) != parent) {
        std::swap(first[parent], first[pos]), parent = pos;
    }
}

// 检验范围 [first, last) 并寻找始于 first 且为最大堆的最大范围
template<class RandomIt, class Compare>
constexpr RandomIt
is_heap_until(RandomIt first, RandomIt last, Compare comp) {
    if (first == last) return last;
    RandomIt pre = first, cur = ++first;
    bool add_pre = false;
    while (cur != last) {
        if (comp(*pre, *cur) == true) break;
        else ++cur;
        if (add_pre) ++pre, add_pre = false;
        else add_pre = true;
    }
    return cur;
}

template<class RandomIt>
constexpr RandomIt
is_heap_until(RandomIt first, RandomIt last) {
    return anya::is_heap_until(first, last, std::less<>());
}

// 检验范围内是否为堆
template<class RandomIt, class Compare>
constexpr bool
is_heap(RandomIt first, RandomIt last, Compare comp) {
    return anya::is_heap_until(first, last, comp) == last;
}

template<class RandomIt>
constexpr bool
is_heap(RandomIt first, RandomIt last) {
    return anya::is_heap(first, last, std::less<>());
}

// 插入位于位置 last-1 的元素到范围 [first, last-1) 所定义的最大堆中
template<class RandomIt, class Compare>
constexpr void
push_heap(RandomIt first, RandomIt last, Compare comp) {
    if (last <= first + 1) return;
    heap_index pos = last - first - 1;
    swim_heap(first, pos, comp);
}

template<class RandomIt>
constexpr void
push_heap(RandomIt first, RandomIt last) {
    return anya::push_heap(first, last, std::less<>());
}

// 将first位置处的元素从[first,last)的堆中移除,放置到last-1处
template<class RandomIt, class Compare>
constexpr void
pop_heap(RandomIt first, RandomIt last, Compare comp) {
    if (last <= first + 1) return;
    heap_index heap_size = last - first - 1;
    std::swap(*first, *(first + heap_size));
    sink_heap(first, 0, heap_size, comp);
}

template<class RandomIt>
constexpr void
pop_heap(RandomIt first, RandomIt last) {
    return anya::pop_heap(first, last, std::less<>());
}

// 将[first,last)的中的元素整理为一个堆
template<class RandomIt, class Compare>
constexpr void
make_heap(RandomIt first, RandomIt last, Compare comp) {
    if (last <= first + 1) return;
    heap_index heap_size = last - first, parent = heap_size / 2;
    while (true) {
        sink_heap(first, parent, heap_size, comp);
        if (parent-- == 0) break;
    }
}

template<class RandomIt>
constexpr void
make_heap(RandomIt first, RandomIt last) {
    return anya::make_heap(first, last, std::less<>());
}

// 将[first,last)的堆排序为升序序列
template<class RandomIt, class Compare>
constexpr void
sort_heap(RandomIt first, RandomIt last, Compare comp) {
    while (last > first + 1) anya::pop_heap(first, last--, comp);
}

template<class RandomIt>
constexpr void
sort_heap(RandomIt first, RandomIt last) {
    return anya::sort_heap(first, last, std::less<>());
}

#pragma endregion

}

#endif //ANYA_STL_ALGORITHM_H
