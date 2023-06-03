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

TEST(HashTableTest, emplace) {
    {
        anya::hashtable<int, int> anya;
        std::unordered_multimap<int, int> std, temp;
        anya::vector<int> nums{1, 1, 4, 5, 1, 4};
        for (auto num : nums) anya.emplace_unique(num, num);
        for (auto num : nums) if (std.count(num) == 0) std.insert({num, num});
        for (auto& kv : anya) temp.insert(kv);
        EXPECT_TRUE(std == temp);
    }

    {
        anya::hashtable<int, int> anya;
        std::unordered_multimap<int, int> std, temp;
        anya::vector<int> nums{1, 1, 4, 5, 1, 4};
        for (auto num : nums) anya.emplace_multi(num, num);
        for (auto num : nums) std.insert({num, num});
        for (auto& kv : anya) temp.insert(kv);
        EXPECT_TRUE(std == temp);
    }
}

TEST(HashTableTest, erase) {
    {
        anya::hashtable<int, int> anya;
        std::unordered_multimap<int, int> std, temp;
        anya::vector<int> nums{1, 1, 4, 5, 1, 4};
        for (auto num : nums) anya.emplace_multi(num, num);
        for (auto num : nums) std.insert({num, num});
        anya.erase(anya.begin(), anya.end());
        std.erase(std.begin(), std.end());
        for (auto& kv : anya) temp.insert(kv);
        EXPECT_TRUE(std == temp);
    }

    {
        anya::hashtable<int, int> anya;
        std::unordered_multimap<int, int> std, temp;
        anya::vector<int> nums{1, 1, 4, 5, 1, 4};
        for (auto num : nums) anya.emplace_multi(num, num);
        for (auto num : nums) std.insert({num, num});
        anya.erase(1);
        std.erase(1);
        for (auto& kv : anya) temp.insert(kv);
        EXPECT_TRUE(std == temp);
    }

    {
        anya::hashtable<int, int> anya;
        std::unordered_multimap<int, int> std, temp;
        anya::vector<int> nums{1, 1, 4, 5, 1, 4};
        for (auto num : nums) anya.emplace_multi(num, num);
        for (auto num : nums) std.insert({num, num});
        anya.erase(996);
        std.erase(996);
        for (auto& kv : anya) temp.insert(kv);
        EXPECT_TRUE(std == temp);
    }
}

// DONE: 有bug，但是没查出来
TEST(HashTableTest, swap) {
    anya::hashtable<std::string, int> anya1;
    anya::hashtable<std::string, int> anya2;
    std::unordered_multimap<std::string, int> std;
    anya::vector<int> nums{1, 1, 4, 5, 1, 4};
    for (auto num : nums) anya1.emplace_multi("anya", num);
    for (auto num : nums) anya2.emplace_multi("mnzn", num);
//    for (auto num : nums) std.insert({"mnzn", num});
//    for (auto& kv : anya2) std::cout << kv.first << " " << kv.second << std::endl;
//    std::cout << std::endl;
    auto temp1 = anya1;
    auto temp2 = anya2;
    anya1.swap(anya2);
//    for (auto& kv : temp2) std::cout << kv.first << " " << kv.second << std::endl;
//    std::cout << std::endl;
//    auto it = anya1.find_range_by_key("mnzn");
//    for (; it.first != it.second; it.first = it.first->next) {
//        std::cout << it.first->value.first << " " << it.first->value.second << std::endl;
//    }
    EXPECT_TRUE(anya1 == temp2);
    EXPECT_TRUE(anya2 == temp1);
}

#endif //ANYA_STL_HASHTABLE_TEST_HPP
