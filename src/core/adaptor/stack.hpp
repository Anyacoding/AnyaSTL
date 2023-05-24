//
// Created by Anya on 2023/5/24.
//

#ifndef ANYA_STL_STACK_HPP
#define ANYA_STL_STACK_HPP

#include "container/deque.hpp"
#include "container/list.hpp"
#include "container/vector.hpp"

namespace anya {

template<class T,
         class Container = anya::deque<T>>
class stack {
private:
    static_assert(std::is_same<T, typename Container::value_type>::value,
                  "value_type must be the same as the underlying container");
protected:
    Container c;

public:
    using container_type  = Container;
    using value_type      = typename Container::value_type;
    using size_type       = typename Container::size_type;
    using reference       = typename Container::reference;
    using const_reference = typename Container::const_reference;

#pragma region 构造 && 析构
public:
    stack() : c(Container()) {}

    explicit stack(const Container& cont) : c(cont) {}

    explicit stack(Container&& cont) : c(std::move(cont)) {}

    stack(const stack& other) : c(std::move(other.c)) {}

    template<class InputIt>
    stack(InputIt first, InputIt last) : c(first, last) {}

    ~stack() = default;
#pragma endregion


#pragma region 赋值
public:
    stack&
    operator=(const stack& other) { c = other.c; return *this; };

    stack&
    operator=(stack&& other) { c = std::move(other.c); return *this; }

#pragma endregion


#pragma region 元素访问
public:
    reference
    top() { return c.back(); }

    const_reference
    top() const { return c.back(); }

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
    push(const value_type& value) { c.push_back(value); }

    void
    push(value_type&& value) { c.push_back(std::move(value)); }

    template<class... Args>
    decltype(auto)
    emplace(Args&&... args) { c.emplace_back(std::forward<Args>(args)...); }

    void
    pop() { c.pop_back(); }

    void
    swap(stack& other) noexcept { using std::swap; swap(c, other.c); }

#pragma endregion

#pragma region 友元比较函数
public:
    bool friend
    operator==(const anya::stack<T, Container>& lhs,
               const anya::stack<T, Container>& rhs) {
        return lhs.size() == rhs.size() && anya::equal(lhs.c.begin(), lhs.c.end(), rhs.c.begin());
    }

    bool friend
    operator!=(const anya::stack<T, Container>& lhs,
               const anya::stack<T, Container>& rhs) {
        return !(lhs == rhs);
    };

    friend bool
    operator<(const anya::stack<T, Container>& lhs,
              const anya::stack<T, Container>& rhs) {
        // DONE: 将来替换成 anya::lexicographical_compare()
        return anya::lexicographical_compare(
            lhs.c.begin(), lhs.c.end(),
            rhs.c.begin(), rhs.c.end());
    }

    friend bool
    operator>(const anya::stack<T, Container>& lhs,
              const anya::stack<T, Container>& rhs) {
        return rhs < lhs;
    }

    friend bool
    operator<=(const anya::stack<T, Container>& lhs,
               const anya::stack<T, Container>& rhs) {
        return !(rhs < lhs);
    }

    friend bool
    operator>=(const anya::stack<T, Container>& lhs,
               const anya::stack<T, Container>& rhs) {
        return !(lhs < rhs);
    }

#pragma endregion

};

// 特化 anya::swap 算法
template<class T, class Container>
constexpr void
swap(anya::stack<T, Container>& lhs, anya::stack<T, Container>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

}

#endif //ANYA_STL_STACK_HPP
