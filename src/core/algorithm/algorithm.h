//
// Created by Anya on 2023/4/4.
//

#ifndef ANYA_STL_ALGORITHM_H
#define ANYA_STL_ALGORITHM_H

#include <utility>

namespace anya {

#pragma region 最小/最大操作
template<typename T>
constexpr T min(const T &a, const T &b) {
    return a < b ? a : b;
}

template<typename T>
constexpr T max(const T &a, const T &b) {
    return a < b ? b : a;
}
#pragma endregion


#pragma region 修改序列的操作
template<class InputIt, class OutputIt>
constexpr OutputIt
move(InputIt first, InputIt last, OutputIt d_first) {
    while (first != last) {
        *d_first++ = std::move(*first++);
    }
    return d_first;
}

template<class BidirIt1, class BidirIt2>
constexpr BidirIt2
move_backward(BidirIt1 first, BidirIt1 last, BidirIt2 result_back) {
    while (first != last) *(--result_back) = std::move(*(--last));
    return result_back;
}
#pragma endregion


#pragma region 元素相等
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

#pragma endregion

}

#endif //ANYA_STL_ALGORITHM_H
