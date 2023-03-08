//
// Created by DELL on 2023/3/8.
//

#include "iterator/iterator.hpp"
#include "allocator/memory.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

TEST(IterTest, address) {
    std::vector<int> anya(110, 0);
    // 这个要等到实现自己的 vector 时才能测试
    // EXPECT_TRUE(anya::distance(anya.begin(), anya.end()) == 110);
}