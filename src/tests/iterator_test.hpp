//
// Created by DELL on 2023/3/8.
//

#include "iterator/iterator.hpp"
#include "iterator/iterator_concept.hpp"
#include "allocator/memory.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <iterator>

TEST(IterTest, address) {
    std::vector<int> anya(110, 0);
    // 这个要等到实现自己的 vector 时才能测试
    // EXPECT_TRUE(anya::distance(anya.begin(), anya.end()) == 110);
}

TEST(IterTest, normal_iterator) {
    std::vector<int> anya(110, 0);
    static_assert(anya::random_access_iterator<anya::normal_iterator<int*, std::vector<int>>>, "ok");
}