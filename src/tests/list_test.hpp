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

TEST(ListTest, iterator) {
//    std::list<int> list;
//    anya::list<int> anya;
//    auto crend = anya.crend();
//    auto rend = anya.rend();
//    typename anya::list<int>::reverse_iterator temp1 = crend;
//    typename anya::list<int>::const_reverse_iterator temp2 = rend;

//    std::vector<int> list;
//    anya::vector<int> anya;
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
    anya::list<int> anya;
    int a = 9;
    anya.insert(anya.begin(), a);
    EXPECT_TRUE(*anya.begin() == a);
    anya.insert(anya.begin(), 10);
    EXPECT_TRUE(*anya.begin() == 10);

    // TODO: 待实现构造函数后一起测试insert函数集
}


#endif //ANYA_STL_LIST_TEST_HPP
