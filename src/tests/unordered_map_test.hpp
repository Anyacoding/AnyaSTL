//
// Created by Anya on 2023/6/4.
//

#ifndef ANYA_STL_UNORDERED_MAP_TEST_HPP
#define ANYA_STL_UNORDERED_MAP_TEST_HPP

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "container/unordered_map.hpp"
#include "algorithm/algorithm.h"
#include <unordered_map>

TEST(UnMapTest, construct) {
    {
        anya::unordered_map<int, int> hash;
        EXPECT_TRUE(hash.empty());
    }

    {
        anya::deque<std::pair<int, int>> anya1(4, {6, 6});
        anya::unordered_map<int, int> hash1(anya1.begin(), anya1.end());
        anya::unordered_map<int, int> hash2 = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
        std::unordered_map<int, int> std = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
        EXPECT_TRUE(hash1 == hash2);
    }

    {
        anya::unordered_map<int, int> hash1 = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
        anya::unordered_map<int, int> hash2 = hash1;
        EXPECT_TRUE(hash1 == hash2);
    }

    {
        anya::unordered_map<int, int> hash1 = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
        anya::unordered_map<int, int> hash2 = std::move(hash1);
        EXPECT_TRUE(hash1 != hash2);
    }
}

TEST(UnMapTest, assign) {
    {
        anya::unordered_map<int, int> hash1 = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
        anya::unordered_map<int, int> hash2;
        hash2 = hash1;
        EXPECT_TRUE(hash1 == hash2);
    }

    {
        anya::unordered_map<int, int> hash1 = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
        anya::unordered_map<int, int> hash2;
        hash2 = std::move(hash1);
        EXPECT_TRUE(hash1 != hash2);
    }
}

TEST(UnMapTest, capacity) {
    anya::unordered_map<int, int> hash1 = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
    EXPECT_TRUE(hash1.size() == 1);
    anya::unordered_map<int, int> hash2;
    EXPECT_TRUE(hash2.empty());
    EXPECT_TRUE(hash1.max_size() == hash2.max_size());
}

TEST(UnMapTest, clear) {
    anya::unordered_map<int, int> hash1 = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
    hash1.clear();
    EXPECT_TRUE(hash1.size() == 0);
}

TEST(UnMapTest, insert) {
    {
        anya::unordered_map<int, int> hash1 = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
        anya::unordered_map<int, int> hash2;
        anya::deque<std::pair<int, int>> anya2(4, {6, 6});
        for (auto it = anya2.begin(); it != anya2.end(); ++it) {
            auto temp = *it;
            hash2.insert(temp);
        }
        EXPECT_TRUE(hash1 == hash2);

    }

    {
        anya::unordered_map<int, int> hash1 = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
        anya::unordered_map<int, int> hash2;
        anya::deque<std::pair<int, int>> anya2(4, {6, 6});
        for (auto it = anya2.begin(); it != anya2.end(); ++it) {
            auto temp = *it;
            hash2.insert(std::move(temp));
        }
        EXPECT_TRUE(hash1 == hash2);

    }

    {
        anya::unordered_map<int, int> hash1 = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
        anya::unordered_map<int, int> hash2;
        anya::deque<std::pair<int, int>> anya2(4, {6, 6});
        hash2.insert(anya2.begin(), anya2.end());
        EXPECT_TRUE(hash1 == hash2);

    }

    {
        anya::unordered_map<int, int> hash1 = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
        anya::unordered_map<int, int> hash2;
        anya::deque<std::pair<int, int>> anya2(4, {6, 6});
        hash2.insert({std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)});
        EXPECT_TRUE(hash1 == hash2);
    }
}

TEST(UnMapTest, erase) {
    {
        anya::unordered_map<int, int> hash1 = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
        anya::unordered_map<int, int> hash2;
        hash1.erase(hash1.begin());
        EXPECT_TRUE(hash1 == hash2);
    }

    {
        anya::unordered_map<int, int> hash1 = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
        anya::unordered_map<int, int> hash2;
        hash1.erase(hash1.begin(), hash1.end());
        EXPECT_TRUE(hash1 == hash2);
    }

    {
        anya::unordered_map<int, int> hash1 = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
        anya::unordered_map<int, int> hash2;
        hash1.erase(6);
        EXPECT_TRUE(hash1 == hash2);
    }
}

TEST(UnMapTest, swap) {
    anya::unordered_map<int, int> hash1 = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
    anya::unordered_map<int, int> anya1;
    using std::swap;
    swap(hash1, anya1);
    anya::unordered_map<int, int> hash2 = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
    anya::unordered_map<int, int> anya2;
    hash2.swap(anya2);
    EXPECT_TRUE(hash1 == hash2);
    EXPECT_TRUE(anya1 == anya2);
}

TEST(UnMapTest, find) {
    anya::unordered_map<int, int> hash1 = {std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6), std::make_pair(6, 6)};
    anya::unordered_map<int, int> hash2;
    hash2[6] = 0;
    EXPECT_TRUE(hash1.count(6) == hash2.count(6));
    EXPECT_TRUE(hash1.contains(6) == hash2.contains(6));
    EXPECT_TRUE(hash1.find(6)->second != hash2.find(6)->second);
}

#endif //ANYA_STL_UNORDERED_MAP_TEST_HPP
