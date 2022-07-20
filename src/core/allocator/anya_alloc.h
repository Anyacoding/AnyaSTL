#ifndef _ANYA_ALLOC_
#define _ANYA_ALLOC_
#include <new>          // for placement new
#include <iostream>     // for ptrdiff_t, size_t
#include <cstdlib>      // for exit()
#include <cstddef>      // for UNIT_MAX
#include <climits>      // for cerr
#include <cstring>

#ifndef __THROW_BAD_ALLOC
#  if defined(__STL_NO_BAD_ALLOC) || !defined(__STL_USE_EXCEPTIONS)
#    include <cstdio>
#    include <cstdlib>
#    define __THROW_BAD_ALLOC fprintf(stderr, "out of memory\n"); exit(1)
#  else /* Standard conforming out-of-memory handling */
#    include <new>
#    define __THROW_BAD_ALLOC throw std::bad_alloc()
#  endif
#endif

// thread-unsafe
#define __NODE_ALLOCATOR_THREADS false

namespace Anya
{
	// 第一级配置器 malloc_based allocator
	// 无模板形参，而非模板形参 inst 其实也没有被用上
	// 因为用的是malloc，所以一般是 thread safe 的
	template<int inst>
	class __malloc_alloc_template {
	private:
		// oom : out of memory
		// 以下的函数指针和函数将用来处理内存不足的情况
		static void* oom_malloc(size_t);
		static void* oom_realloc(void*, size_t);
		static void (*__malloc_alloc_oom_handler)();
	public:
		static void* allocate(size_t n) {
			void* result = malloc(n);
			if (result == nullptr) result = oom_malloc(n);
			return result;
		}

		static void* reallocate(void* p, size_t old_sz, size_t new_sz) {
			void* result = realloc(p, new_sz);                       // 第一级配置器直接调用 realloc
			if (result == nullptr) result = oom_realloc(p, new_sz);  // 如果失败了，改用 oom_realloc
			return result;
		}

		static void deallocate(void* p, size_t n) {
			free(p);   // 第一级配置器直接调用 free
		}

		// 以下模拟C++的 set_new_handler()
		// 之所以没有用set_new_handler()是因为第一级配置器使用的是 malloc 而不是::operator new
		// 用户可以指定自定义版本的 out-of-memory handler
		static auto set_malloc_handler(void (*f)()) -> void (*)() {
			void (*old)() = __malloc_alloc_oom_handler;
			__malloc_alloc_oom_handler = f;
			return old;
		}
	};

	// 初值为nullptr，由用户自定义
	template<int inst> 
	void (*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = nullptr;

	template<int inst>
	inline void* __malloc_alloc_template<inst>::oom_malloc(size_t n) {
		void (*my_malloc_handler)() = nullptr;
		void* result = nullptr;

		while (true) {
			my_malloc_handler = __malloc_alloc_oom_handler;
			if (my_malloc_handler == nullptr) { __THROW_BAD_ALLOC; }
			my_malloc_handler();    // 调用处理程序，尝试释放内存
			result = malloc(n);     // 再次尝试配置内存
			if (result) return result;
		}
		return result;
	}

	template<int inst>
	inline void* __malloc_alloc_template<inst>::oom_realloc(void* p, size_t n) {
		void (*my_malloc_handler)() = nullptr;
		void* result = nullptr;

		while (true) {
			my_malloc_handler = __malloc_alloc_oom_handler;
			if (my_malloc_handler == nullptr) { __THROW_BAD_ALLOC; }
			my_malloc_handler();        // 调用处理程序，尝试释放内存
			result = realloc(p, n);     // 再次尝试配置内存
			if (result) return result;
		}
		return result;
	}

	// 以下直接将参数 inst 指定为0
	using malloc_alloc = __malloc_alloc_template<0>;


	// 以下是第二级配置器，我们默认使用的alloc就是这个
	// 无模板形参，且第二参数也没有派上用场
	// 第一参数在 SGI STL 中是用于多线程环境下的，这里给与保留方便以后扩展，但在这一版中不考虑多线程环境
	template<bool threads, int inst>
	class __default_alloc_template {
	private:
		enum { __ALIGN = 8 };                            // 小型区块的对齐边界
		enum { __MAX__BYTES = 128 };                     // 小型区块的上限
		enum { __NFREELISTS = __MAX__BYTES / __ALIGN };  // free-list 个数
	private:
		// 将 bytes 上调至8的倍数
		constexpr static size_t ROUND_UP(size_t bytes) {
			return ((bytes + __ALIGN - 1) & ~(__ALIGN - 1));
		}
	private:
		union obj {
			union obj* free_list_link;   // free_list 的节点
			char  client_data[1];        // 用户看到的是这一个
		};
	private:
		// 16个 free_list
		static obj* volatile free_list[__NFREELISTS];
		// 根据区块大小，决定使用第n号free-list
		constexpr static size_t FREELIST_INDEX(size_t bytes) {
			return (bytes + __ALIGN - 1) / __ALIGN - 1;
		}
		// 返回一个大小为n的对象，在可能的情况下把大小为n的其他块加入到 free-list 中
		static void* refill(size_t n);
		// 配置一大块空间，可容纳 nobjs 个大小为 "size" 的区块
		// 如果空间不够，通过引用同步修改 nobjs 的大小
		static char* chunk_malloc(size_t size, int& nobjs);
	private:
		static char*  start_free;   // 内存池起始位置，只在 chunk_malloc 中变化
		static char*  end_free;     // 内存池结束位置，只在 chunk_malloc 中变化
		static size_t heap_size;
	public:
		static void* allocate(size_t n);
		static void* reallocate(void* p, size_t old_sz, size_t new_sz);
		static void  deallocate(void* p, size_t n);
	};

	// 初始化 static data member
	template<bool threads, int inst>
	char* __default_alloc_template<threads, inst>::start_free = nullptr;

	template<bool threads, int inst>
	char* __default_alloc_template<threads, inst>::end_free = nullptr;

	template<bool threads, int inst>
	size_t __default_alloc_template<threads, inst>::heap_size = 0;

	// 这里必须加一个typename来暗示obj是类型
	template<bool threads, int inst>
	typename __default_alloc_template<threads, inst>::obj* volatile
	__default_alloc_template<threads, inst>::free_list[__default_alloc_template<threads, inst>::__NFREELISTS] =
	{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	
	template<bool threads, int inst>
	inline void* __default_alloc_template<threads, inst>::allocate(size_t n) {
		obj* volatile* my_free_list = nullptr;
		obj* result = nullptr;

		// 大于128的大区块就直接调用第一级分配器
		if (n > __MAX__BYTES) {
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

	template<bool threads, int inst>
	inline void __default_alloc_template<threads, inst>::deallocate(void* p, size_t n) {
		obj* volatile* my_free_list = nullptr;
		obj* q = (obj*)p;

		// 大于128的块交由第一级配置器回收
		if (n > __MAX__BYTES) {
			malloc_alloc::deallocate(p, n);
			return;
		}
		// 小区块则回收至 free-list 
		my_free_list = free_list + FREELIST_INDEX(n);
		q->free_list_link = *my_free_list;
		*my_free_list = q;
	}

	template<bool threads, int inst>
	inline void* __default_alloc_template<threads, inst>::reallocate(void* p, size_t old_sz, size_t new_sz) {
		// 大于128的块则交由第一级配置器处理
		if (old_sz > (size_t)__MAX__BYTES && new_sz > (size_t)__MAX__BYTES) {
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

	template<bool threads, int inst>
	inline void* __default_alloc_template<threads, inst>::refill(size_t n) {
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

	// 由调用者保证size已经是8的倍数
	template<bool threads, int inst>
	inline char* __default_alloc_template<threads, inst>::chunk_malloc(size_t size, int& nobjs) {
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
				for (int i = size; i <= __MAX__BYTES; i += __ALIGN) {
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

#ifdef  __USE_MALLOC
using malloc_alloc = __malloc_alloc_template<0>;
using alloc = malloc_alloc;
#else 
using alloc = __default_alloc_template<__NODE_ALLOCATOR_THREADS, 0>;
#endif 

	template<class T, class Alloc = alloc>
	class allocator {
	public:
		static T* allocate(size_t n) { 
			return n == 0 ? nullptr : (T*)Alloc::allocate(n * sizeof(T));
		}
		static T* allocate() { 
			return (T*)Alloc::allocate(sizeof(T));
		}
		static void deallocate(T* p, size_t n) { 
			if (n != 0) Alloc::deallocate(p, n * sizeof(T));
		}
		static void deallocate(T* p) {
			Alloc::deallocate(p, sizeof(T));
		}
	};

}

#endif