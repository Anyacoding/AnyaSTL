//
// Created by Anya on 2023/5/31.
//

#ifndef ANYA_STL_HASHTABLE_TEST_HPP
#define ANYA_STL_HASHTABLE_TEST_HPP

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "container/built-in/hashtable.hpp"
#include "algorithm/algorithm.h"
#include <unordered_map>
#include <hashtable.h>

TEST(HashTableTest, construct) {
    {
        anya::hashtable<int, int> hash;
        EXPECT_TRUE(hash.empty());
        EXPECT_TRUE(hash.size() == 0);
    }

    {
        anya::hashtable<int, int> hash(60);
        EXPECT_TRUE(hash.empty());
        EXPECT_TRUE(hash.size() == 0);
    }

    {
        anya::hashtable<int, int> hash(60);
        anya::hashtable<int, int> hash2 = hash;
        EXPECT_TRUE(hash.empty());
        EXPECT_TRUE(hash.size() == 0);
        EXPECT_TRUE(hash2.empty());
        EXPECT_TRUE(hash2.size() == 0);
    }

    {
        anya::hashtable<int, int> hash(60);
        anya::hashtable<int, int> hash2 = std::move(hash);
        EXPECT_TRUE(hash.empty());
        EXPECT_TRUE(hash.size() == 0);
        EXPECT_TRUE(hash2.empty());
        EXPECT_TRUE(hash2.size() == 0);
    }
}

#endif //ANYA_STL_HASHTABLE_TEST_HPP
