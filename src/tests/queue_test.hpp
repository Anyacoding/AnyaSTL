//
// Created by Anya on 2023/5/24.
//

#ifndef ANYA_STL_QUEUE_TEST_HPP
#define ANYA_STL_QUEUE_TEST_HPP

#include "allocator/memory.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "adaptor/queue.hpp"
#include <queue>

TEST(QueueTest, construct) {
    {
        anya::deque<int> anya1(7, 6);
        anya::queue<int> q1(anya1);
        anya::queue<int> q2(anya1.begin(), anya1.end());
        EXPECT_TRUE(q1 == q2);
    }

    {
        anya::list<int> anya1(7, 6);
        anya::queue<int, anya::list<int>> q1(anya1);
        anya::queue<int, anya::list<int>> q2(anya1.begin(), anya1.end());
        EXPECT_TRUE(q1 == q2);
    }

    {
        anya::list<int> anya1(7, 6);
        anya::queue<int, anya::list<int>> q1(anya1);
        anya::queue<int, anya::list<int>> q2(q1);
        EXPECT_TRUE(q1 == q2);
    }

    {
        anya::list<int> anya1(7, 6);
        anya::queue<int, anya::list<int>> q1(anya1);
        anya::queue<int, anya::list<int>> q2(std::move(q1));
        EXPECT_TRUE(q1 != q2);
    }
}

TEST(QueueTest, assign) {
    {
        anya::deque<int> anya1(7, 6);
        anya::queue<int> q1(anya1);
        anya::queue<int> q2;
        q2 = q1;
        EXPECT_TRUE(q1 == q2);
    }

    {
        anya::deque<int> anya1(7, 6);
        anya::queue<int> q1(anya1);
        anya::queue<int> q2;
        q2 = std::move(q1);
        EXPECT_TRUE(q1 != q2);
    }
}

TEST(QueueTest, front_and_back) {
    anya::deque<int> anya1(7, 6);
    anya::deque<int> anya2{6, 2, 3, 4, 5, 6};
    anya::queue<int> q1(anya1);
    anya::queue<int> q2(anya2);
    EXPECT_TRUE(q1.front() == q2.front());
    EXPECT_TRUE(q1.back() == q2.back());
}

TEST(QueueTest, capacity) {
    anya::queue<int> s1;
    anya::queue<int> s2;
    EXPECT_TRUE(s1.empty() == s2.empty());
    s1.push(3), s2.push(4);
    EXPECT_TRUE(s1.size() == s2.size());
}

TEST(QueueTest, push_and_pop) {
    anya::queue<int> s1;
    anya::queue<int> s2;
    anya::deque<int> anya1(7, 6);
    for (auto num : anya1) s1.push(num), s2.push(std::move(num));
    EXPECT_TRUE(s1 == s2);
    while (!s1.empty() || !s2.empty()) s1.pop(), s2.pop();
    EXPECT_TRUE(s1 == s2);
    for (auto num : anya1) s1.emplace(num), s2.emplace(std::move(num));
    EXPECT_TRUE(s1 == s2);
}

TEST(QueueTest, swap) {
    anya::deque<int> anya1(7, 6);
    anya::deque<int> anya2{1, 2, 3, 4, 5, 6};
    anya::queue<int> s1(anya1);
    anya::queue<int> s2(anya2);
    s1.swap(s2);
    s1 = anya::queue<int>(anya1);
    EXPECT_TRUE(s1 == s2);
}

TEST(QueueTest, compare) {
    anya::deque<std::string> _mnzn {"m", "n", "z", "n"};
    anya::deque<std::string> _anya {"a", "n", "y", "a"};
    anya::queue<std::string> mnzn(_mnzn);
    anya::queue<std::string> anya(_anya);
    EXPECT_TRUE(anya < mnzn);
    EXPECT_TRUE(anya <= mnzn);
    EXPECT_FALSE(anya > mnzn);
    EXPECT_FALSE(anya >= mnzn);
    EXPECT_TRUE(anya != mnzn);
    std::deque<std::string> _std_mnzn {"m", "n", "z", "n"};
    std::deque<std::string> _std_anya {"a", "n", "y", "a"};
    std::queue<std::string> std_mnzn(_std_mnzn);
    std::queue<std::string> std_anya(_std_anya);
    EXPECT_TRUE(std_anya < std_mnzn);
    EXPECT_TRUE(std_anya <= std_mnzn);
    EXPECT_FALSE(std_anya > std_mnzn);
    EXPECT_FALSE(std_anya >= std_mnzn);
    EXPECT_TRUE(std_anya != std_mnzn);
}


#endif //ANYA_STL_QUEUE_TEST_HPP
