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

TEST(HashTableTest, iterator) {
//    anya::hashtable<int, int> anya;
//    auto anya_cbegin = anya.cbegin();
//    auto anya_begin = anya.begin();
//    typename anya::hashtable<int, int>::iterator anya_temp1 = anya_cbegin;
//    typename anya::hashtable<int, int>::const_iterator anya_temp2 = anya_begin;

//    anya::hashtable<int, int> anya;
//    auto anya_cbegin = anya.cbegin();
//    auto anya_begin = anya.begin();
//    typename anya::hashtable<int, int>::iterator anya_temp1 = anya_begin;
//    typename anya::hashtable<int, int>::const_iterator anya_temp2 = anya_cbegin;

//    anya::hashtable<int, int> anya;
//    auto anya_cend = anya.cend();
//    auto anya_end = anya.end();
//    typename anya::hashtable<int, int>::iterator anya_temp1 = anya_cend;
//    typename anya::hashtable<int, int>::const_iterator anya_temp2 = anya_end;

//    anya::hashtable<int, int> anya;
//    auto anya_cend = anya.cend();
//    auto anya_end = anya.end();
//    typename anya::hashtable<int, int>::iterator anya_temp1 = anya_end;
//    typename anya::hashtable<int, int>::const_iterator anya_temp2 = anya_cend;
}

#endif //ANYA_STL_HASHTABLE_TEST_HPP
