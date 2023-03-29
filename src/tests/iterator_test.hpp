//
// Created by DELL on 2023/3/8.
//

#include "iterator/iterator.hpp"
#include "iterator/iterator_concept.hpp"
#include "allocator/memory.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <iterator>
#include <list>

TEST(IterTest, address) {
    std::vector<int> anya(110, 0);
    // 这个要等到实现自己的 vector 时才能测试
    // EXPECT_TRUE(anya::distance(anya.begin(), anya.end()) == 110);
}

TEST(IterTest, normal_iterator) {
    std::list<int> ls{};
    auto it = ls.rbegin();
    std::vector<int> anya(110, 0);
    static_assert(anya::random_access_iterator<anya::normal_iterator<int*, std::vector<int>>>, "ok");
    static_assert(anya::random_access_iterator<anya::reverse_iterator<anya::normal_iterator<int*, std::vector<int>>>>, "ok");
}

TEST(IterTest, reverse_iterator) {
    static_assert(anya::random_access_iterator<anya::reverse_iterator<anya::normal_iterator<int*, std::vector<int>>>>, "ok");
    std::vector<int> temp{ 1, 2, 3, 4, 5 };
    anya::reverse_iterator<std::vector<int>::iterator> rit1(temp.end());
    for (int i = 4; i >= 0; --i, ++rit1) {
        EXPECT_TRUE(*rit1 == temp[i]);
    }
    int arr[6] = {1, 1, 4, 5, 1, 4};
    anya::reverse_iterator<int*> rit2(arr + 6);
    for (int i = 5; i >= 0; --i, ++rit2) {
        EXPECT_TRUE(*rit2 == arr[i]);
    }
}