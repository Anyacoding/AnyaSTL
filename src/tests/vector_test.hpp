//
// Created by Anya on 2023/4/4.
//

#ifndef ANYA_STL_VECTOR_TEST_HPP
#define ANYA_STL_VECTOR_TEST_HPP

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "container/vector.hpp"

TEST(VecTest, construct) {
    std::vector<int> std(110, 0);
    anya::vector<int> anya(110, 0);
    EXPECT_EQ(std.size(), anya.size());
    EXPECT_EQ(std.capacity(), anya.capacity());
    EXPECT_EQ(std.empty(), anya.empty());
    std.emplace_back(9);
    anya.emplace_back(9);
    EXPECT_EQ(std.size(), anya.size());
    EXPECT_EQ(std.capacity(), anya.capacity());
    // auto cend = anya.cend();
    // typename anya::vector<int>::iterator end = cend;
    anya::vector<int> mnzn(anya.begin(), anya.end());
    EXPECT_EQ(mnzn.size(), anya.size());

    // TODO: 测试 mnzn == anya
}

#endif //ANYA_STL_VECTOR_TEST_HPP
