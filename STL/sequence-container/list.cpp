/*
===========================前言==============================

【1】 相比较于 vector 的线性空间 list就显得复杂许多 它的好处是每次插入或删除一个元素就配置或释放一个元素空间

【2】list 对于空间的运用绝对的精准 一点也不浪费 
对于任何位置的元素插入或者元素移除都是常数时间

【3】所有的容器除了 vector和array的迭代器都必须是一个class 

【4】list 有一个重要特性：插入操作和结合操作(splice)都不会造成原有的List迭代器失效 这在 vector中是不会成立的
因为vector的插入操作可能造成记忆体重新配置 导致原有的迭代器全部失效。甚至 list 的元素删除操作也只有 指向被删除元素 的那个
迭代器失效，其它迭代器不受影响。

前言
前几节我们分析了vector的实现, vector的缺点也很明显, 在频率较高的插入和删除时效率就太低了, 本节我们就来分析在频率较高的插入和删除很也很好的效率的list.

list是用链表进行实现的, 而链表对删除, 插入的时间复杂度为O(1), 效率相当高, 但是随机访问的时间复杂度为O(n). list将具体实现分成几个部分, 通过嵌套的方式进行调用, 所以list实现也很灵活. 而且**list在插入和删除操作后迭代器并不会失效.**

本节只分析关于list的结构, 构造和析构函数的实现.
*/

/*
/*===================list基本结构框架=====================
list 底层源码框架分成了两部分 一部分是 __list_node(实现节点), 一部分是 __list_iterator(实现迭代器) 两部分方便随时调用 

*/
#include <list>
#include <algorithm>
#include <iostream>
using namespace std;


/*

==============__list_node链表结构=======================
__list_node用来实现节点, 数据结构中就储存前后指针和属性.
*/
template <class T>
struct __list_node {
    typedef void* void_pointer;
    void_pointer prev;//前后指针 
    void_pointer next;
    T data;//属性值
};

/*
============__list_iterator结构 基本类型================
*/

template <class T, class Ref, class Ptr>
struct __list_iterator {
    typedef __list_iterator<T, T&, T*> iterator;
    typedef __list_iterator<T, Ref, Ptr> self;
    
    /*为什么迭代器里面有这么一大坨的东西 这个和迭代器的萃取机制有关 对于一个具体算法的应用和提问，迭代器需要回答算法的问题，下面这些就是迭代器提供给算法的回答*/
    typedef bidirectional_iterator_tag iterator_category;  // list 的迭代器是 双向移动迭代器 bidirectional_iterator_tag
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef __list_node<T>* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
};

//==============构造函数
template<class T, class Ref, class Ptr>
struct __list_iterator 
{
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

//===============重载
template<class T, class Ref, class Ptr>
struct __list_iterator 
{
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
  	self& operator++() 
    { 
	    node = (link_type)((*node).next);
	    return *this;
  	}
  	self operator++(int) 
    { 
	    self tmp = *this;
	    ++*this;
	    return tmp;
  	}
  	self& operator--() 
    { 
	    node = (link_type)((*node).prev);
	    return *this;
  	}
  	self operator--(int) 
    { 
    	self tmp = *this;
    	--*this;
    	return tmp;
  	}
};
/*
SGI list 不仅是一个双向链表 而且还是一个环状双向链表 所以它只需要一个指针 便可以完整实现整个链表
如果让指针 node 指向刻意置于尾端的一个空白节点 node 便能符合 STL 对于 前闭后开区间的要求 成为 一个 last 迭代器。
list 自己定义了嵌套类型满足 traits 编程. 在定义 node 节点时, 定义的不是一个指针, 这里要注意.
*/

template <class T, class Alloc = alloc>
class list {
protected:
    typedef __list_node<T> list_node;
    typedef void* void_pointer;
    typedef simple_alloc<list_node, Alloc> list_node_allocator; //空间配置器
    
public:
    typedef list_node* link_type;
    typedef __list_iterator<T, T&, T*> iterator;//定义迭代器

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
    link_type node;//定义一个节点
     
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
};

//==============================list构造和析构函数实现
/*
构造函数前期准备
分配空间get_node
释放空间put_node
分配并构造create_node
析构并释放空间destroy_node
对节点进行初始化empty_initialize
*/
template <class T, class Alloc = alloc>
class list 
{
    //...
protected:
    //分配一个元素大小的空间 返回分配的地址
    link_type get_node() {  return list_node_allocator::allocate(); }

    //释放一个元素大小的内存
    void put_node(link_type p) {  list_node_allocator::deallocate(p); }

    //分配一个元素大小的空间并调用构造初始化内存
    link_type create_node(const T& x) {
        link_type p = get_node();
        __STL_TRY {
            construct(&p->data, x);
        }
        __STL_UNWIND(put_node(p));
        return p;
    }

    //调用析构函数并释放一个元素大小的空间
    void destroy_node(link_type p) {
        destroy(&p->data);
        put_node(p);
    }
    //对节点初始化
    void empty_initialize() { 
      node = get_node();
      node->next = node;
      node->prev = node;
    }  
}

/*
构造函数
1、多个重载， 以实现直接构造 n 个节点并初始化一个值 支持传入迭代器进行范围初始化 也支持接收一个 list 参数 进行范围初始化
2、每个构造函数都会创造一个空的 node 节点 为了保证我们在执行任何操作都不会修改迭代器
*/

template <class T, class Alloc = alloc>
class list {
    ...
protected:
    //构造函数
    list() {    empty_initialize(); } //默认构造函数 分配一个空的 node 节点
    //调用同一个函数进行初始化
    list(size_type n, const T& value) { fill_initialize(n, value);  }
    list(int n, const T& value) {   fill_initialize(n, value);  }
    list(long n, const T& value) {   fill_initialize(n, value);  }

    //在C++中，explicit关键字用来修饰类的构造函数，被修饰的构造函数的类，不能发生相应的隐式类型转换，只能以显示的方式进行类型转换。
    explicit list(size_type n) {    fill_initialize(n, T());    }

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
    //接受一个 list 参数 进行拷贝
    list(const list<T, Alloc>& x) {
        range_initialize(x.begin(), x.end());
    }
    list<T, Alloc>& operator=(const list<T, Alloc>& x);
    ...
}

/*
构造函数内部都会调用这个函数 可以看出来 list 在初始化的时候都会够着一个空的 node 节点然后对元素进行 insert 操作
*/
void fill_initialize(size_type n, const T& value) {
    empty_initialize();
    __STL_TRY {
        insert(begin(), n, value);
    }
    __STL_UNWIND(clear(); put_node(node));
}

/*
析构函数 , 释放所有的节点空间. 包括最初的空节点.
*/

~list() {
        // 删除初空节点以外的所有节点
      clear();
        // 删除空节点
      put_node(node);
}

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
inline void swap(list<T, Alloc>& x, list<T, Alloc>& y) 
{
  	x.swap(y);
}

//=========================中

/*
前言
上节分析了list的类型, 构造析构的实现, 本节我们着重探讨list的push, pop, 插入和删除等基本操作.
*/
void test20() {
    std::list<int> L;
    L.push_back(111);
    L.push_front(222);
    L.push_back(333);
    L.push_front(444);
    L.insert(L.begin(), 555);	// list没有重载 + 运算符, 所以不能直接操作迭代器
    for (auto&e : L) {
        std::cout<<e<<" ";
    }
    std::cout<<endl;
    list<int> L1(L);
    list<int> L2 = L1;
    cout << "L2 size = " << L2.size() << endl;
    for (auto&e : L2) {
        std::cout<<e<<" ";
    }
}

// 555 444 222 111 333 
// L2 size = 5
// 555 444 222 111 333 

/*
list操作
push和pop操作=====================================
因为list是一个循环的双链表, 所以push和pop就必须实现是在头插入, 删除还是在尾插入和删除. push操作都调用insert函数, pop操作都调用erase函数.
*/

template<class T, class Alloc = alloc> 
class list {
    ...
    //直接在头部或尾部插入
    void push_front(const T& x) {   insert(begin(), x);  }
    void push_back(const T& x) {    insert(end(), x);   }
    //直接在头部或尾部删除
    void pop_front() {  erase(begin());    }
    void pop_back() {   iterator tmp = end(); erase(--tmp); }
    //end指向的是最后一个元素的下一个位置
    ...
}

/*
insert操作=======================================
insert函数有很多的重载函数, 满足足够用户的各种插入方法了. 但是最核心的还是iterator insert(iterator position, const T& x), 每一个重载函数都是直接或间接的调用该函数.

insert是将元素插入到指定地址的前一个位置.
*/

template <class T, class Alloc = alloc>
class list {
public:
    // 最基本的insert操作, 在 A C 之间之插入一个元素 B
    iterator insert(iterator position, const T& x)  {
        // 将元素插入指定位置的前一个地址
        //先拉进后面的关系 在往前攀岩 最后笼络人心
      link_type B = create_node(x);
      B->next = position.node;
      B->prev = position.node->prev;
      (link_type(position.node->prev))->next = B;
      position.node->prev = B;
      return B;
    }
  // 以下重载函数都是调用iterator insert(iterator position, const T& x)函数
   iterator insert(iterator position) { return insert(position, T()); }
#ifdef __STL_MEMBER_TEMPLATES
    template <class InputIterator>
      void insert(iterator position, InputIterator first, InputIterator last);
#else /* __STL_MEMBER_TEMPLATES */
    void insert(iterator position, const T* first, const T* last);
    void insert(iterator position,
        const_iterator first, const_iterator last);
#endif /* __STL_MEMBER_TEMPLATES */
    void insert(iterator pos, size_type n, const T& x);
    void insert(iterator pos, int n, const T& x) {
      insert(pos, (size_type)n, x);
    }
    void insert(iterator pos, long n, const T& x) {
      insert(pos, (size_type)n, x);
    }
    void resize(size_type new_size, const T& x);
    ...
};

/*
删除操作=========================================
删除元素的操作大都是由erase函数来实现的, 其他的所有函数都是直接或间接调用erase. list是链表, 所以链表怎么实现删除, list就在怎么操作.
*/
template <class T, class Alloc = alloc>
class list {
    ...
    iterator erase(iterator first, iterator last);
    void clear();
}