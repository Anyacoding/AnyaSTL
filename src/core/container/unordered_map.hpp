//
// Created by Anya on 2023/6/4.
//

#ifndef ANYA_STL_UNORDERED_MAP_HPP
#define ANYA_STL_UNORDERED_MAP_HPP

#include "container/built-in/hashtable.hpp"

namespace anya {

template<
    class Key,
    class T,
    class Hash      = std::hash<Key>,
    class KeyEqual  = std::equal_to<Key>,
    class Allocator = anya::allocator<std::pair<const Key, T>>>
class unordered_map {
private:
    using base_map = hashtable<Key, T, Hash, KeyEqual, Allocator>;

public:
    using key_type        = Key;
    using mapped_type     = T;
    using value_type      = std::pair<const Key, T>;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using hasher          = Hash;
    using key_equal       = KeyEqual;
    using allocator_type  = anya::allocator<std::pair<const Key, T>>;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using iterator        = typename base_map::iterator;
    using const_iterator  = typename base_map::const_iterator;

private:
    base_map table;
    constexpr static size_t default_size = 11;

#pragma region 构造 && 析构
public:
    unordered_map() = default;

    explicit unordered_map(size_type bucket_count,
                           const hasher& hash = hasher(),
                           const key_equal& equal = key_equal())
        : table(bucket_count, hash, equal)
    {}

    template<class InputIt>
    requires std::derived_from<typename InputIt::iterator_category, anya::input_iterator_tag>
    unordered_map(InputIt first, InputIt last,
                  size_type bucket_count = default_size,
                  const hasher& hash = hasher(),
                  const key_equal& equal = key_equal())
        : table(bucket_count, hash, equal) {
        while (first != last) emplace(*first++);
    }

    unordered_map(const unordered_map&) = default;

    unordered_map(unordered_map&&) noexcept = default;

    unordered_map(std::initializer_list<value_type> init,
                  size_type bucket_count = default_size,
                  const hasher& hash = hasher(),
                  const key_equal& equal = key_equal())
        : table(bucket_count, hash, equal) {
        auto first = init.begin(), last = init.end();
        while (first != last) emplace(*first++);
    }

    ~unordered_map() = default;
#pragma endregion


#pragma region 赋值
public:
    unordered_map&
    operator=(const unordered_map&) = default;

    unordered_map&
    operator=(unordered_map&&) noexcept = default;

    allocator_type
    get_allocator() const noexcept { return table.get_allocator(); }
#pragma endregion


#pragma region 迭代器
public:
    iterator
    begin() noexcept { return table.begin(); }

    const_iterator
    begin() const noexcept { return table.begin(); }

    const_iterator
    cbegin() const noexcept { return table.cbegin(); }

    iterator
    end() noexcept { return table.end(); }

    const_iterator
    end() const noexcept { return table.end(); }

    const_iterator
    cend() const noexcept { return table.cend(); }
#pragma endregion


#pragma region 容量
public:
    [[nodiscard]] bool
    empty() const noexcept { return table.empty(); }

    [[nodiscard]] size_type
    size() const noexcept { return table.size(); }

    [[nodiscard]] size_type
    max_size() const noexcept { return table.max_size(); }
#pragma endregion


#pragma region 修改器
public:
    void
    clear() noexcept { return table.clear(); }

    std::pair<iterator, bool>
    insert(const value_type& value) { return emplace(value); }

    std::pair<iterator, bool>
    insert(value_type&& value) { return emplace(std::move(value)); }

    template<class InputIt>
    requires std::derived_from<typename InputIt::iterator_category, anya::input_iterator_tag>
    void
    insert(InputIt first, InputIt last) {
        while (first != last) emplace(*first++);
    }

    void
    insert(std::initializer_list<value_type> ilist) {
        auto first = ilist.begin(), last = ilist.end();
        while (first != last) emplace(*first++);
    }

    template<class... Args>
    std::pair<iterator, bool>
    emplace(Args&&... args) {
        return table.template emplace_unique(std::forward<Args>(args)...);
    }

    iterator
    erase(const_iterator pos) { return table.erase(pos); }

    iterator
    erase(const_iterator first, const_iterator last) { return table.erase(first, last); }

    size_type
    erase(const Key& key) { return table.erase(key); }

    void
    swap(unordered_map &other) noexcept { table.swap(other.table); }
#pragma endregion


#pragma region 查找
public:
    T&
    at(const Key& key) {
        auto it = table.find(key);
        if (it == table.end())
            throw std::out_of_range("unordered_map has not this key");
        return it->second;
    }

    const T&
    at(const Key& key) const {
        auto it = table.find(key);
        if (it == table.end())
            throw std::out_of_range("unordered_map has not this key");
        return it->second;
    }

    T&
    operator[](const Key& key) {
        auto ret = table.emplace_unique(key, T{});
        return ret.first->second;
    }

    T&
    operator[](Key&& key) {
        auto ret = table.emplace_unique(std::move(key), T{});
        return ret.first->second;
    }

    size_type
    count(const Key& key) const { return table.count(key); }

    iterator
    find(const Key& key) { return table.find(key); }

    const_iterator
    find(const Key& key) const { return table.find(key); }

    bool
    contains(const Key& key) const { return find(key) != end(); }

    std::pair<iterator, iterator>
    equal_range(const Key& key) { return table.equal_range(key); }

    std::pair<const_iterator, const_iterator>
    equal_range(const Key& key) const { return table.equal_range(key); }
#pragma endregion


#pragma region 桶接口
public:
    [[nodiscard]] size_type
    bucket_count() const { return table.bucket_count(); }

    [[nodiscard]] size_type
    max_bucket_count() const { return table.max_bucket_count(); }

    [[nodiscard]] size_type
    bucket_size(size_type n) const { return table.bucket_size(n); }

    [[nodiscard]] size_type
    bucket(const Key& key) const { return table.bucket(key); }
#pragma endregion


#pragma region 哈希策略
public:
    [[nodiscard]] float
    max_load_factor() const { return table.max_load_factor(); }

    void
    max_load_factor(float ml) { table.max_load_factor(ml); }

    void
    rehash(size_type count) { table.rehash(count); }

    void
    reserve(size_type count) { table.reserve(count); }
#pragma endregion


#pragma region 观察器
public:
    hasher
    hash_function() const { return table.hash_function(); }

    key_equal
    key_eq() const { return table.key_eq(); }
#pragma endregion


#pragma region 友元比较函数
public:
    friend bool
    operator==(const unordered_map& lhs, const unordered_map& rhs) {
        return lhs.table == rhs.table;
    }

    friend bool
    operator!=(const unordered_map& lhs, const unordered_map& rhs) {
        return !(rhs == lhs);
    }
#pragma endregion
};

// 特化 anya::swap 算法
template<
    class Key,
    class T,
    class Hash      = std::hash<Key>,
    class KeyEqual  = std::equal_to<Key>,
    class Allocator = anya::allocator<std::pair<const Key, T>>>
constexpr void
swap(anya::unordered_map<Key, T, Hash, KeyEqual, Allocator>& lhs,
     anya::unordered_map<Key, T, Hash, KeyEqual, Allocator>& rhs) noexcept {
    lhs.swap(rhs);
}


}

#endif //ANYA_STL_UNORDERED_MAP_HPP
