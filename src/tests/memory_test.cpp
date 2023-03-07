//
// Created by Anya on 2023/3/7.
//

#include <memory>
#include <vector>
#include "allocator/memory.hpp"
#include "iterator/iterator.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace anya {

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


}

