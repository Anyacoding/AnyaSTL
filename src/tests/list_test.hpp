//
// Created by Anya on 2023/4/15.
//

#ifndef ANYA_STL_LIST_TEST_HPP
#define ANYA_STL_LIST_TEST_HPP

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "container/list.hpp"
#include "container/vector.hpp"
#include <list>

TEST(ListTest, construct) {
    {
        anya::list<int> anya(1, 6);
        EXPECT_TRUE(*anya.begin() == *anya.rbegin());
    }

    {
        anya::list<int> anya1(6, 0);
        anya::list<int> anya2(6);
        EXPECT_TRUE(anya1 == anya2);
    }

    {
        anya::list<int> anya1(6, 6);
        anya::list<int> anya2(anya1.begin(), anya1.end());
        EXPECT_TRUE(anya1 == anya2);
    }

    {
        anya::list<int> anya1(5, 6);
        anya::list<int> anya2 = anya1;
        EXPECT_TRUE(anya1 == anya2);
    }

    {
        anya::list<int> anya1(7, 6);
        anya::list<int> anya2 = std::move(anya1);
        EXPECT_FALSE(anya1 == anya2);
    }

    {
        anya::list<int> anya1(4, 6);
        anya::list<int> anya2 = {6, 6, 6, 6};
        EXPECT_TRUE(anya1 == anya2);
    }
}

TEST(ListTest, iterator) {
//    std::list<int> list;
//    anya::list<int> anya;
//    auto crend = anya.crend();
//    auto rend = anya.rend();
//    typename anya::list<int>::reverse_iterator temp1 = crend;
//    typename anya::list<int>::const_reverse_iterator temp2 = rend;


//    auto crbegin = anya.crbegin();
//    auto rbegin = anya.rbegin();
//    typename anya::vector<int>::reverse_iterator temp1 = crbegin;
//    typename anya::vector<int>::const_reverse_iterator temp2 = rbegin;
//    auto cbegin = list.cbegin();
//    auto begin = list.begin();
//    typename std::list<int>::iterator temp1 = cbegin;
//    typename std::list<int>::const_iterator temp2 = begin;
}

TEST(ListTest, insert) {
    {
        anya::list<int> anya;
        int a = 9;
        anya.insert(anya.begin(), a);
        EXPECT_TRUE(*anya.begin() == a);
        anya.insert(anya.begin(), 10);
        EXPECT_TRUE(*anya.begin() == 10);
    }

    {
        anya::list<int> anya1(6, 6);
        anya::list<int> anya2;
        anya2.insert(anya2.begin(), 6, 6);
        EXPECT_TRUE(anya1 == anya2);
    }

    {
        anya::list<int> anya1(6, 6);
        anya::list<int> anya2;
        anya2.insert(anya2.begin(), anya1.begin(), anya1.end());
        EXPECT_TRUE(anya1 == anya2);
    }

    {
        anya::list<int> anya1{1, 1, 4, 5, 1, 4};
        anya::list<int> anya2;
        anya2.insert(anya2.begin(), {1, 1, 4, 5, 1, 4});
        EXPECT_TRUE(anya1 == anya2);
    }

    {
        anya::list<int> anya1{1, 1, 4, 5, 1, 4};
        anya::list<int> anya2;
        anya1.clear();
        EXPECT_TRUE(anya1 == anya2);
    }

    {
        anya::list<int> anya1{1, 1, 4, 5, 1, 4};
        anya::list<int> anya2;
        anya2.emplace_back(1);
        anya2.emplace_back(1);
        anya2.emplace_back(4);
        anya2.emplace_back(5);
        anya2.emplace_back(1);
        anya2.emplace_back(4);
        EXPECT_TRUE(anya1 == anya2);
    }

    {
        anya::list<int> anya1{1, 1, 4, 5, 1, 4};
        anya::list<int> anya2;
        anya2.emplace_front(4);
        anya2.emplace_front(1);
        anya2.emplace_front(5);
        anya2.emplace_front(4);
        anya2.emplace_front(1);
        anya2.emplace_front(1);
        EXPECT_TRUE(anya1 == anya2);
    }

    {
        anya::list<int> anya1{1, 1, 4, 5, 1, 4};
        anya::list<int> anya2;
        anya2.push_back(1);
        anya2.push_back(1);
        anya2.push_back(4);
        anya2.push_back(5);
        anya2.push_back(1);
        anya2.push_back(4);
        EXPECT_TRUE(anya1 == anya2);
    }

    {
        anya::list<int> anya1{1, 1, 4, 5, 1, 4};
        anya::list<int> anya2;
        anya2.push_front(4);
        anya2.push_front(1);
        anya2.push_front(5);
        anya2.push_front(4);
        anya2.push_front(1);
        anya2.push_front(1);
        EXPECT_TRUE(anya1 == anya2);
    }

    {
        anya::list<int> anya1{1, 1, 4, 5, 1, 4};
        anya::list<int> anya2{1, 4, 5, 1, 4};
        anya1.pop_front();
        EXPECT_TRUE(anya1 == anya2);
        anya1.pop_front();
        anya2.pop_front();
        EXPECT_TRUE(anya1 == anya2);
    }

    {
        anya::list<int> anya1{1, 1, 4, 5, 1, 4};
        anya::list<int> anya2{1, 1, 4, 5, 1};
        anya1.pop_back();
        EXPECT_TRUE(anya1 == anya2);
        anya1.pop_back();
        anya2.pop_back();
        EXPECT_TRUE(anya1 == anya2);
    }
}

TEST(ListTest, capacity) {
    anya::list<int> anya1{1, 1, 4, 5, 1, 4};
    EXPECT_TRUE(anya1.size() == 6);
    anya::list<int> anya2;
    EXPECT_TRUE(anya2.empty());
    EXPECT_TRUE(anya1.max_size() == anya1.max_size());

}

TEST(ListTest, access) {
    anya::list<int> anya1{1, 1, 4, 5, 1, 4};
    EXPECT_TRUE(anya1.front() == 1);
    EXPECT_TRUE(anya1.back() == 4);
    const anya::list<int> anya3{2, 0, 0, 1, 4, 14};
    EXPECT_TRUE(anya3.front() == 2);
    EXPECT_TRUE(anya3.back() == 14);
}

TEST(ListTest, assign) {
    {
        anya::list<int> anya1{1, 1, 4, 5, 1, 4};
        anya::list<int> anya2;
        anya2 = anya1;
        EXPECT_TRUE(anya2 == anya1);
        anya2 = std::move(anya1);
        EXPECT_TRUE(anya2 != anya1);
        anya1 = {1, 1, 4, 5, 1, 4};
        EXPECT_TRUE(anya2 == anya1);
    }

    {
        anya::list<int> anya1{1, 1, 4, 5, 1, 4};
        anya::list<int> anya2;
        anya2.assign(6, 6);
        EXPECT_TRUE(anya2 != anya1);
        EXPECT_TRUE(anya2 == (anya::list<int>{6, 6, 6, 6, 6, 6}));
        anya1.assign(anya2.begin(), anya2.end());
        EXPECT_TRUE(anya2 == anya1);
        anya1.assign({1, 1, 4, 5, 1, 4});
        anya2.assign({1, 1, 4, 5, 1, 4});
        EXPECT_TRUE(anya2 == anya1);
    }
}

TEST(ListTest, erase) {
    anya::list<int> anya1{1, 1, 4, 5, 1, 4};
    anya::list<int> anya2;
    anya1.erase(anya1.begin(), anya1.end());
    EXPECT_TRUE(anya2 == anya1);
    anya2 = {1, 4, 5, 1, 4};
    anya1 = {1, 1, 4, 5, 1, 4};
    anya1.erase(anya1.begin());
    EXPECT_TRUE(anya2 == anya1);
    anya1.erase(anya1.begin());
    anya2.erase(anya2.begin());
    EXPECT_TRUE(anya2 == anya1);
}

TEST(ListTest, resize) {
    anya::list<int> anya1{1, 1, 4, 5, 1, 4, 0, 0, 0};
    anya::list<int> anya2{1, 1, 4, 5, 1, 4};
    anya2.resize(anya2.size() + 3);
    EXPECT_TRUE(anya2 == anya1);
    anya1.resize(anya1.size() + 3, 6);
    anya2.resize(anya2.size() + 3, 6);
    EXPECT_TRUE(anya2 == anya1);
}

TEST(ListTest, swap) {
    anya::list<int> anya1{1, 1, 4, 5, 1, 4, 0, 0, 0};
    anya::list<int> anya2{1, 1, 4, 5, 1, 4};
    anya::list<int> anya3{1, 1, 4, 5, 1, 4, 0, 0, 0};
    EXPECT_TRUE(anya2 != anya1);
    anya2.swap(anya3);
    EXPECT_TRUE(anya2 == anya1);
}

TEST(ListTest, merge) {
    {
        anya::list<int> anya1{0, 0, 0, 1, 1, 4, 5, 1, 4};
        anya::list<int> anya2{1, 1, 4, 5, 1, 4};
        anya::list<int> anya3{0, 0, 0};
        anya2.merge(anya3);
        EXPECT_TRUE(anya2 == anya1);
    }

    {
        anya::list<int> anya1{1, 1, 4, 5, 1, 4, 0, 0, 0};
        anya::list<int> anya2{1, 1, 4, 5, 1, 4};
        anya::list<int> anya3{0, 0, 0};

        anya2.merge(anya3, std::greater<>());
        EXPECT_TRUE(anya2 == anya1);
    }
}

TEST(ListTest, splice) {
    {
        anya::list<int> anya1{1, 2, 3, 4, 5};
        anya::list<int> anya2{10, 20, 30, 40, 50};

        auto it = anya1.begin();
        anya::advance(it, 2);

        anya1.splice(it, anya2);
        EXPECT_TRUE(anya2 == anya::list<int>());
        EXPECT_TRUE(anya1 == (anya::list<int>{1, 2, 10, 20, 30, 40, 50, 3, 4, 5}));

        anya2.splice(anya2.begin(), anya1, it, anya1.end());
        EXPECT_TRUE(anya2 == (anya::list<int>{3, 4, 5}));
        EXPECT_TRUE(anya1 == (anya::list<int>{1, 2, 10, 20, 30, 40, 50}));
    }

    {
        anya::list<int> anya1{1, 2, 3, 4, 5};
        anya::list<int> anya2{10, 20, 30, 40, 50};

        auto it = anya1.begin();
        anya::advance(it, 2);

        anya1.splice(it, anya2);
        EXPECT_TRUE(anya2 == anya::list<int>());
        EXPECT_TRUE(anya1 == (anya::list<int>{1, 2, 10, 20, 30, 40, 50, 3, 4, 5}));

        anya2.splice(anya2.begin(), anya1, it);
        EXPECT_TRUE(anya2 == (anya::list<int>{3, 4, 5}));
        EXPECT_TRUE(anya1 == (anya::list<int>{1, 2, 10, 20, 30, 40, 50}));
    }
}

TEST(ListTest, remove) {
    anya::list<int> anya = { 1,100,2,3,10,1,11,-1,12 };
    auto ret = anya.remove(1);
    EXPECT_TRUE(ret == 2);
    ret = anya.remove_if([](int n) { return n > 10; });
    EXPECT_TRUE(ret == 3);
    EXPECT_TRUE(anya == (anya::list<int>{2, 3, 10, -1}));
}

TEST(ListTest, reverse) {
    anya::list<int> anya{10, 20, 30, 40, 50};
    anya.reverse();
    EXPECT_TRUE(anya == (anya::list<int>{50, 40, 30, 20, 10}));
}

TEST(ListTest, unique) {
    {
        anya::list<int> anya{ 1, 2, 2, 3, 3, 2, 1, 1, 2 };
        anya.unique();
        EXPECT_TRUE(anya == (anya::list<int>{ 1, 2, 3, 2, 1, 2 }));

        anya = { 1, 2, 12, 23, 3, 2, 51, 1, 2 };
        anya.unique([mod = 10](int x, int y) { return (x % mod) == (y % mod); });

        EXPECT_TRUE(anya == (anya::list<int>{ 1, 2, 23, 2, 51, 2 }));
    }

    {
        anya::list<int> anya{ 1, 2, 2, 2, 3, 3, 3, 1, 1 };
        anya.unique();
        std::list<int> std{ 1, 2, 2, 2, 3, 3, 3, 1, 1 };
        std.unique();

        auto anya_it = anya.begin();
        auto std_it = std.begin();
        while (anya_it != anya.end()) {
            EXPECT_TRUE(*anya_it == *std_it);
            ++anya_it;
            ++std_it;
        }
    }
}

TEST(ListTest, sort) {
    anya::list<int> anya{8, 7, 5, 9, 0, 1, 3, 2, 6, 4};
    anya.sort();
    EXPECT_TRUE(anya == (anya::list<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
    anya.sort(std::greater<>());
    EXPECT_TRUE(anya == (anya::list<int>{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
}

#endif //ANYA_STL_LIST_TEST_HPP
