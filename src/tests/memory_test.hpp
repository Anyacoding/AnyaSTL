//
// Created by Anya on 2023/3/7.
//

#include <memory>
#include <vector>
#include "allocator/memory.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


TEST(MemoryTest, construct) {
    anya::allocator<std::vector<int>> anya_alloc;
    auto anya_vec = anya_alloc.allocate(1);
    anya_alloc.construct(anya_vec, 10, 2);

    std::allocator<std::vector<int>> std_alloc;
    auto std_vec = std_alloc.allocate(1);
    std::construct_at(std_vec, 10, 2);
    EXPECT_THAT(*std_vec, *anya_vec);

    anya_alloc.destroy(anya_vec);
    anya_alloc.deallocate(anya_vec);
    std::destroy_at(std_vec);
    std_alloc.deallocate(std_vec, 1);
}

TEST(MemoryTest, address) {
    anya::allocator<std::shared_ptr<int>> anya_alloc;
    auto sp = anya_alloc.allocate(1);
    std::allocator<std::shared_ptr<int>> std_alloc;
    EXPECT_EQ(std::addressof(*sp), anya_alloc.address(*sp));
    anya_alloc.deallocate(sp);
}


TEST(MemoryTest, uninitialized_copy) {
    std::vector<int> base{ 1, 7, 1, 0, 2, 7, 3, 9, 2 };
    std::vector<int> anya(9, 0);
    anya::uninitialized_copy(base.begin(), base.end(), anya.begin());
    EXPECT_THAT(base, anya);
}


TEST(MemoryTest, uninitialized_copy_n) {
    std::vector<int> base{ 1, 7, 1, 0, 2, 7, 3, 9, 2 };
    std::vector<int> anya(9, 0);
    anya::uninitialized_copy_n(base.begin(), 5, anya.begin());
    EXPECT_FALSE(base == anya);
    anya::uninitialized_copy_n(base.begin(), 9, anya.begin());
    EXPECT_TRUE(base == anya);
}

TEST(MemoryTest, uninitialized_fill) {
    std::vector<int> base(110, 6);
    std::vector<int> anya(110, 0);
    anya::uninitialized_fill(anya.begin(), anya.end(), 6);
    EXPECT_TRUE(base == anya);
}

// 对于int默认初始不做任何事情
TEST(MemoryTest, uninitialized_fill_n) {
    std::vector<int> base(110, 6);
    std::vector<int> anya(110, 0);
    anya::uninitialized_fill_n(anya.begin(), anya.size(), 7);
    EXPECT_FALSE(base == anya);
    anya::uninitialized_fill_n(anya.begin(), anya.size(), 6);
    EXPECT_TRUE(base == anya);
}

TEST(MemoryTest, uninitialized_default_construct) {
    std::vector<int> base(110, 7);
    std::vector<int> anya(110, 7);
    anya::uninitialized_default_construct(anya.begin(), anya.end());
    std::uninitialized_default_construct(base.begin(), base.end());
    EXPECT_TRUE(base == anya);
}

TEST(MemoryTest, uninitialized_default_construct_n) {
    std::vector<int> base(110, 7);
    std::vector<int> anya(110, 7);
    anya::uninitialized_default_construct_n(anya.begin(), anya.size());
    std::uninitialized_default_construct_n(base.begin(), base.size());
    EXPECT_TRUE(base == anya);
}



TEST(MemoryTest, destroy) {
    struct Test {
        int a = 10;
        Test() = default;
        Test(int num): a(num) {}
        ~Test() { a = 0; }
    };
    std::vector<Test> base(110, 110);
    std::vector<Test> anya(110, 7);
    anya::destroy(anya.begin(), anya.end());
    std::destroy(base.begin(), base.end());
    for (int i = 0; i < base.size(); ++i) {
        EXPECT_TRUE(base[i].a == anya[i].a);
    }
}



