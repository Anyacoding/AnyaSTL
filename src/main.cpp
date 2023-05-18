#include "gtest/gtest.h"
#include "tests/memory_test.hpp"
#include "tests/iterator_test.hpp"
#include "tests/vector_test.hpp"
#include "tests/list_test.hpp"
#include "tests/deque_test.hpp"
#include <iterator>

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



