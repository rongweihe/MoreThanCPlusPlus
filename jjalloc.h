#ifndef _JJALLOC_
#define _JJALLOC_

//通过设计一个简单的空间配置器 JJ::allocator 来看一下 空间配置器的 标准接口
allocator::value_type

#include <new>
#include <cstdlib>
#include <cstddef>
#include <climits>
#include <iostream>
#include <algorithm>
namespace JJ {

template <class T>
inline T* _allocate(ptrdiff_t size, T*) {
	set_new_handler(0);
	T* tmp = (T*)(::operator new((size_t)(size * sizeof(T)))); // 分配内存 底层调用 C API 的 malloc 函数
	if (tmp == 0) {
		cerr << "out of memory" << endl;
		exit(1);
	}
	return tmp;
}

template <class T>
inline void _deallocate(T* buffer) {
	::operator delete(buffer); // 释放内存 底层调用 C API 的 free 函数
}

template <class T1, class T2>
inline void _construct(T1* p, const T2& value) {
	new(p) T1(value);         //构造函数
}

template <class T>
inline void _destroy(T* ptr) {
	ptr->~T();               //析构函数
}

template <class T>
class allocator {
public:
	typedef T 			value_type;
	typedef T* 			pointer;
	typedef const T*	const_pointer;
	typedef T&			reference;
	typedef const T&	const_reference;
	typedef size_t		size_type;
	typedef ptrdiff_t 	difference_type;

	// rebind allocator of type U
	template <class U>
	struct rebind {
		typedef allocator<U> other;
	};

	pointer allocator(size_type n, const void* hint = 0) {
		return _allocate((difference_type)n, (pointer)0);
	}
	//配置空间 存储 n 个 T 对象 第二个参数是个提示可以忽略

	void deallocate(pointer p, size_type n)   {		_deallocate(p); 	}
	//释放配置的空间

	void construct(pointer p, const T& value) {		_construct(p, value_type);	}
	//构造 等同于 new((void*) p) T(x)

	void destroy(pointer p) {	_destroy(p);	}
	//析构 等同于 p->~T()

	pointer address(reference x) {	return (pointer)&x;	}
	//返回某个对象的地址

	const_pointer const_address(reference x) {	return (pointer)&x;	}
	//返回某个 const 对象的地址

	size_type max_size() const {
		return size_type(UINT_MAX/sizeof(T));
	}
	//返回可成功配置的最大量
};

}

#endif