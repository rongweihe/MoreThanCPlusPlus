# deque（上篇-基本结构）

## 前言

deque 的功能很强大，其复杂度也比 list， vector 复杂很多， deque 是一个 random_access_iterator_tag 类型。

前面分析过 vector 是保存在连续的线性空间，头插入和删除其代价也很大，当数组满了还要重新寻找更大的空间；

deque 也是一个保存在连续的线性空间中，但是它是一个双向开口，头尾插入和删除都是 O(1) 的时间复杂度，空间也是可扩展的。

deque 的内存操作主要是一个 map 来实现的。

> 1、
>
> 2、

## __deque_iterator 迭代器结构

虽然 deque 在某些方面和 vector 有些相似，但是迭代器并不是一个普通指针， deque 的迭代器很复杂，现在我们就来分析一下。

#### 全局函数

```c++
inline size_t __deque_buf_size(size_t n, size_t sz) {
  return n != 0 ? n : (sz < 512 ? size_t( 512 / sz ) : size_t(1) );
}
```

#### 类型定义

`deque`是`random_access_iterator_tag`类型，满足`traits`编程。

这里重点分析四个参数 cur， first， last 以及`node`。

```c++
#ifndef __STL_NON_TYPE_TMPL_PARAM_BUG
template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
	// 迭代器定义
  typedef __deque_iterator<T, T&, T*, BufSiz>             iterator;
  typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;
  static size_t buffer_size() {return __deque_buf_size(BufSiz, sizeof(T)); }
#else /* __STL_NON_TYPE_TMPL_PARAM_BUG */
template <class T, class Ref, class Ptr>
struct __deque_iterator {
  typedef __deque_iterator<T, T&, T*>             iterator;
  typedef __deque_iterator<T, const T&, const T*> const_iterator;
  static size_t buffer_size() {return __deque_buf_size(0, sizeof(T)); }
#endif
	// deque 是 random_access_iterator_tag 类型
  typedef random_access_iterator_tag iterator_category;
  // 基本类型的定义, 满足traits编程
  typedef T value_type;
  typedef Ptr pointer;
  typedef Ref reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  // node
  typedef T** map_pointer;
  map_pointer node;
	
  typedef __deque_iterator self;
  ...
};
```

```c++
// 满足traits编程
template <class T, class Ref, class Ptr, size_t BufSiz>
inline random_access_iterator_tag
iterator_category(const __deque_iterator<T, Ref, Ptr, BufSiz>&) {
  return random_access_iterator_tag();
}
template <class T, class Ref, class Ptr, size_t BufSiz>
inline T* value_type(const __deque_iterator<T, Ref, Ptr, BufSiz>&) {
  return 0;
}
template <class T, class Ref, class Ptr, size_t BufSiz>
inline ptrdiff_t* distance_type(const __deque_iterator<T, Ref, Ptr, BufSiz>&) {
  return 0;
}
```

`cur`, `first`, `last`这三个变量类似于 vector 中的 3 个迭代器一样.

- cur : 当前所指的位置
- first : 当前数组中头的位置
- last : 当前数组中尾的位置

因为 deque 的空间是由 map 管理，是一个指向指针的指针，所以三个参数都是指向当前的数组。这样的数组可能有多个，只是每个数组都需要管理这 3 个变量。

```c++
template<class T, class Ref, class Ptr, size_t BufSiz> 
struct __deque_iterator {
	//...
	typedef T value_type;
	T* cur;
	T* first;
	T* last;
	//...
}
```

每一个 数组，都有一个 node 指针，它是用来指向 *map 的指针

```c++
template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
	//...
	typedef T** map_pointer;
  map_pointer node;
	//...
}
```

构造函数

```c++
template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
	...
  	// 初始化cur指向当前数组位置, last指针数组的尾, node指向y
  	__deque_iterator(T* x, map_pointer y)  : cur(x), first(*y), last(*y + buffer_size()), node(y) {}
  	// 初始化为一个空的deque
  	__deque_iterator() : cur(0), first(0), last(0), node(0) {}
  	// 接受一个迭代器
  	__deque_iterator(const iterator& x) : cur(x.cur), first(x.first), last(x.last), node(x.node) {}
    ...
};
```

重载

`__deque_iterator`实现了基本运算符, `deque`重载的运算符操作都是调用`__deque_iterator`的运算符。

不过先分析一下待会会用到的函数 set_node;

因为 node 是一个指向 *map 的指针， 当数组填充满了之后，要重新指向下一个数组的头，set_node 就是更新指向数组的头的功能。

```c++
template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
	...
	void set_node(map_pointer new_node)	{
		// 让node指针另一个数组的头, 同时修改头和尾的地址
    	node = new_node;
    	first = *new_node;
    	last = first + difference_type(buffer_size());
  	}
  	...
};
```

**重载++和--**

需要注意++和--都可能出现数组越界, 如果判断要越界就得更新`node`的指向。

```c++
template <class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
	...  
 	// 这里需要先加之后判断是否达到当前数组的尾部
  self& operator++() {
    ++cur;
    // 达到了尾部就需要更新node的指向
    if (cur == last) {
      set_node(node + 1);
      cur = first;
    }
    return *this; 
  }
  // 同理, 需要判断是否到达数组的头，先判断在加 到达就要更新node指向
  self& operator--() {
    if (cur == first) {
      set_node(node - 1);
      cur = last;
    }
    --cur;
    return *this;
  }
  
  self operator++(int)  {
    self tmp = *this;
    ++*this;
    return tmp;
  }
  self operator--(int) {
    self tmp = *this;
    --*this;
    return tmp;
  }
  ...
};
```

