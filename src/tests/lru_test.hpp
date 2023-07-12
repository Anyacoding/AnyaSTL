//
// Created by Anya on 2023/7/12.
//

#ifndef ANYA_STL_LRU_TEST_HPP
#define ANYA_STL_LRU_TEST_HPP

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "expand/lru.hpp"

TEST(LRUTest, all) {
    anya::lru_cache<std::string, int> lru;
    lru.set_max_size(2);
    lru.push("anya", 0);
    lru.push("mnzn", 1);
    lru.push("neko", 2);
    EXPECT_FALSE(lru.contains("anya"));
    EXPECT_TRUE(lru.get_or_default("mnzn") == 1);
    EXPECT_TRUE(lru.get_or_default("neko") == 2);
    lru.push("anya", 0);
    EXPECT_FALSE(lru.contains("mnzn"));
    lru.erase("neko");
    EXPECT_FALSE(lru.contains("neko"));
    EXPECT_TRUE(lru.contains("anya"));
}

#endif //ANYA_STL_LRU_TEST_HPP
