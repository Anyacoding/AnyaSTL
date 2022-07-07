#ifndef _ANYA_CONSTRUCT_
#define _ANYA_CONSTRUCT_
#include <new>

namespace Anya
{
	// class 没有任何成员，不会带来额外负担，却又能够标示真假。
	// 从traits挖过来的，先放在这里, 目前没有任何卵用
	struct __true_type {
	};

	struct __false_type {
	};

	// 将初值 value 设定到指针所指的空间上。
	template<class T1, class T2>
	inline void construct(T1* ptr, const T2& value) {
		new(ptr) T1(value);
	}

	// 以下是 destroy() 第一版本，接受一个指针并将所指之物析构掉
	template<class T>
	inline void destroy(T* ptr) {
		ptr->~T();
	}

	// 以下是 destroy() 第二版本，接受两个前向迭代器。
	// 此函数设法找出元素的数值型别，进而利用 __type_traits<> 求取最适当措施
	template<class ForwardIterator>
	inline void destroy(ForwardIterator first, ForwardIterator last) {
		__destroy(first, last, value_type(first));
	}

	// 如果元素的数值型别（value type）有 trivial destructor. no-op
	template<class ForwardIterator>
	inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __true_type) {}

	// 如果元素的数值型别（value type）有 non-trivial destructor
	template<class ForwardIterator>
	inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
		for (; first != last; ++first) {
			destroy(&*first);
		}
	}

	// 判断元素的数值型别（value type）是否有 trivial destructor
	template<class ForwardIterator, class T>
	inline void __destroy(ForwardIterator first, ForwardIterator last, T*) {
		using trivial_destructor = typename __type_traits<T>::has_trivial_destructor;
		__destroy_aux(first, last, trivial_destructor());
	}

	// 以下是 destroy() 第二版针对迭代器为 char* 和 wchar_t* 的特化版. no-op
	template<> inline void destroy(char*, char*) {};
	template<> inline void destroy(wchar_t*, wchar_t*) {};
}


#endif
