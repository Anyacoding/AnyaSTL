//
// Created by Anya on 2023/2/25.
//

#ifndef ANYA_STL_ANYA_ALLOC_HPP
#define ANYA_STL_ANYA_ALLOC_HPP

#include <new>
#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <numeric>
#include <iterator>
#include <concepts>
#include "iterator/iterator.hpp"

namespace anya {

#ifndef THROW_BAD_ALLOC
#  if defined(__STL_NO_BAD_ALLOC) || !defined(__STL_USE_EXCEPTIONS)
#    include <cstdio>
#    include <cstdlib>
#    define THROW_BAD_ALLOC fprintf(stderr, "out of memory\n"); exit(1)
#  else /* Standard conforming out-of-memory handling */
#    include <new>
#    define THROW_BAD_ALLOC throw std::bad_alloc()
#  endif
#endif

// thread-unsafe
#define NODE_ALLOCATOR_THREADS false

#pragma region 第一级配置器
// 第一级配置器 malloc_based allocator
// 无模板形参，而非模板形参 inst 其实也没有被用上
// 因为用的是malloc，所以一般是 thread safe 的
template<int inst>
class malloc_alloc_template {
private:
    static void (*malloc_alloc_oom_handler)();

private:
    // oom : out of memory
    // 以下的函数指针和函数将用来处理内存不足的情况
    static void*
    oom_malloc(size_t n) {
        void (*my_malloc_handler)() = nullptr;
        void* result = nullptr;
        while (true) {
            my_malloc_handler = malloc_alloc_oom_handler;
            if (my_malloc_handler == nullptr) { THROW_BAD_ALLOC; }
            my_malloc_handler();    // 调用处理程序，尝试释放内存
            result = malloc(n);     // 再次尝试配置内存
            if (result) return result;
        }
        return result;
    }

    static void*
    oom_realloc(void* p, size_t n) {
        void (*my_malloc_handler)() = nullptr;
        void* result = nullptr;

        while (true) {
            my_malloc_handler = malloc_alloc_oom_handler;
            if (my_malloc_handler == nullptr) { THROW_BAD_ALLOC; }
            my_malloc_handler();        // 调用处理程序，尝试释放内存
            result = realloc(p, n);     // 再次尝试配置内存
            if (result) return result;
        }
        return result;
    }

public:
    static void*
    allocate(size_t n) {
        void* result = malloc(n);
        if (result == nullptr) result = oom_malloc(n);
        return result;
    }

    static void*
    reallocate(void* p, size_t old_sz, size_t new_sz) {
        void* result = realloc(p, new_sz);         // 第一级配置器直接调用 realloc
        if (result == nullptr) result = oom_realloc(p, new_sz);  // 如果失败了，改用 oom_realloc
        return result;
    }

    static void
    deallocate(void* p, size_t n) {
        free(p);   // 第一级配置器直接调用 free
    }

    // 以下模拟C++的 set_new_handler()
    // 之所以没有用set_new_handler()是因为第一级配置器使用的是 malloc 而不是::operator new
    // 用户可以指定自定义版本的 out-of-memory handler
    static auto set_malloc_handler(void (*f)()) -> void (*)() {
        void (*old)() = malloc_alloc_oom_handler;
        malloc_alloc_oom_handler = f;
        return old;
    }
};

// 初值为nullptr，由用户自定义
template<int inst>
void (*malloc_alloc_template<inst>::malloc_alloc_oom_handler)() = nullptr;

// 以下直接将参数 inst 指定为0
using malloc_alloc = malloc_alloc_template<0>;

#pragma endregion

#pragma region 第二级配置器
// 以下是第二级配置器，我们默认使用的alloc就是这个
// 无模板形参，且第二参数也没有派上用场
// 第一参数在 SGI STL 中是用于多线程环境下的，这里给与保留方便以后扩展，但在这一版中不考虑多线程环境
template<bool threads, int inst>
class default_alloc_template {
private:
    static constexpr size_t ALIGN = 8;                       // 小型区块的对齐边界
    static constexpr size_t MAX_BYTES = 128;                 // 小型区块的上限
    static constexpr size_t NFREELISTS = MAX_BYTES / ALIGN;  // free-list 个数

private:
    // 将 bytes 上调至8的倍数
    constexpr static size_t
    ROUND_UP(size_t bytes) {
        return ((bytes + ALIGN - 1) & ~(ALIGN - 1));
    }

private:
    union obj {
        union obj* free_list_link;   // free_list 的节点
        char  client_data[1];        // 用户看到的是这一个
    };

private:
    // 16个 free_list
    static obj* volatile free_list[NFREELISTS];

    // 根据区块大小，决定使用第n号free-list
    constexpr static size_t
    FREELIST_INDEX(size_t bytes) {
        return (bytes + ALIGN - 1) / ALIGN - 1;
    }

    // 返回一个大小为n的对象，在可能的情况下把大小为n的其他块加入到 free-list 中
    static void*
    refill(size_t n) {
        int nobjs = 20;
        // 调用 chunk_alloc 尝试取得 nobjs 个区块作为 free-list 的新节点
        // 参数 nobjs 是 pass by ref
        // 我们需要保证 n 已经是8的倍数
        char* chunk = chunk_malloc(n, nobjs);
        // 如果只获得一个区块，则把这个区块直接返回给用户
        if (nobjs == 1) return chunk;
        // 否则准备调整 free-list
        obj* volatile* my_free_list = free_list + FREELIST_INDEX(n);

        obj* result  = (obj*)chunk;        // 这一块准备返回给客户
        obj* next    = (obj*)(chunk + n);  // 剩下的 nobjs-1 块填充进 free-list
        obj* current = nullptr;
        *my_free_list = next;

        for (int i = 1; ; ++i) {
            current = next;
            next    = (obj*)((char*)next + n);
            if (i == nobjs - 1) {
                current->free_list_link = nullptr;
                break;
            }
            else {
                current->free_list_link = next;
            }
        }
        return result;
    }

    // 配置一大块空间，可容纳 nobjs 个大小为 "size" 的区块
    // 如果空间不够，通过引用同步修改 nobjs 的大小
    // 由调用者保证size已经是8的倍数
    static char*
    chunk_malloc(size_t size, int& nobjs) {
        char* result = nullptr;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free;  // 内存池剩余空间

        if (bytes_left >= total_bytes) {
            // 剩余空间完全满足要求
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        else if (bytes_left >= size) {
            // 剩余空间不完全满足要求，但能返回一个以上的块
            nobjs = bytes_left / size;
            total_bytes = size * nobjs;
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        else {
            // 剩余空间连一个块都提供不了
            size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
            // 如果内存池中还有剩余的块，先配给适当的 free-list
            if (bytes_left > 0) {
                obj* volatile* my_free_list = free_list + FREELIST_INDEX(bytes_left);
                ((obj*)start_free)->free_list_link = *my_free_list;
                *my_free_list = (obj*)start_free;
            }

            start_free = (char*)malloc(bytes_to_get);

            // heap空间不足，malloc 失败
            if (start_free == nullptr) {
                obj* volatile* my_free_list = nullptr;
                obj* p = nullptr;
                // 试着看看我们手上的 free-list, 寻找适当的 free-list
                // 合适指的是"未使用但区块足够大"的 free-list
                // 不要尝试配置小区块，因为在多线程下很有可能是有问题的
                for (int i = size; i <= MAX_BYTES; i += ALIGN) {
                    my_free_list = free_list + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if (p) {
                        // free-list 尚有未用合适之区块
                        *my_free_list = p->free_list_link;
                        start_free = (char*)p;
                        end_free = start_free + i;
                        // 在调整 nobjs 的同时，内存池中的残余零头将会被配置到合适的 free-list
                        return chunk_malloc(size, nobjs);
                    }
                }
                end_free = nullptr;   //内存池中一滴都不剩了
                start_free = (char*)malloc_alloc::allocate(bytes_to_get);  // 转去调用第一级分配器，看看oom还能不能抢救一下，不能则抛出异常
            }

            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            // 递归调用自己，为了修正 nobjs
            return chunk_malloc(size, nobjs);
        }
    }

private:
    static char*  start_free;   // 内存池起始位置，只在 chunk_malloc 中变化
    static char*  end_free;     // 内存池结束位置，只在 chunk_malloc 中变化
    static size_t heap_size;

public:
    static void*
    allocate(size_t n) {
        obj* volatile* my_free_list = nullptr;
        obj* result = nullptr;

        // 大于128的大区块就直接调用第一级分配器
        if (n > MAX_BYTES) {
            return malloc_alloc::allocate(n);
        }
        // 寻找16个 free-list 中适合的那一个
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;
        if (result == nullptr) {
            // free-list 无可用的块了，准备填充free-list
            void* p = refill(ROUND_UP(n));
            return p;
        }
        // free-list 有空闲的块，取出一个给用户并把 free-list 的指针指向下一格空闲块
        *my_free_list = result->free_list_link;
        return result;
    }

    static void*
    reallocate(void* p, size_t old_sz, size_t new_sz) {
        // 大于128的块则交由第一级配置器处理
        if (old_sz > (size_t)MAX_BYTES && new_sz > (size_t)MAX_BYTES) {
            return malloc_alloc::reallocate(p, old_sz, new_sz);
        }
        // 由于第二级配置器配置的内存都是8的倍数的块，若 ROUND_UP 后一致则直接返回原来的地址即可
        if (ROUND_UP(old_sz) == ROUND_UP(new_sz)) return p;
        void* result = allocate(new_sz);
        size_t copy_sz = old_sz < new_sz ? old_sz : new_sz;
        ::memcpy(result, p, copy_sz);
        deallocate(p, old_sz);
        return result;
    }

    static void
    deallocate(void* p, size_t n) {
        obj* volatile* my_free_list = nullptr;
        obj* q = (obj*)p;

        // 大于128的块交由第一级配置器回收
        if (n > MAX_BYTES) {
            malloc_alloc::deallocate(p, n);
            return;
        }
        // 小区块则回收至 free-list
        my_free_list = free_list + FREELIST_INDEX(n);
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }
};

// 初始化 static data member
template<bool threads, int inst>
char* default_alloc_template<threads, inst>::start_free = nullptr;

template<bool threads, int inst>
char* default_alloc_template<threads, inst>::end_free = nullptr;

template<bool threads, int inst>
size_t default_alloc_template<threads, inst>::heap_size = 0;

// 这里必须加一个typename来暗示obj是类型
template<bool threads, int inst>
typename default_alloc_template<threads, inst>::obj* volatile
         default_alloc_template<threads, inst>::free_list[default_alloc_template<threads, inst>::NFREELISTS] =
         { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

#pragma endregion

#pragma region 用户使用的配置器

#ifdef  __USE_MALLOC
using malloc_alloc = malloc_alloc_template<0>;
using alloc = malloc_alloc;
#else
using alloc = default_alloc_template<NODE_ALLOCATOR_THREADS, 0>;
#endif


template<class T, class Alloc = alloc>
class allocator {
public:
    using value_type                             = T;
    using pointer                                = T*;
    using const_pointer                          = const T*;
    using reference                              = T&;
    using const_reference                        = const T&;
    using size_type                              = std::size_t;
    using difference_type                        = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal                        = std::true_type;

    // 重新绑定分配器
    template<class U>
    struct rebind {
        using other = allocator<U>;
    };

public:
    allocator() = default;
    allocator(const allocator&) = default;
    ~allocator() = default;

    // 开辟内存
    [[nodiscard]] constexpr pointer
    allocate(size_t n) {
        if (std::numeric_limits<std::size_t>::max() / sizeof(T) < n)
            throw std::bad_array_new_length();
        return n == 0 ? nullptr : (T*)Alloc::allocate(n * sizeof(T));
    }

    // 回收内存
    constexpr void
    deallocate(T* p, size_t n) const noexcept {
        if (n != 0) Alloc::deallocate(p, n * sizeof(T));
    }

    constexpr void
    deallocate(T* p) const noexcept {
        Alloc::deallocate(p, sizeof(T));
    }

    // 取地址
    constexpr pointer
    address(reference x) const noexcept {
        return address_of(x);
    };

    constexpr const_pointer
    address(const_reference x) const noexcept {
        return address_of(x);
    };

    // 构造已开辟内存的对象
    template<class U, class... Args>
    constexpr void
    construct(U* p, Args&&... args) {
        ::new(const_cast<void*>(static_cast<const volatile void*>(p)))
            U(std::forward<Args>(args)...);
    }

    // 析构已经开辟内存的对象
    template< class U >
    constexpr void
    destroy(U* p) {
        p->~U();
    }

    [[nodiscard]] size_type
    max_size() const noexcept {
        return std::numeric_limits<difference_type>::max() / sizeof(T);
    }

private:
    constexpr pointer
    address_of(const_reference x) const noexcept {
        return reinterpret_cast<pointer> (
                    &const_cast<std::byte&> (
                        reinterpret_cast<const volatile std::byte&>(x)
                    )
               );
    };
};

#pragma endregion

template<typename T>
void
destroy_at(T* x) {
    x->~T();
}

template<typename ForwardIt>
void
destroy(ForwardIt first, ForwardIt last) {
    using V = typename anya::iterator_traits<ForwardIt>::value_type;
    for (; first != last; ++first) anya::destroy_at(anya::allocator<V>().address(*first));
}


#pragma region 未初始化内存算法
/*!
 * @tparam InputIt
 * @tparam NoThrowForwardIt
 * @param first     要复制的元素的左闭上界
 * @param last      要复制的元素的右开下界
 * @param d_first   目标范围的起始
 * @return          指向最后复制的元素后一元素的迭代器
 */
template<class InputIt, class NoThrowForwardIt>
NoThrowForwardIt
uninitialized_copy(InputIt first, InputIt last, NoThrowForwardIt d_first) {
    using T = typename anya::iterator_traits<NoThrowForwardIt>::value_type;
    NoThrowForwardIt current = d_first;
    try {
        for (; first != last; ++first, (void)++current) {
            ::new(static_cast<void*>(std::addressof(*current))) T(*first);
        }
        return current;
    }
    catch (...) {
        for (; d_first != current; ++d_first) {
            d_first->~T();
        }
        throw;
    }
}

/*!
 * @tparam InputIt
 * @tparam Size
 * @tparam NoThrowForwardIt
 * @param first     要复制的元素的左闭上界
 * @param count     要重复复制的次数
 * @param d_first   目标范围的起始
 * @return          指向最后复制的元素后一元素的迭代器
 */
template<class InputIt, class Size, class NoThrowForwardIt>
NoThrowForwardIt
uninitialized_copy_n(InputIt first, Size count, NoThrowForwardIt d_first) {
    using T = typename anya::iterator_traits<NoThrowForwardIt>::value_type;
    NoThrowForwardIt current = d_first;
    try {
        for (; count > 0; ++first, (void)++current, --count) {
            ::new(static_cast<void*>(std::addressof(*current))) T(*first);
        }
        return current;
    }
    catch (...) {
        for (; d_first != current; ++d_first) {
            d_first->~T();
        }
        throw;
    }
}

/*!
 * @tparam InputIt
 * @tparam NoThrowForwardIt
 * @param first    要移动的元素的左闭上界
 * @param last     要移动的元素的右开下界
 * @param d_first  目标范围的起始
 * @return         指向最后移动的元素后一元素的迭代器
 */
template<class InputIt, class NoThrowForwardIt>
NoThrowForwardIt
uninitialized_move(InputIt first, InputIt last, NoThrowForwardIt d_first) {
    using Value = typename anya::iterator_traits<NoThrowForwardIt>::value_type;
    NoThrowForwardIt current = d_first;
    try {
        for (; first != last; ++first, (void) ++current) {
            ::new (static_cast<void*>(std::addressof(*current))) Value(std::move(*first));
        }
        return current;
    }
    catch (...) {
        anya::destroy(d_first, current);
        throw;
    }
}

/*!
 * @tparam InputIt
 * @tparam Size
 * @tparam NoThrowForwardIt
 * @param first    要移动的元素范围起始
 * @param count    要移动的元素数目
 * @param d_first  目标范围的起始
 * @return         一对迭代器，其首元素是指向源范围中最后被移动的元素后一元素的迭代器，第二元素是指向目标范围中最后移动到的元素后一元素的迭代器
 */
template<class InputIt, class Size, class NoThrowForwardIt>
std::pair<InputIt, NoThrowForwardIt>
uninitialized_move_n(InputIt first, Size count, NoThrowForwardIt d_first) {
    using Value = typename anya::iterator_traits<NoThrowForwardIt>::value_type;
    NoThrowForwardIt current = d_first;
    try {
        for (; count > 0; ++first, (void) ++current, --count) {
            ::new (const_cast<void*>(static_cast<const volatile void*>(
                std::addressof(*current)))) Value(std::move(*first));
        }
    } catch (...) {
        anya::destroy(d_first, current);
        throw;
    }
    return {first, current};
}

/*!
 * @tparam ForwardIt
 * @tparam T
 * @param first   要初始化的元素的左闭上界
 * @param last    要初始化的元素的右开下界
 * @param value   构造元素所用的值
 */
template<class ForwardIt, class T>
void
uninitialized_fill(ForwardIt first, ForwardIt last, const T& value) {
    using V = typename anya::iterator_traits<ForwardIt>::value_type;
    ForwardIt current = first;
    try {
        for (; current != last; ++current) {
            ::new(static_cast<void*>(std::addressof(*current))) V(value);
        }
    }
    catch (...) {
        for (; first != current; ++first) {
            first->~V();
        }
        throw;
    }
}


/*!
 * @tparam ForwardIt
 * @tparam Size
 * @tparam T
 * @param first   要初始化的元素的左闭上界
 * @param count   要构造的元素数量
 * @param value   构造元素所用的值
 * @return        指向最后初始化的元素后一元素的迭代器
 */
template<class ForwardIt, class Size, class T>
ForwardIt
uninitialized_fill_n(ForwardIt first, Size count, const T& value) {
    using V = typename anya::iterator_traits<ForwardIt>::value_type;
    ForwardIt current = first;
    try {
        for (; count > 0; ++current, (void)--count) {
            ::new(static_cast<void*>(std::addressof(*current))) V(value);
        }
        return current;
    }
    catch (...) {
        for (; first != current; ++first) {
            first->~V();
        }
        throw;
    }
}

/*!
 * @tparam ForwardIt
 * @param first   要初始化的元素的左闭上界
 * @param last    要初始化的元素的右开下界
 */
template<class ForwardIt>
void
uninitialized_default_construct(ForwardIt first, ForwardIt last) {
    using Value = typename anya::iterator_traits<ForwardIt>::value_type;
    ForwardIt current = first;
    try {
        for (; current != last; ++current) {
            ::new (const_cast<void*>(static_cast<const volatile void*>(
                std::addressof(*current)))) Value;
        }
    }
    catch (...) {
        for (; first != current; ++first) {
            first->~Value();
        }
        throw;
    }
}

/*!
 * @tparam ForwardIt
 * @tparam Size
 * @param first 要初始化的元素的左闭上界
 * @param n     要构造的元素数量
 * @return      指向最后初始化的元素后一元素的迭代器
 */
template< class ForwardIt, class Size >
ForwardIt
uninitialized_default_construct_n(ForwardIt first, Size n) {
    using T = typename anya::iterator_traits<ForwardIt>::value_type;
    ForwardIt current = first;
    try {
        for (; n > 0; (void)++current, --n) {
            ::new (const_cast<void*>(static_cast<const volatile void*>(
                std::addressof(*current)))) T;
        }
        return current;
    }
    catch (...) {
        for (; first != current; ++first) {
            first->~T();
        }
        throw;
    }
}

#pragma endregion

}

#endif //ANYA_STL_ANYA_ALLOC_HPP
