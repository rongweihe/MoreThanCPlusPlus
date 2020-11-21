# list（上篇-基本结构， 构造和析构函数）

## 前言

前几节我们分析了 `vector` 的实现， `vector` 的和 `list` 对比如下：

> 1、 `vector`  封装了数组，使用连续内存 ，对于随机访问速度很快，但是对于插入和删除操作很慢（尤其是头部插入），在尾部插入很快。
>
> 2、`list`  封装了链表，链表对插入，删除的操作效率比较高,，但是随机访问的速度很慢。

在 `SGI STL`  源码里面， `list`  将具体的实现分成了几个部分，通过嵌套的方式进行调用， 所以 list 实现也很灵活，而且 **`list` 在插入和删除操作后迭代器并不会失效。**

本节只分析关于 `list` 的结构， 构造和析构函数的实现。

## list 基本结构框架

`list`  底层源码框架分成了两部分 一部分是 `__list_node` (实现节点)，一部分是 `__list_iterator` (实现迭代器) 两部分方便随时调用。

### __list_node 链表结构

`__list_node ` 用来实现节点，数据结构中储存前后指针和节点的属性。

```c++
template <class T>
struct __list_node {
    typedef void* void_pointer;
    void_pointer prev;//前后指针 
    void_pointer next;
    T data;//属性值
};
```

### __list_iterator 结构

**基本类型**

```c++
template <class T, class Ref, class Ptr>
struct __list_iterator {
    typedef __list_iterator<T, T&, T*> iterator;//1
    typedef __list_iterator<T, Ref, Ptr> self;//2
    typedef bidirectional_iterator_tag iterator_category;  // list 的迭代器是 双向移动迭代器 bidirectional_iterator_tag
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef __list_node<T>* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
  ...
};
```

**思考：**

为什么迭代器里面有（1，2）这么一大坨的东西，原因在于和迭代器的萃取机制有关，对于一个具体算法的应用和提问，迭代器需要回答算法的问题，上面这些就是迭代器提供给算法的回答。

**构造函数**

```c++
template<class T, class Ref, class Ptr> 
struct __list_iterator {
    ...
    // 定义节点指针
  	typedef __list_node<T>* link_type;
  	link_type node;
	  // 构造函数
  	__list_iterator(link_type x) : node(x) {}
  	__list_iterator() {}
  	__list_iterator(const iterator& x) : node(x.node) {}
   ... 
};
```

**重载**

```c++
template<class T, class Ref, class Ptr>
struct __list_iterator {
    ...
	// 重载
  	bool operator==(const self& x) const { return node == x.node; }
  	bool operator!=(const self& x) const { return node != x.node; }
    // 对*和->操作符进行重载
  	reference operator*() const { return (*node).data; }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
  	pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */
    // ++和--是直接操作的指针指向next还是prev, 因为list是一个双向链表
  	self& operator++()  { 
	    node = (link_type)((*node).next);
	    return *this;
  	}
  	self operator++(int)  { 
	    self tmp = *this;
	    ++*this;
	    return tmp;
  	}
  	self& operator--()  { 
	    node = (link_type)((*node).prev);
	    return *this;
  	}
  	self operator--(int)  { 
    	self tmp = *this;
    	--*this;
    	return tmp;
  	}
};
```

## list 结构

**list 基本类型定义**

`list ` 不仅是一个双向链表，而且还是一个环状双向链表，所以它只需要一个指针 ，便可以完整实现整个链表。

如果让指针 `node`  指向刻意置于尾端的一个空白节点 `node` 便能符合 `STL` 对于前闭后开区间的要求， 成为 一个 last 迭代器。

**`list ` 在定义 node 节点时，定义的不是一个指针， 这里要注意。**

```c++
template <class T, class Alloc = alloc>
class list {
protected:
    typedef void* void_pointer;
    typedef __list_node<T> list_node;	// 节点
    typedef simple_alloc<list_node, Alloc> list_node_allocator;	// 空间配置器
public:      
    // 定义嵌套类型
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef list_node* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    
protected:
    // 定义一个节点, 这里节点并不是一个指针.
    link_type node;
    bool empty() const {
         return node->next == node;
     }

    size_type size() const {
      size_type result = 0;
      distance(begin(), end(), result); // 全局函数
      return reault;
    }
    //取头结点的内容(元素值)
    reference front() { return *begin();    }
    reference back() {  return *(--end());  }
    
public:
    // 定义迭代器
    typedef __list_iterator<T, T&, T*>             iterator;
    typedef __list_iterator<T, const T&, const T*> const_iterator;
	...
};
```

**list 构造和析构函数实现**

构造函数前期准备

1. 调用 `get_node` 分配空间。
2. 调用 `put_node` 释放空间 。
3. 调用 `create_node` 分配并构造 。
4. 调用 `destroy_node` 析构并释放空间。
5. 调用 `empty_initialize` 对节点进行初始化。

```c++
template <class T, class Alloc = alloc>
class list {
    ...
protected:
	// 分配一个元素大小的空间, 返回分配的地址
    link_type get_node() { return list_node_allocator::allocate(); }
    // 释放一个元素大小的内存
    void put_node(link_type p) { list_node_allocator::deallocate(p); }
	// 分配一个元素大小的空间并调用构造初始化内存
    link_type create_node(const T& x) {
      link_type p = get_node();
      __STL_TRY {
        construct(&p->data, x);
      }
      __STL_UNWIND(put_node(p));
      return p;
    }
    // 调用析构并释放一个元素大小的空间
    void destroy_node(link_type p) {
      destroy(&p->data);
      put_node(p);
    }
    // 对节点初始化
    void empty_initialize() { 
      node = get_node();
      node->next = node;
      node->prev = node;
    }  
    ...
};
```

**构造函数**

1、多个重载， 以实现直接构造 n 个节点并初始化一个值 ，支持传入迭代器进行范围初始化， 也支持接收一个 list 参数进行范围初始化。

2、每个构造函数都会创造一个空的 node 节点， 为了保证我们在执行任何操作都不会修改迭代器。

```c++
template <class T, class Alloc = alloc>
class list {
    ...
protected: 
    // 构造函数
    list() { empty_initialize(); }	// 默认构造函数, 分配一个空的node节点
    // 都调用同一个函数进行初始化
    list(size_type n, const T& value) { fill_initialize(n, value); }
    list(int n, const T& value) { fill_initialize(n, value); }
    list(long n, const T& value) { fill_initialize(n, value); }
    // 分配n个节点
    explicit list(size_type n) { fill_initialize(n, T()); }

#ifdef __STL_MEMBER_TEMPLATES
    // 接受两个迭代器进行范围的初始化
    template <class InputIterator>
      list(InputIterator first, InputIterator last) {
        range_initialize(first, last);
      }
#else /* __STL_MEMBER_TEMPLATES */
    // 接受两个迭代器进行范围的初始化
    list(const T* first, const T* last) { range_initialize(first, last); }
    list(const_iterator first, const_iterator last) {
      range_initialize(first, last);
    }
#endif /* __STL_MEMBER_TEMPLATES */
    // 接受一个list参数, 进行拷贝
    list(const list<T, Alloc>& x) {
      range_initialize(x.begin(), x.end());
    }
    list<T, Alloc>& operator=(const list<T, Alloc>& x);
    ...
};
```

构造函数内调用这个 `fill_initialize`  函数, 可以看出来 `list` 在初始化的时候都会**构造一个空的`node`节点**, 然后对元素进行 `insert` 插入操作。

**析构函数** ，释放所有的节点空间，包括最初的空节点。

```c++
~list() {
  // 删除初空节点以外的所有节点
  clear();
  // 删除空节点
  put_node(node);
}
```

## 基本属性获取

要注意一点`list`中的迭代器一般不会被修改, 因为`node`节点始终指向的一个空节点同时`list`是一个循环的链表, 空节点正好在头和尾的中间, 所以`node.next`就是指向头的指针, `node.prev`就是指向结束的指针, `end`返回的是最后一个数据的后一个地址也就是`node`. 清楚这些后就容易看懂下面怎么获取属性了.

```c++
template<class T, class Alloc = alloc> 
class list {
    ...
public:
    iterator begin() { return (link_type)((*node).next); }	// 返回指向头的指针
    const_iterator begin() const { return (link_type)((*node).next); }
    iterator end() { return node; }	// 返回最后一个元素的后一个的地址
    const_iterator end() const { return node; }

    // 判断是否为空链表, 这是判断只有一个空node来表示链表为空.
    bool empty() const { return node->next == node; }
    // 因为这个链表, 地址并不连续, 所以要自己迭代计算链表的长度.
    size_type size() const {
      size_type result = 0;
      distance(begin(), end(), result);
      return result;
    }
    size_type max_size() const { return size_type(-1); } //TODO
    // 返回第一个元素的值
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    // 返回最后一个元素的值
    reference back() { return *(--end()); }
    const_reference back() const { return *(--end()); }
 // 交换
    void swap(list<T, Alloc>& x) { __STD::swap(node, x.node); }
    ...
};
template <class T, class Alloc>
inline void swap(list<T, Alloc>& x, list<T, Alloc>& y) {
  	x.swap(y);
}
```

