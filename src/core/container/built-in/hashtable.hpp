//
// Created by Anya on 2023/5/30.
//

#ifndef ANYA_STL_HASHTABLE_HPP
#define ANYA_STL_HASHTABLE_HPP

#include "container/vector.hpp"
#include "iterator/iterator.hpp"

namespace anya {

template<
    class Key,
    class T,
    class Hash      = std::hash<Key>,
    class KeyEqual  = std::equal_to<Key>,
    class Allocator = anya::allocator<std::pair<const Key, T>>>
class hashtable {
#pragma region 迭代器实现
private:
    // 开链法结点
    struct bucket_node {
        bucket_node* next;
        std::pair<const Key, T> value;
    };

private:
    template<class Tp>
    class hashtable_iterator
        : public anya::iterator<anya::forward_iterator_tag, Tp> {
    private:
        friend class hashtable;

    private:
        bucket_node* current;   // 迭代器当前位置
        const hashtable* table; // 所属的容器

    public:
        using value_type      = typename hashtable_iterator::value_type;
        using pointer         = typename hashtable_iterator::pointer;
        using reference       = typename hashtable_iterator::reference;
        using difference_type = typename hashtable_iterator::difference_type;

    public:
        hashtable_iterator(bucket_node* node, const hashtable* belong)
            : current(node), table(belong) {}

        hashtable_iterator() = default;

        hashtable_iterator(const hashtable_iterator&) = default;

        template<typename U>
        requires std::same_as<U, std::pair<const Key, T>>
        hashtable_iterator(const hashtable_iterator<U>& other)
            noexcept: current(other.current), table(other.table) {}

    public:
        reference
        operator*() const { return current->value; }

        pointer
        operator->() const { return std::addressof(current->value); }

        hashtable_iterator&
        operator++() {
            current = table->next_node(current); return *this;
        }

        hashtable_iterator
        operator++(int) {
            hashtable_iterator tmp = *this; return ++*this, tmp;
        }

        friend bool
        operator==(const hashtable_iterator& lhs,
                   const hashtable_iterator& rhs) {
            return lhs.current == rhs.current;
        }

        friend bool
        operator!=(const hashtable_iterator& lhs,
                   const hashtable_iterator& rhs) {
            return !(rhs == lhs);
        }
    };
#pragma endregion

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
    using iterator        = hashtable_iterator<value_type>;
    using const_iterator  = hashtable_iterator<const value_type>;

private:
    using bucket_container = anya::vector<bucket_node*>;
    using node_alloc_type  = anya::allocator<bucket_node>;

    allocator_type  default_alloc{};      // 普通内存分配器
    node_alloc_type bucket_node_alloc{};  // bucket_node 内存分配器

    hasher           hash_fcn{};   // 哈希函数
    key_equal        equal_fcn{};  // 比较函数
    bucket_container buckets{};    // 桶数组
    size_t           elements{};   // 元素数量
    float            factor = 1;   // 装置因子

    // 桶的个数，以及备用质数表
    constexpr static size_t default_size = 11;
    constexpr static size_t number_of_primer = 28;
    constexpr static size_t primers[number_of_primer] = {
        53, 97, 193, 389,
        769, 1543, 3079, 6151,
        12289, 24593, 49157, 98317,
        196613, 393241, 786433, 1572869,
        3145739, 6291469, 12582917, 25165843,
        50331653, 100663319, 201326611, 402653189,
        805306457, 1610612741, 3221225473ul, 4294967291ul
    };

#pragma region 构造 && 析构
public:
    hashtable() : hashtable(default_size) {}

    explicit hashtable(size_t bucket_count,
                       const hasher& hash = hasher(),
                       const key_equal& equal = key_equal())
        : hash_fcn(hash), equal_fcn(equal), buckets(bucket_count, nullptr)
    {}

    hashtable(const hashtable& other) {
        deep_copy_from(other);
    }

    hashtable(hashtable&& other) noexcept:
        buckets(std::move(other.buckets)),
        hash_fcn(std::move(other.hash_fcn)),
        equal_fcn(std::move(other.equal_fcn)) {
        elements = other.elements, other.elements = 0;
    }

    ~hashtable() { destroy_all(); }
#pragma endregion


#pragma region 赋值
public:
    hashtable&
    operator=(const hashtable& other) {
        if (&other != this) deep_copy_from(other);
        return *this;
    }

    hashtable&
    operator=(hashtable&& other) noexcept {
        if (&other == this) return *this;
        buckets = std::move(other.buckets);
        elements = other.num_elements, other.num_elements = 0;
        hash_fcn = std::move(other.hash_fcn), equal_fcn = std::move(other.equal_fcn);
        return *this;
    }

    allocator_type
    get_allocator() const noexcept { return default_alloc; }
#pragma endregion


#pragma region 迭代器
public:
    iterator
    begin() noexcept { return iterator{first_bucket(), this}; }

    const_iterator
    begin() const noexcept { return const_iterator(first_bucket(), this); }

    const_iterator
    cbegin() const noexcept { return const_iterator(first_bucket(), this); }

    iterator
    end() noexcept { return iterator(nullptr, this); }

    const_iterator
    end() const noexcept { return const_iterator(nullptr, this); }

    const_iterator
    cend() const noexcept { return const_iterator(nullptr, this); }
#pragma endregion


#pragma region 容量
public:
    [[nodiscard]] bool
    empty() const noexcept { return this->elements == 0; }

    [[nodiscard]] size_type
    size() const noexcept { return this->elements; }

    [[nodiscard]] size_type
    max_size() const noexcept { return default_alloc.max_size(); }
#pragma endregion


#pragma region 修改器
public:
    void
    clear() { destroy_all(); }

    // 设置为合适的size
    void
    resize(size_t hint_elements) {
        size_t bucket_size = buckets.size();
        if (is_overload(hint_elements, bucket_size) == false) return;
        size_t new_bucket_size = next_primer(hint_elements);
        bucket_container temp(new_bucket_size, nullptr);
        bucket_node* next;
        for (bucket_node* ptr : this->buckets) {
            while (ptr) {
                size_t new_index = bucket_index(ptr->value.first, new_bucket_size);
                next = ptr->next;
                insert_head(temp[new_index], ptr);
                ptr = next;
            }
        }
        buckets.swap(temp);
    }

    // 不可重复置入
    template<class... Args>
    std::pair<iterator, bool>
    emplace_unique(Args&&... args) {
        resize(this->elements + 1);
        return insert_unique(value_type(std::forward<Args>(args)...));
    }

    // 可重复置入
    template<class... Args>
    std::pair<iterator, bool>
    emplace_multi(Args&&... args) {
        resize(this->elements + 1);
        return insert_multi(value_type(std::forward<Args>(args)...));
    }

    iterator
    erase(const_iterator pos) {
        size_t index = bucket_index(pos->first);
        bucket_node* current = buckets[index];
        bucket_node* ptr = pos.current, *next = ptr->next, *pre = nullptr;
        while (current != ptr) pre = current, current = current->next;
        connect_next(pre, next, index);
        destroy_node(current);
        return {next, this};
    }

    iterator
    erase(const_iterator first, const_iterator last) {
        if (first == last) return {last.current, this};
        size_t index = bucket_index(first->first), buckets_size = buckets.size();
        bucket_node* current = buckets[index];
        bucket_node* pre = nullptr, *next = nullptr;
        bucket_node* start = first.current, *finish = last.current;
        // 找到第一个迭代器的前驱
        while (current != start) pre = current, current = current->next;
        while (true) {
            next = current->next, destroy_node(current);
            current = next;
            // 需要跨越bucket
            if (current == nullptr) {
                connect_next(pre, next, index), pre = nullptr;
                while (!current && ++index < buckets_size) current = buckets[index];
            }
            if (current == finish) {
                if (finish) connect_next(pre, next, index);
                break;
            }
        }
        return {last.current, this};
    }

    size_type
    erase(const Key& key) {
        size_t index = bucket_index(key), cnt = 0;
        bucket_node* current = buckets[index];
        bucket_node* pre = nullptr, *next = nullptr;
        while (current && !equal_fcn(current->value.first, key))
            pre = current, current = current->next;
        // 因为相同的值肯定在同一个哈希桶里，所以这里可以直接返回
        if (!current) return 0;
        do {
            next = current->next, destroy_node(current);
            current = next, ++cnt;
        } while (current && equal_fcn(current->value.first, key));
        (pre ? pre->next : buckets[index]) = current;
        return cnt;
    }

    void
    swap(hashtable& other) noexcept {
        buckets.swap(other.buckets);
        std::swap(this->elements, other.elements);
        std::swap(this->hash_fcn, other.hash_fcn);
        std::swap(this->equal_fcn, other.equal_fcn);
    }
#pragma endregion


#pragma region 查找
public:
    size_t
    count(const Key& key) const {
        size_t cnt = 0, pos = bucket_index(key);
        bucket_node* current = buckets[pos];
        while (current && !equal_fcn(current->value.first, key))
            current = current->next;
        while (current && equal_fcn(current->value.first, key))
            current = current->next, ++cnt;
        return cnt;
    }

    iterator
    find(const Key& key) { return {find_by_key(key), this}; }

    const_iterator
    find(const Key& key) const { return {find_by_key(key), this}; }

    std::pair<iterator, iterator>
    equal_range(const Key& key) {
        auto tmp = find_range_by_key(key);
        return {{tmp.first,  this}, {tmp.second, this}};
    }

    std::pair<const_iterator, const_iterator>
    equal_range(const Key& key) const {
        auto tmp = find_range_by_key(key);
        return {{tmp.first,  this}, {tmp.second, this}};
    }
#pragma endregion


#pragma region 桶接口
public:
    [[nodiscard]] size_type
    bucket_count() const { return buckets.size(); }

    [[nodiscard]] size_type
    max_bucket_count() const { return primers[number_of_primer - 1]; }

    [[nodiscard]] size_type
    bucket_size(size_type n) const {
        size_type cnt = 0;
        bucket_node* current = buckets[n];
        while (current) current = current->next, ++cnt;
        return cnt;
    }

    [[nodiscard]] size_type
    bucket(const Key& key) const { return bucket_index(key); }
#pragma endregion


#pragma region 哈希策略
public:
    [[nodiscard]] float
    load_factor() const { return size() / bucket_count(); }

    [[nodiscard]] float
    max_load_factor() const { return factor; }

    void
    max_load_factor(float ml) { factor = ml; }

    void
    rehash(size_type count) { resize(count); }

    void
    reserve(size_type count) { rehash(std::ceil(static_cast<float>(count) / max_load_factor())); }
#pragma endregion


#pragma region 友元比较函数
public:
    friend bool
    operator==(const hashtable& lhs, const hashtable& rhs) {
        if (&lhs == &rhs) return true;
        if (lhs.size() != rhs.size()) return false;
        for (const auto &kv : lhs) {
            if (!rhs.contain_by_key_value(kv)) return false;
        }
        return true;
    }

    friend bool
    operator!=(const hashtable& lhs, const hashtable& rhs) {
        return !(rhs == lhs);
    }
#pragma endregion


#pragma region storage
private:
    // 创建链表bucket_node
    bucket_node*
    make_node(const value_type& kv) {
        bucket_node* node = bucket_node_alloc.allocate(1);
        default_alloc.template construct(std::addressof(node->value), kv);
        ++this->elements;
        return node;
    }

    // 下一个非空结点，没有就返回nullptr
    bucket_node*
    next_node(bucket_node* cur) const {
        if (cur == nullptr) return nullptr;
        bucket_node* old = cur;
        cur = cur->next;
        if (cur == nullptr) {
            size_t pos = bucket_index(old->value.first);
            size_t buckets_size = buckets.size();
            while (!cur && ++pos < buckets_size) cur = buckets[pos];
        }
        return cur;
    }

    // 令 pre->next = next, 当pre为nullptr时，令 buckets[index] = next
    void
    connect_next(bucket_node* pre, bucket_node* next, size_t index) {
        (pre ? pre->next : buckets[index]) = next;
    }

    // 析构并回收buckets里的每个元素
    void
    destroy_all() {
        bucket_node* temp;
        for (auto& ptr : buckets) {
            auto bucket = ptr;
            ptr = nullptr;
            while (bucket) {
                temp = bucket->next;
                destroy_node(bucket);
                bucket = temp;
            }
        }
    }

    // 析构并回收链表的一个节点
    void
    destroy_node(bucket_node*& node) {
        default_alloc.template destroy(std::addressof(node->value));
        bucket_node_alloc.deallocate(node, 1);
        node = nullptr;
        --this->elements;
    }
#pragma endregion


#pragma region 观察器
public:
    hasher
    hash_function() const { return hash_fcn; }

    key_equal
    key_eq() const { return equal_fcn; }
#pragma endregion


#pragma region 工具函数
private:
    // 头插法，让插入的结点成为新的头
    bucket_node*
    insert_head(bucket_node*& head, bucket_node* node) {
        node->next = head, head = node;
        return head;
    }

    // 尾插法，插到pre的后面
    bucket_node*
    insert_tail(bucket_node* pre, bucket_node* node) {
        node->next = pre->next, pre->next = node;
        return node;
    }

    // 不重复插入
    std::pair<iterator, bool>
    insert_unique(const value_type& kv) {
        const size_t pos = bucket_index(kv.first);
        for (auto cur = buckets[pos]; cur != nullptr; cur = cur->next) {
            if (equal_fcn(cur->value.first, kv.first)) {
                return {iterator(cur, this), false};
            }
        }
        // 直接头插法
        auto head = insert_head(buckets[pos], make_node(kv));
        return {iterator(head, this), true};
    }

    // 可重复插入
    std::pair<iterator, bool>
    insert_multi(const value_type& kv) {
        const size_t pos = bucket_index(kv.first);
        for (auto cur = buckets[pos]; cur; cur = cur->next) {
            if (equal_fcn(cur->value.first, kv.first)) {
                // 尾插法接到cur的后面
                auto tail = insert_tail(cur, make_node(kv));
                return {iterator(tail, this), true};
            }
        }
        auto head = insert_head(buckets[pos], make_node(kv));
        return {iterator(head, this), true};
    }

    // 深拷贝哈希表
    void
    deep_copy_from(const hashtable& other) {
        clear();
        size_t bucket_size = other.buckets.size();
        buckets.resize(bucket_size, nullptr);
        for (size_t i = 0; i < bucket_size; ++i) {
            if (auto ptr = other.buckets[i]) {
                while (ptr) {
                    bucket_node* temp = make_node(ptr->value);
                    insert_head(buckets[i], temp);
                    ptr = ptr->next;
                }
            }
        }
    }

    // 获取第一个非空的bucket
    bucket_node*
    first_bucket() const {
        auto it = buckets.cbegin(), finish = buckets.cend();
        while (*it == nullptr && it != finish) ++it;
        return it == finish ? nullptr : *it;
    }

    // 判断是否超载, 超载返回true
    [[nodiscard]] bool
    is_overload(size_t element_size, size_t bucket_size) const {
        return static_cast<float>(element_size) > static_cast<float>(bucket_size) * factor;
    }

    // 获取最接近target的primer
    [[nodiscard]] size_t
    next_primer(size_t target) const {
        auto finish = primers + number_of_primer;
        auto it = std::lower_bound(primers, finish, target);
        return it == finish ? *--finish : *it;
    }

    // 根据哈希函数获取key在size个bucket中应该位于的下标
    [[nodiscard]] size_t
    bucket_index(const Key& key, size_t size) const {
        return hash_fcn(key) % size;
    }

    [[nodiscard]] size_t
    bucket_index(const Key& key) const {
        return bucket_index(key, buckets.size());
    }

    // 查找第一个k为key的结点
    bucket_node*
    find_by_key(const Key& key) const {
        size_t pos = bucket_index(key);
        bucket_node* current = buckets[pos];
        while (current && !equal_fcn(current->value.first, key)) current = current->next;
        return current;
    }

    // 查找k为key的结点范围
    std::pair<bucket_node*, bucket_node*>
    find_range_by_key(const Key& key) const {
        bucket_node* start = find_by_key(key);
        bucket_node* finish = start ? start->next : nullptr;
        while (finish && equal_fcn(finish->value.first, key)) finish = finish->next;
        return {start, finish};
    }

    // 查找是否存在这个kv
    bool
    contain_by_key_value(const value_type& kv) const {
        // DONE: range.first 向后迭代时不能是 ++range.first，否则会错过 range.first != range.second
        for (auto range = find_range_by_key(kv.first); range.first != range.second; range.first = range.first->next) {
            if (range.first->value.second == kv.second) return true;
        }
        return false;
    }

#pragma endregion
};

}

#endif //ANYA_STL_HASHTABLE_HPP
