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
        bucket_node* current;  // 迭代器当前位置
        hashtable*   table;    // 所属的容器

    public:
        using value_type      = typename hashtable_iterator::value_type;
        using pointer         = typename hashtable_iterator::pointer;
        using reference       = typename hashtable_iterator::reference;
        using difference_type = typename hashtable_iterator::difference_type;

    public:
        hashtable_iterator(bucket_node* node, hashtable* belong)
            : current(node), table(belong) {}

        hashtable_iterator() = default;

        hashtable_iterator(const hashtable_iterator&) = default;

        // TODO: 待确定，不知道是否正确
        template<typename U>
        requires std::same_as<U, T>
        hashtable_iterator(const hashtable_iterator<U>& oth)
            noexcept: current(oth.current), table(oth.table) {}

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

    constexpr static size_t default_size = 11;
    constexpr static size_t primers[28] = {
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


#pragma region 容量
public:
    [[nodiscard]] bool
    empty() const noexcept { return elements == 0; }

    [[nodiscard]] size_type
    size() const noexcept { return elements; }
#pragma endregion


#pragma region 修改器
public:
    void
    clear() { destroy_all(); }
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
    insert_front(bucket_node*& head, bucket_node* node) {
        node->next = head, head = node;
        return head;
    }

    void
    deep_copy_from(const hashtable& other) {
        clear();
        size_t bucket_size = other.buckets.size();
        buckets.resize(bucket_size, nullptr);
        for (size_t i = 0; i < bucket_size; ++i) {
            if (auto ptr = other.buckets[i]) {
                while (ptr) {
                    bucket_node* temp = make_node(ptr->value);
                    insert_front(buckets[i], temp);
                    ptr = ptr->next;
                }
            }
        }
    }

#pragma endregion
};

}

#endif //ANYA_STL_HASHTABLE_HPP
