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
    for (size_t i = 0; i < std.size(); ++i)
        EXPECT_EQ(std[i], anya[i]);
    std.emplace_back(9);
    anya.emplace_back(9);

    EXPECT_EQ(std.size(), anya.size());
    EXPECT_EQ(std.capacity(), anya.capacity());
    EXPECT_EQ(std.max_size(), anya.max_size());
    for (size_t i = 0; i < std.size(); ++i)
        EXPECT_EQ(std[i], anya[i]);

    anya::vector<int> mnzn(anya.begin(), anya.end());
    EXPECT_EQ(mnzn.size(), anya.size());
    EXPECT_EQ(mnzn, anya);

    anya::vector<int> MnZn = mnzn;
    EXPECT_EQ(mnzn, MnZn);
}

TEST(VecTest, capacity) {
    std::vector<int> std(110, 0);
    anya::vector<int> anya(110, 0);
    std.reserve(111);
    anya.reserve(111);
    EXPECT_EQ(std.size(), anya.size());
    EXPECT_EQ(std.capacity(), anya.capacity());
    EXPECT_EQ(std.empty(), anya.empty());

    std.shrink_to_fit();
    anya.shrink_to_fit();
    EXPECT_EQ(std.size(), anya.size());
    EXPECT_EQ(std.capacity(), anya.capacity());
    EXPECT_EQ(std.empty(), anya.empty());


    std.reserve(55);
    anya.reserve(55);
    EXPECT_EQ(std.size(), anya.size());
    EXPECT_EQ(std.capacity(), anya.capacity());
    EXPECT_EQ(std.empty(), anya.empty());
}

TEST(VecTest, access) {
    std::vector<int> std(110, 0);
    anya::vector<int> anya(110, 0);
    std[55] = 56;
    anya[55] = 56;
    EXPECT_EQ(std.at(55), anya.at(55));
    EXPECT_EQ(std.front(), anya.front());
    EXPECT_EQ(std.back(), anya.back());
    EXPECT_TRUE(std.data() != anya.data());
}

TEST(VecTest, compare) {
    anya::vector<std::string> mnzn {"m", "n", "z", "n"};
    anya::vector<std::string> anya {"a", "n", "y", "a"};
    EXPECT_TRUE(anya < mnzn);
    EXPECT_TRUE(anya <= mnzn);
    EXPECT_FALSE(anya > mnzn);
    EXPECT_FALSE(anya >= mnzn);
    EXPECT_TRUE(anya != mnzn);
    std::vector<std::string> std_mnzn {"m", "n", "z", "n"};
    std::vector<std::string> std_anya {"a", "n", "y", "a"};
    EXPECT_TRUE(std_anya < std_mnzn);
    EXPECT_TRUE(std_anya <= std_mnzn);
    EXPECT_FALSE(std_anya > std_mnzn);
    EXPECT_FALSE(std_anya >= std_mnzn);
    EXPECT_TRUE(std_anya != std_mnzn);
}


TEST(VecTest, assign) {
    anya::vector<std::string> mnzn {"m", "n", "z", "n"};
    anya::vector<std::string> anya;
    anya = mnzn;
    EXPECT_EQ(mnzn, anya);
    anya = std::move(mnzn);
    mnzn = {"m", "n", "z", "n"};
    EXPECT_EQ(mnzn, anya);
    anya.assign({"a", "n", "y", "a"});
    mnzn.assign(anya.begin(), anya.end());
    EXPECT_EQ(mnzn, anya);
    anya.assign(10, "anya");
    mnzn.assign(10, "anya");
    EXPECT_EQ(mnzn, anya);
}

#endif //ANYA_STL_VECTOR_TEST_HPP
