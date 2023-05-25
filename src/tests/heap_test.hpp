//
// Created by Anya on 2023/5/25.
//

#ifndef ANYA_STL_HEAP_TEST_HPP
#define ANYA_STL_HEAP_TEST_HPP

#include "allocator/memory.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "algorithm/algorithm.h"
#include "container/vector.hpp"

TEST(HeapTest, is_heap) {
    anya::vector<int> v{ 3, 1, 4, 1, 5, 9 };
    if (!anya::is_heap(v.begin(), v.end())) {
        anya::make_heap(v.begin(), v.end());
    }
    EXPECT_TRUE(v == (anya::vector<int>{9, 5, 4, 1, 1, 3}));
}

TEST(HeapTest, is_heap_until) {
    anya::vector<int> v{ 3, 1, 4, 1, 5, 9 };
    anya::make_heap(v.begin(), v.end());
    v.push_back(2);
    v.push_back(6);
    auto heap_end = anya::is_heap_until(v.begin(), v.end());
    EXPECT_TRUE(v == (anya::vector<int>{9, 5, 4, 1, 1, 3, 2, 6}));
    EXPECT_TRUE(anya::vector<int>(v.begin(), heap_end) == (anya::vector<int>{9, 5, 4, 1, 1, 3, 2}));
}

TEST(HeapTest, make_heap) {
    anya::vector<int> v{ 3, 2, 4, 1, 5, 9 };
    anya::make_heap(v.begin(), v.end());
    EXPECT_TRUE(v == (anya::vector<int>{9, 5, 4, 1, 2, 3}));
    anya::pop_heap(v.begin(), v.end());
    EXPECT_TRUE(v == (anya::vector<int>{5, 3, 4, 1, 2, 9}));
    auto top = v.back();
    v.pop_back();
    EXPECT_TRUE(top == 9);
    EXPECT_TRUE(v == (anya::vector<int>{5, 3, 4, 1, 2}));

    anya::vector<int> v1 { 3, 2, 4, 1, 5, 9 };
    anya::make_heap(v1.begin(), v1.end(), std::greater<>{});
    EXPECT_TRUE(v1 == (anya::vector<int>{1, 2, 4, 3, 5, 9}));
    anya::pop_heap(v1.begin(), v1.end(), std::greater<>{});
    EXPECT_TRUE(v1 == (anya::vector<int>{2, 3, 4, 9, 5, 1}));
    auto top1 = v1.back();
    v1.pop_back();
    EXPECT_TRUE(top1 == 1);
    EXPECT_TRUE(v1 == (anya::vector<int>{2, 3, 4, 9, 5}));
}

TEST(HeapTest, push_heap) {
    anya::vector<int> v { 3, 1, 4, 1, 5, 9 };
    anya::make_heap(v.begin(), v.end());
    EXPECT_TRUE(v == (anya::vector<int>{9, 5, 4, 1, 1, 3}));
    v.push_back(6);
    EXPECT_TRUE(v == (anya::vector<int>{9, 5, 4, 1, 1, 3, 6}));
    anya::push_heap(v.begin(), v.end());
    EXPECT_TRUE(v == (anya::vector<int>{9, 5, 6, 1, 1, 3, 4}));
}

TEST(HeapTest, pop_heap) {
    anya::vector<int> v { 3, 1, 4, 1, 5, 9 };
    anya::make_heap(v.begin(), v.end());
    EXPECT_TRUE(v == (anya::vector<int>{9, 5, 4, 1, 1, 3}));
    anya::pop_heap(v.begin(), v.end());
    EXPECT_TRUE(v == (anya::vector<int>{5, 3, 4, 1, 1, 9}));
    v.pop_back();
    EXPECT_TRUE(v == (anya::vector<int>{5, 3, 4, 1, 1}));
}

TEST(HeapTest, sort_heap) {
    anya::vector<int> v { 3, 1, 4, 1, 5, 9 };
    anya::make_heap(v.begin(), v.end());
    EXPECT_TRUE(v == (anya::vector<int>{9, 5, 4, 1, 1, 3}));
    anya::sort_heap(v.begin(), v.end());
    EXPECT_TRUE(v == (anya::vector<int>{1, 1, 3, 4, 5, 9}));
}
#endif //ANYA_STL_HEAP_TEST_HPP
