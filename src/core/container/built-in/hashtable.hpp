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
        // TODO: 待确定，不知道加入const是否正确
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
            return lhs.cur == rhs.cur;
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
    max_size() const noexcept { return primers[number_of_primer - 1]; }
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
#pragma endregion


#pragma region 友元比较函数
public:

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
    destroy_node(bucket_node* node) {
        default_alloc.template destroy(std::addressof(node->value));
        bucket_node_alloc.deallocate(node, 1);
        --this->elements;
    }

#pragma endregion


#pragma region 工具函数
private:
    // 头插法，让插入的结点成为新的头
    bucket_node*
    insert_head(bucket_node*& head, bucket_node* node) {
        node->next = head, head = node;
        return head;
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

    // 判断是否超载
    [[nodiscard]] bool
    is_overload(size_t element_size, size_t bucket_size) const {
        return static_cast<float>(element_size) < static_cast<float>(bucket_size) * factor;
    }

    // 获取最接近target的primer
    [[nodiscard]] size_t
    next_primer(size_t target) const {
        auto finish = primers + number_of_primer;
        auto it = std::lower_bound(primers, finish, target);
        return it == finish ? *--finish : *it;
    }

    // 根据哈希函数获取key在size个bucket中应该位于的下标
    size_t
    bucket_index(const Key& key, size_t size) const {
        return hash_fcn(key) % size;
    }

#pragma endregion
};

}

#endif //ANYA_STL_HASHTABLE_HPP
