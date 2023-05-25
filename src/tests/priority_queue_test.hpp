//
// Created by Anya on 2023/5/25.
//

#ifndef ANYA_STL_PRIORITY_QUEUE_TEST_HPP
#define ANYA_STL_PRIORITY_QUEUE_TEST_HPP

#include "allocator/memory.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "adaptor/priority_queue.hpp"
#include <queue>

TEST(PQueueTest, construct) {
    {
        anya::priority_queue<int> pq1;
        anya::priority_queue<int> pq2;
        EXPECT_TRUE(pq1 == pq2);
    }

    {
        anya::vector<int> anya1{1, 8, 5, 6, 3, 4, 0, 9, 7, 2};
        anya::priority_queue<int> pq1(std::less<int>(), anya1);
        anya::priority_queue<int> pq2(std::less<int>(), std::move(anya1));
        EXPECT_TRUE(pq1 == pq2);
    }

    {
        anya::vector<int> anya1{1, 8, 5, 6, 3, 4, 0, 9, 7, 2};
        anya::priority_queue<int> pq1(std::less<int>(), anya1);
        anya::priority_queue<int> pq2 = pq1;
        EXPECT_TRUE(pq1 == pq2);
    }

    {
        anya::vector<int> anya1{1, 8, 5, 6, 3, 4, 0, 9, 7, 2};
        anya::priority_queue<int> pq1(std::less<int>(), anya1);
        anya::priority_queue<int> pq2 = std::move(pq1);
        EXPECT_TRUE(pq1 != pq2);
    }
}

TEST(PQueueTest, assign) {
    {
        anya::vector<int> anya1{1, 8, 5, 6, 3, 4, 0, 9, 7, 2};
        anya::priority_queue<int> pq1(std::less<int>(), anya1);
        anya::priority_queue<int> pq2;
        pq2 = pq1;
        EXPECT_TRUE(pq1 == pq2);
    }

    {
        anya::vector<int> anya1{1, 8, 5, 6, 3, 4, 0, 9, 7, 2};
        anya::priority_queue<int> pq1(std::less<int>(), anya1);
        anya::priority_queue<int> pq2;
        pq2 = std::move(pq1);
        EXPECT_TRUE(pq1 != pq2);
    }
}

TEST(PQueueTest, top) {
    anya::vector<int> anya1{1, 8, 5, 6, 3, 4, 0, 9, 7, 2};
    anya::priority_queue<int> pq1(std::less<int>(), anya1);
    anya::vector<int> anya2{1, 8, 5, 9, 7, 2};
    anya::priority_queue<int> pq2(std::less<int>(), anya1);
    EXPECT_TRUE(pq1.top() == pq2.top());
}

TEST(PQueueTest, capacity) {
    anya::priority_queue<int> pq1;
    anya::priority_queue<int> pq2;
    EXPECT_TRUE(pq1.empty() == pq2.empty());
    pq1.push(3), pq2.push(4);
    EXPECT_TRUE(pq1.size() == pq2.size());
}

TEST(PQueueTest, push_and_pop) {
    anya::vector<int> anya1{1, 8, 5, 6, 3, 4, 0, 9, 7, 2};
    anya::priority_queue<int> q1;
    for (int n : anya1) q1.push(n);
    anya::priority_queue<int> q2(std::less<int>(), {9, 8, 7, 6, 5, 4, 3, 2, 1, 0});
    for (; !q2.empty(); q2.pop(), q1.pop()) EXPECT_TRUE(q1.top() == q2.top());

    anya::priority_queue<int, anya::vector<int>, std::greater<int>>
        minq1(anya1.begin(), anya1.end());
    anya::priority_queue<int, anya::vector<int>, std::greater<int>>
        minq2(std::greater<int>(), {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    for (; !minq1.empty(); minq1.pop(), minq2.pop()) EXPECT_TRUE(minq1.top() == minq2.top());

    auto cmp = [](int left, int right) { return (left ^ 1) < (right ^ 1); };
    anya::priority_queue<int, anya::vector<int>, decltype(cmp)> q5(cmp);
    for (int n : anya1) q5.push(n);
    anya::priority_queue<int, anya::vector<int>, decltype(cmp)>
        q6(cmp, {8, 9, 6, 7, 4, 5, 2, 3, 0, 1});
    for (; !q5.empty(); q6.pop(), q5.pop()) EXPECT_TRUE(q6.top() == q5.top());
}

TEST(PQueueTest, swap) {
    anya::vector<int> anya1{1, 8, 5, 6, 3, 4, 0, 9, 7, 2};
    anya::priority_queue<int> q1;
    for (int n : anya1) q1.push(n);
    anya::priority_queue<int> q2(std::less<int>(), {9, 8, 7, 6, 5, 4, 3, 2, 1, 0});
    q1.swap(q2);
    q1 = anya::priority_queue<int>(q2);
    EXPECT_TRUE(q1 == q2);
}


#endif //ANYA_STL_PRIORITY_QUEUE_TEST_HPP
