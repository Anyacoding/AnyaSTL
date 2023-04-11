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
    EXPECT_EQ(mnzn.capacity(), anya.capacity());
    EXPECT_EQ(mnzn.empty(), anya.empty());

    anya.assign(10, "anya");
    mnzn.assign(10, "anya");
    EXPECT_EQ(mnzn, anya);

    anya::vector<size_t> temp1;
    temp1.assign(10, 10);
    anya::vector<size_t> temp2;
    temp2.assign(temp1.begin(), temp1.end());
    EXPECT_EQ(temp1, temp2);
    EXPECT_EQ(temp1.size(), temp2.size());
    EXPECT_EQ(temp1.capacity(), temp2.capacity());
}

TEST(VecTest, insert) {
    anya::vector<int> anya;
    std::vector<int>  stand;
    auto anya_it = anya.insert(anya.begin(), 6);
    auto std_it = stand.insert(stand.begin(), 6);
    EXPECT_EQ(anya::distance(anya.begin(), anya_it), std::distance(stand.begin(), std_it));
    EXPECT_EQ(stand.size(), anya.size());
    for (int i = 0; i < stand.size(); ++i)
        EXPECT_EQ(stand[i], anya[i]);

    int value = 9;
    anya_it = anya.insert(anya.begin(), value);
    std_it = stand.insert(stand.begin(), value);
    EXPECT_EQ(anya::distance(anya.begin(), anya_it), std::distance(stand.begin(), std_it));
    EXPECT_EQ(stand.size(), anya.size());
    for (int i = 0; i < stand.size(); ++i)
        EXPECT_EQ(stand[i], anya[i]);

    anya_it = anya.insert(anya.begin(), 9, value);
    std_it = stand.insert(stand.begin(), 9, value);

    EXPECT_EQ(anya::distance(anya.begin(), anya_it), std::distance(stand.begin(), std_it));
    EXPECT_EQ(stand.size(), anya.size());
    for (int i = 0; i < stand.size(); ++i)
        EXPECT_EQ(stand[i], anya[i]);

    anya::vector temp1 = {1, 1, 4, 5, 1, 4};
    std::vector temp2 = {1, 1, 4, 5, 1, 4};
    anya_it = anya.insert(anya.begin(), temp1.begin(), temp1.end());
    std_it = stand.insert(stand.begin(), temp2.begin(), temp2.end());
    EXPECT_EQ(anya::distance(anya.begin(), anya_it), std::distance(stand.begin(), std_it));
    EXPECT_EQ(stand.size(), anya.size());
    for (int i = 0; i < stand.size(); ++i)
        EXPECT_EQ(stand[i], anya[i]);
}


TEST(VecTest, erase) {
    anya::vector<int> anya { 1, 1, 4, 5, 1, 4 };
    std::vector<int>  stand { 1, 1, 4, 5, 1, 4 };
    auto anya_it = anya.erase(anya.begin() + 2);
    auto std_it = stand.erase(stand.begin() + 2);
    EXPECT_EQ(anya::distance(anya.begin(), anya_it), std::distance(stand.begin(), std_it));
    EXPECT_EQ(stand.size(), anya.size());
    EXPECT_EQ(stand.capacity(), anya.capacity());
    for (int i = 0; i < stand.size(); ++i) {
        EXPECT_EQ(stand[i], anya[i]);
    }

    anya_it = anya.erase(anya.begin() + 2, anya.end());
    std_it = stand.erase(stand.begin() + 2, stand.end());
    EXPECT_EQ(anya::distance(anya.begin(), anya_it), std::distance(stand.begin(), std_it));
    EXPECT_EQ(stand.size(), anya.size());
    EXPECT_EQ(stand.capacity(), anya.capacity());
    for (int i = 0; i < stand.size(); ++i) {
        EXPECT_EQ(stand[i], anya[i]);
    }
}

struct test {
    ~test() { std::cout << "die" << std::endl; }
    friend bool operator==(const test& a, const test& b) {
        return true;
    }
};

TEST(VecTest, push_back) {
    anya::vector<int> anya { 1, 1, 4, 5, 1, 4 };
    std::vector<int>  stand { 1, 1, 4, 5, 1, 4 };
    anya.push_back(6);
    stand.push_back(6);
    EXPECT_EQ(stand.size(), anya.size());
    EXPECT_EQ(stand.capacity(), anya.capacity());
    for (int i = 0; i < stand.size(); ++i) {
        EXPECT_EQ(stand[i], anya[i]);
    }

    anya.pop_back();
    stand.pop_back();
    EXPECT_EQ(stand.size(), anya.size());
    EXPECT_EQ(stand.capacity(), anya.capacity());
    for (int i = 0; i < stand.size(); ++i) {
        EXPECT_EQ(stand[i], anya[i]);
    }

    anya::vector<test> temp1(2);
    std::vector<test>  temp2(2);
    temp1.pop_back();
    temp2.pop_back();
    EXPECT_EQ(temp1.size(), temp2.size());
    EXPECT_EQ(temp1.capacity(), temp2.capacity());
    for (int i = 0; i < stand.size(); ++i) {
        EXPECT_EQ(temp1[i], temp2[i]);
    }
}

TEST(VecTest, resize) {
    anya::vector<int> anya { 1, 1, 4, 5, 1, 4 };
    std::vector<int>  stand { 1, 1, 4, 5, 1, 4 };
    anya.resize(anya.size() + 5);
    stand.resize(stand.size() + 5);
    EXPECT_EQ(stand.size(), anya.size());
    EXPECT_EQ(stand.capacity(), anya.capacity());
    for (int i = 0; i < stand.size(); ++i) {
        EXPECT_EQ(stand[i], anya[i]);
    }

    anya.resize(anya.size() + 5, 6);
    stand.resize(stand.size() + 5, 6);
    EXPECT_EQ(stand.size(), anya.size());
    EXPECT_EQ(stand.capacity(), anya.capacity());
    for (int i = 0; i < stand.size(); ++i) {
        EXPECT_EQ(stand[i], anya[i]);
    }

    anya.resize(anya.size() + 5, 9);
    stand.resize(stand.size() + 5, 9);
    EXPECT_EQ(stand.size(), anya.size());
    EXPECT_EQ(stand.capacity(), anya.capacity());
    for (int i = 0; i < stand.size(); ++i) {
        EXPECT_EQ(stand[i], anya[i]);
    }
}

TEST(VecTest, dilatation) {
    anya::vector<int> anya { 1, 1, 4, 5, 1, 4 };
    std::vector<int>  stand { 1, 1, 4, 5, 1, 4 };
    anya.reserve(12);
    stand.reserve(12);
    EXPECT_EQ(stand.size(), anya.size());
    EXPECT_EQ(stand.capacity(), anya.capacity());
    for (int i = 0; i < stand.size(); ++i) {
        EXPECT_EQ(stand[i], anya[i]);
    }

    anya.insert(anya.end(), 12, 6);
    stand.insert(stand.end(), 12, 6);
    EXPECT_EQ(stand.size(), anya.size());
    EXPECT_EQ(stand.capacity(), anya.capacity());
    for (int i = 0; i < stand.size(); ++i) {
        EXPECT_EQ(stand[i], anya[i]);
    }
}

#endif //ANYA_STL_VECTOR_TEST_HPP
