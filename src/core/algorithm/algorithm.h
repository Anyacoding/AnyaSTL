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



}

#endif //ANYA_STL_ALGORITHM_H
