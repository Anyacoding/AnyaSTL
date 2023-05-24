#include "gtest/gtest.h"
#include "tests/memory_test.hpp"
#include "tests/iterator_test.hpp"
#include "tests/vector_test.hpp"
#include "tests/list_test.hpp"
#include "tests/deque_test.hpp"
#include "tests/stack_test.hpp"
#include <iterator>

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    std::string s;
    return RUN_ALL_TESTS();
}



