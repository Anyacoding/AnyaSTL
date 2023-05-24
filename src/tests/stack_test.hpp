//
// Created by Anya on 2023/5/24.
//

#ifndef ANYA_STL_STACK_TEST_HPP
#define ANYA_STL_STACK_TEST_HPP

#include "allocator/memory.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "adaptor/stack.hpp"
#include <stack>

TEST(StackTest, construct) {
    {
        anya::deque<int> anya1(7, 6);
        anya::stack<int> s1(anya1);
        anya::stack<int> s2(anya1.begin(), anya1.end());
        EXPECT_TRUE(s1 == s2);
    }

    {
        anya::vector<int> anya1(7, 6);
        anya::stack<int, anya::vector<int>> s1(anya1);
        anya::stack<int, anya::vector<int>> s2(anya1.begin(), anya1.end());
        EXPECT_TRUE(s1 == s2);
    }

    {
        anya::list<int> anya1(7, 6);
        anya::stack<int, anya::list<int>> s1(anya1);
        anya::stack<int, anya::list<int>> s2(anya1.begin(), anya1.end());
        EXPECT_TRUE(s1 == s2);
    }
}

TEST(StackTest, assign) {
    {
        anya::deque<int> anya1(7, 6);
        anya::stack<int> s1(anya1);
        anya::stack<int> s2;
        s2 = s1;
        EXPECT_TRUE(s1 == s2);
    }

    {
        anya::deque<int> anya1(7, 6);
        anya::stack<int> s1(anya1);
        anya::stack<int> s2;
        s2 = std::move(s1);
        EXPECT_TRUE(s1 != s2);
    }
}

TEST(StackTest, top) {
    anya::deque<int> anya1(7, 6);
    anya::deque<int> anya2{1, 2, 3, 4, 5, 6};
    anya::stack<int> s1(anya1);
    anya::stack<int> s2(anya2);
    EXPECT_TRUE(s1.top() == s2.top());
}

TEST(StackTest, capacity) {
    anya::stack<int> s1;
    anya::stack<int> s2;
    EXPECT_TRUE(s1.empty() == s2.empty());
    s1.push(3), s2.push(4);
    EXPECT_TRUE(s1.size() == s2.size());
}

TEST(StackTest, push_and_pop) {
    anya::stack<int> s1;
    anya::stack<int> s2;
    anya::deque<int> anya1(7, 6);
    for (auto num : anya1) s1.push(num), s2.push(std::move(num));
    EXPECT_TRUE(s1 == s2);
    while (!s1.empty() || !s2.empty()) s1.pop(), s2.pop();
    EXPECT_TRUE(s1 == s2);
}

TEST(StackTest, swap) {
    anya::deque<int> anya1(7, 6);
    anya::deque<int> anya2{1, 2, 3, 4, 5, 6};
    anya::stack<int> s1(anya1);
    anya::stack<int> s2(anya2);
    s1.swap(s2);
    s1 = anya::stack<int>(anya1);
    EXPECT_TRUE(s1 == s2);
}

TEST(StackTest, compare) {
    anya::deque<std::string> _mnzn {"m", "n", "z", "n"};
    anya::deque<std::string> _anya {"a", "n", "y", "a"};
    anya::stack<std::string> mnzn(_mnzn);
    anya::stack<std::string> anya(_anya);
    EXPECT_TRUE(anya < mnzn);
    EXPECT_TRUE(anya <= mnzn);
    EXPECT_FALSE(anya > mnzn);
    EXPECT_FALSE(anya >= mnzn);
    EXPECT_TRUE(anya != mnzn);
    std::deque<std::string> _std_mnzn {"m", "n", "z", "n"};
    std::deque<std::string> _std_anya {"a", "n", "y", "a"};
    std::stack<std::string> std_mnzn(_std_mnzn);
    std::stack<std::string> std_anya(_std_anya);
    EXPECT_TRUE(std_anya < std_mnzn);
    EXPECT_TRUE(std_anya <= std_mnzn);
    EXPECT_FALSE(std_anya > std_mnzn);
    EXPECT_FALSE(std_anya >= std_mnzn);
    EXPECT_TRUE(std_anya != std_mnzn);
}


#endif //ANYA_STL_STACK_TEST_HPP
