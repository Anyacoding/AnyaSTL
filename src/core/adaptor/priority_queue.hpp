//
// Created by Anya on 2023/5/25.
//

#ifndef ANYA_STL_PRIORITY_QUEUE_H
#define ANYA_STL_PRIORITY_QUEUE_H

#include "container/vector.hpp"
#include "algorithm/algorithm.h"

namespace anya {

template<class T,
    class Container = anya::vector<T>,
    class Compare   = std::less<typename Container::value_type>>
class priority_queue {
private:
    static_assert(std::is_same<T, typename Container::value_type>::value,
                  "value_type must be the same as the underlying container");

public:
    Container c;
    Compare comp;

public:
    using container_type  = Container;
    using value_compare   = Compare;
    using value_type      = typename Container::value_type;
    using size_type       = typename Container::size_type;
    using reference       = typename Container::reference;
    using const_reference = typename Container::const_reference;


#pragma region 构造 && 析构
public:
    priority_queue()
        : priority_queue(Compare(), Container()) {}

    explicit priority_queue(const Compare& compare)
        : priority_queue(compare, Container()) {}

    priority_queue(const Compare& compare, const Container& cont)
        : comp(compare), c(cont) {
        anya::make_heap(c.begin(), c.end(), comp);
    }

    priority_queue(const Compare& compare, Container&& cont)
        : comp(compare), c(std::move(cont)) {
        anya::make_heap(c.begin(), c.end(), comp);
    }

    priority_queue(const priority_queue& other)
        : comp(other.comp), c(other.c) {}

    priority_queue(priority_queue&& other)
        : comp(std::move(other.comp)), c(std::move(other.c)) {}

    template<class InputIt>
    priority_queue(InputIt first, InputIt last, const Compare& compare = Compare())
        : priority_queue(compare, Container(first, last)) {}

    ~priority_queue() = default;
#pragma endregion

#pragma region 赋值
public:
    priority_queue&
    operator=(const priority_queue& other) = default;

    priority_queue&
    operator=(priority_queue&& other) noexcept = default;
#pragma endregion

#pragma region 元素访问
public:
    const_reference
    top() const { return c.front(); }
#pragma endregion

#pragma region 容量
public:
    [[nodiscard]]
    bool empty() const { return c.empty(); }

    size_type
    size() const { return c.size(); }
#pragma endregion

#pragma region 修改器
public:
    void
    push(const value_type& value) {
        c.push_back(value); anya::push_heap(c.begin(), c.end(), comp);
    }

    void
    push(value_type&& value) {
        c.push_back(std::move(value)); anya::push_heap(c.begin(), c.end(), comp);
    }

    template<class... Args>
    decltype(auto)
    emplace(Args&&... args) {
        c.emplace_back(std::forward<Args>(args)...), anya::push_heap(c.begin(), c.end(), comp);
    }

    void
    pop() { anya::pop_heap(c.begin(), c.end(), comp), c.pop_back(); }

    void
    swap(priority_queue& other) noexcept {
        using std::swap;
        swap(c, other.c);
        swap(comp, other.comp);
    }
#pragma endregion

#pragma region 友元比较函数
public:
    bool friend
    operator==(const anya::priority_queue<T, Container, Compare>& lhs,
               const anya::priority_queue<T, Container, Compare>& rhs) {
        return lhs.c == rhs.c;
    }
#pragma endregion

};


// 特化 anya::swap 算法
template<class T, class Container, class Compare>
constexpr void
swap(anya::priority_queue<T, Container, Compare>& lhs,
     anya::priority_queue<T, Container, Compare>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

}

#endif //ANYA_STL_PRIORITY_QUEUE_H
