//
// Created by Anya on 2023/5/9.
//

#ifndef ANYA_STL_DEQUE_TEST_HPP
#define ANYA_STL_DEQUE_TEST_HPP

#include "allocator/memory.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "container/deque.hpp"
#include <deque>

TEST(DequeTest, construct) {
    {
        anya::deque<int> anya{};
        EXPECT_TRUE(*anya.begin() == *anya.rbegin());
    }

    {
        anya::deque<int> anya(1);
        EXPECT_TRUE(*anya.begin() == *anya.rbegin());
    }

    {
        anya::deque<int> anya(1, 6);
        EXPECT_TRUE(*anya.begin() == *anya.rbegin());
    }
}

TEST(DequeTest, iterator) {
//    std::deque<int> std;
//    anya::deque<int> anya;
//    auto anya_cend = anya.cend();
//    auto anya_end = anya.end();
//    typename anya::deque<int>::iterator anya_temp1 = anya_cend;
//    typename anya::deque<int>::const_iterator anya_temp2 = anya_end;
//    auto std_cend = std.cend();
//    auto std_end = std.end();
//    typename std::deque<int>::iterator std_temp1 = std_cend;
//    typename std::deque<int>::const_iterator std_temp2 = std_end;

//    std::deque<int> std;
//    anya::deque<int> anya;
//    auto anya_crend = anya.crend();
//    auto anya_rend = anya.rend();
//    typename anya::deque<int>::reverse_iterator anya_temp1 = anya_crend;
//    typename anya::deque<int>::const_reverse_iterator anya_temp2 = anya_rend;
//    auto std_crend = std.crend();
//    auto std_rend = std.rend();
//    typename std::deque<int>::reverse_iterator std_temp1 = std_crend;
//    typename std::deque<int>::const_reverse_iterator std_temp2 = std_rend;
}

#endif //ANYA_STL_DEQUE_TEST_HPP
