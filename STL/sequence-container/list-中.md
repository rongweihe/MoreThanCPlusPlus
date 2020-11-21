# list（中篇-基本结构， 构造和析构函数）

## 前言

上节分析了 `list` 的类型，构造析构的实现，本节我们着重探讨 `list` 的 push，pop 插入和删除等基本操作。

## list 实例

与上节一样，我们将待会会用到的部分常用的操作先执行一次，进行一次快速的回忆。

```c++
void ListTest() {
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
```

 输出

```c++
555 444 222 111 333
L2 size = 5
555 444 222 111 333
```

### push 和 pop 操作

因为 `list` 是一个循环的双链表， 所以 `push` 和 `pop` 就必须实现是在头/尾插入，删除是在头/尾删除。

`push` 操作都调用 `insert` 函数， `pop` 操作都调用 `erase` 函数。

```c++
template <class T, class Alloc = alloc>
class list {
    ...
    // 直接在头部或尾部插入
    void push_front(const T& x) { insert(begin(), x); }
    void push_back(const T& x) { insert(end(), x); }
    // 直接在头部或尾部删除
    void pop_front() { erase(begin()); }
    //end指向的是最后一个元素的下一个位置
    void pop_back() { 
      iterator tmp = end();
      erase(--tmp);
    }
    ...
};
```

### 插入操作

`insert`  函数有很多的重载函数， 满足足够用户的各种插入方法了，但是最核心的还是 `iterator insert(iterator position, const T& x)`，每一个重载函数都是直接或间接的调用该函数。

```c++
class list {
public:
    // 最基本的insert操作, 在 A node 之间之插入一个元素 B
    iterator insert(iterator position, const T& x)  {
        // 将元素插入指定位置的前一个地址
        //先拉进后面的关系 在往前攀岩 最后笼络人心
      link_type B = create_node(x);
      B->next = position.node;
      B->prev = position.node->prev; //相当于 B 指向 A
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
```

### 删除操作

删除元素的操作大都是由 `erase` 函数来实现的, 其他的所有函数都是直接或间接调用 `erase`. `list `是链表, 所以链表怎么实现删除，`list`  就 怎么操作。

```c++
template <class T, class Alloc = alloc>
class list {
    ...
	iterator erase(iterator first, iterator last);
    void clear();   
    // 参数是一个迭代器
    // 修改该元素的前后指针指向再单独释放节点就行了
	iterator erase(iterator position) {
      link_type next_node = link_type(position.node->next);
      link_type prev_node = link_type(position.node->prev);
      prev_node->next = next_node;
      next_node->prev = prev_node;
      destroy_node(position.node);
      return iterator(next_node);
    }
    ...
};
// erase 的重载， 删除两个迭代器之间的元素
template<class T, class Alloc>
list<T, Alloc>::iterator list<T, Alloc>::erase(iterator first, iterator last) {
  //就是一次次调用上面的 erase 
  while(first != last) {
    erase(first++);
    return last;
  }
}
// remove 调用 erase 链表清除
template <class T, class Alloc>
void list<T, Alloc>::remove(const T& value) {
  iterator first = begin();
  iterator last = end();
  while (first != last) {
    iterator next = first;
    ++next;
    if (*first == value) erase(first);
    first = next;
  }
}
//clear 是删除除空节点以外的所有节点 只留下了最初创建的空节点
template<class T, class Alloc> 
void list<T, Alloc>::clear() {
  link_type cur = (link_type)node->next;
  //除空节点都删除
  while(cur != node) {
    link_type tmp = cur;
    cur = (link_type)cur->next;
    destroy_node(tmp);
  }
  node->next = node;
  node->prev = node;
}
```

### 重载

`list` 也提供了基本操作的重载, 所以我们使用 `list` 也很方便.

相等比较

```c++
// 判断两个list相等
template <class T, class Alloc>
inline bool operator==(const list<T,Alloc>& x, const list<T,Alloc>& y) {
  typedef typename list<T,Alloc>::link_type link_type;
  link_type e1 = x.node;
  link_type e2 = y.node;
  link_type n1 = (link_type) e1->next;
  link_type n2 = (link_type) e2->next;
  // 将两个链表执行一一的对比来分析是否相等. 
  // 这里不把元素个数进行一次比较, 主要获取个数时也要遍历整个数组, 所以就不将个数纳入比较
  for ( ; n1 != e1 && n2 != e2 ; n1 = (link_type) n1->next, n2 = (link_type) n2->next)
    if (n1->data != n2->data)
      return false;
  return n1 == e1 && n2 == e2;
}
```

### 赋值操作

需要考虑的是，两个链表的实际大小不一致的情况。

1、如果原链表长：复制新链表之后要删除原链表多余的元素。

2、如果原链表短：复制完新链表之后要将新链表的剩余元素以插入的方式插入到原链表当中。

```c++
template<class T, class Alloc>
list<T, Alloc>& list<T, Alloc>::operator = (const list<T, Alloc>& x) {
	if (this != &x) {
		iterator first1 = begin();
    iterator last1  = end();
    const_iterator first2 = x.begin();
    const_iterator last2 = x.end();
    //直到两个链表有一个走到尾
    while(first1 != last1 && first2 != last2) 
      *first1++ = *first2++;
    if (first2 == last2) { //	原链表长：复制新链表之后要删除原链表多余的元素。
      erase(first1, last1);
    } else {            //原链表短：复制完新链表之后要将新链表的剩余元素以插入的方式插入到原链表当中。
      insert(last1, first2, last2);
    }
	}
}
```

### resize操作

`resize` 重新修改 `list` 的大小。

```c++
template <class T, class Alloc = alloc>
class list  {
    ...
    resize(size_type new_size, const T& x);
	void resize(size_type new_size) { resize(new_size, T()); }
    ...
}; 
template <class T, class Alloc>
void list<T, Alloc>::resize(size_type new_size, const T& x) {
  iterator i = begin();
  size_type len = 0;
  for ( ; i != end() && len < new_size; ++i, ++len);
  // 如果链表长度大于new_size的大小, 那就删除后面多余的节点
  if (len == new_size)
    erase(i, end());
    // i == end(), 扩大链表的节点
  else                          
    insert(end(), new_size - len, x);
}
```

### unique 操作

`unique ` 函数是将数值相同且连续的元素删除， 只保留一个副本.

 记住， `unique` 并不是删除所有的相同元素， 而是连续的相同元素，如果要删除所有相同元素就要对 `list`  做一个排序在进行 `unique` 操作。

一般 `unique`  同 `sort` 一起用的，`sort`函数准备放在下一节来分析。

```c++
template <class T, class Alloc> template <class BinaryPredicate>
void list<T, Alloc>::unique(BinaryPredicate binary_pred) {
  iterator first = begin();
  iterator last = end();
  if (first == last) return;
  iterator next = first;
  // 删除连续相同的元素, 留一个副本
  while (++next != last) {
    if (binary_pred(*first, *next))
      erase(next);
    else
      first = next;
    next = first;
  }
}
```

## 总结

本节分析了`list`的插入， 删除，重载等操作，这些都是链表的基本操作， 相信大家看的时候应该也没有什么问题，最难的部分--`sort` 等操作放在下一节来分析。

这里还是提醒一下:

1. **节点实际是以`node`空节点开始的**
2. **插入操作是将元素插入到指定位置的前一个地址进行插入的.**

