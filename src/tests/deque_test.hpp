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
        EXPECT_TRUE(anya.empty());
    }

    {
        anya::deque<int> anya(1);
        EXPECT_TRUE(*anya.begin() == *anya.rbegin());
    }

    {
        anya::deque<int> anya(1, 6);
        EXPECT_TRUE(*anya.begin() == *anya.rbegin());
    }

    {
        anya::deque<int> anya1(6, 0);
        anya::deque<int> anya2(6);
        // for (auto num : anya2) std::cout << num << " ";  std::cout << std::endl;
        EXPECT_TRUE(anya1 == anya2);
    }

    {
        anya::deque<int> anya1(5, 6);
        anya::deque<int> anya2 = anya1;
        EXPECT_TRUE(anya1 == anya2);
    }

    {
        anya::deque<int> anya1(7, 6);
        anya::deque<int> anya2 = std::move(anya1);
        EXPECT_FALSE(anya1 == anya2);
    }

    {
        anya::deque<int> anya1(4, 6);
        anya::deque<int> anya2 = {6, 6, 6, 6};
        EXPECT_TRUE(anya1 == anya2);
    }

    {
        anya::deque<int> anya1(4, 6);
        anya::deque<int> anya2(anya1.begin(), anya1.end());
        EXPECT_TRUE(anya1 == anya2);
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

TEST(DequeTest, access) {
    anya::deque<int> anya1{1, 1, 4, 5, 1, 4};
    EXPECT_TRUE(anya1.front() == 1);
    EXPECT_TRUE(anya1.back() == 4);
    EXPECT_TRUE(anya1.at(3) == anya1[3]);
    const anya::deque<int> anya3{2, 0, 0, 1, 4, 14};
    EXPECT_TRUE(anya3.front() == 2);
    EXPECT_TRUE(anya3.back() == 14);
    EXPECT_TRUE(anya3.at(5) == anya3[5]);
}

TEST(DequeTest, capacity) {
    anya::deque<int> anya1{1, 1, 4, 5, 1, 4};
    EXPECT_TRUE(anya1.size() == 6);
    anya::deque<int> anya2;
    EXPECT_TRUE(anya2.empty());
    EXPECT_TRUE(anya1.max_size() == anya1.max_size());
}

TEST(DequeTest, clear) {
    anya::deque<int> anya1{1, 1, 4, 5, 1, 4};
    anya1.clear();
    EXPECT_TRUE(anya1.size() == 0);
}

TEST(DequeTest, insert) {
    {
        anya::deque<int> anya1;
        anya::deque<int> anya2{1, 1, 4, 5, 1, 4};
        for (auto it = anya2.begin(); it != anya2.end(); ++it) {
            auto temp = *it;
            anya1.insert(anya1.end(), temp);
        }
        EXPECT_TRUE(anya1 == anya2);

    }
    {
        anya::deque<int> anya1;
        anya::deque<int> anya2{6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6};
        anya1.insert(anya1.end(), 15, 6);
        EXPECT_TRUE(anya1 == anya2);
    }
}

#endif //ANYA_STL_DEQUE_TEST_HPP
