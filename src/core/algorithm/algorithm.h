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
bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2) {
    for (; first1 != last1; ++first1, ++first2) {
        if (*first1 != *first2) {
            return false;
        }
    }
    return true;
}


template<class InputIt1, class InputIt2>
bool equal(InputIt1 first1, InputIt1 last1,
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

}

#endif //ANYA_STL_ALGORITHM_H
