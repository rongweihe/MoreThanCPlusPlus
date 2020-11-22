# list（下篇-sort 源码分析）

## 前言

前两节对 `list` 的 push，pop， insert 等操作做了分析, 本节准备探讨 `list` 怎么实现 `sort` 功能。

 `list`  的 sort 算法实现

## 相关函数实现

在 分析 `sort` 之前先来分析 `transfer` ， `reverse`，`merge` 这几个会被调用的函数。

### Transfer 函数

此函数功能是将一段链表插入到我们指定的位置之前，提供一个迁移的功能。这个函数的实现原理最好搞明白，其实学过链表的话，搞明白它不难，都是指针的指向而已。后面分析的所有函数都是该基础上进行修改的。

输入：该函数接受 3 个迭代器，第一个 `position`  表示要插入的位置，第二个和第三个表示将  [first, last)  内的所有元素移到  `position`  之前。

下面就先用一张图来看看它的逻辑。

![list-transfer.png](https://i.loli.net/2020/11/22/SwR9sgNhQOoKa6T.jpg)

其中 A 节点代表 迁移之后 `first` 节点的前一个节点。

B 节点代表迁移之后 ` last` 节点的前一个节点。

C 节点代表迁移之后 `position`  节点的前一个节点。

> （1）将 C 的 next 指向插入的 position 节点。
>
> （2）将 B 的 next 指向 last 节点。
>
> （3）将 A 的 next 指向 first 节点。
>
> （4）将 position 的前一个节点赋值给 A（保留该节点的状态第五步会改变它的指向）。
>
> （5）将 position 的的前一个节点指向 C（此时改变了方向）。
>
> （6）将 last 的前一个节点指向 B 。
>
> （7）将 first 前一个节点指向 A。

代码实现：

```c++
template<class T, class Alloc = alloc>
class list {
	protected:
		void transfer(iterator position, iterator first, iterator last) {
		if (position != last) {
      (*(link_type((*last.node).prev))).next  = position.node;//(1)
      (*(link_type((*first.node).prev))).next = last.node;//(2)
      (*(link_type((*position.node).prev))).next = first.node;//(3)
      link_type tmp = link_type((*position.node).prev);//(4)
      (*position.node).prev = (*last.node).prev;//(5)
      (*last.node).prev = (*first.node).prev;//(6)
      (*first.node).prev = tmp;//(7)
    }
	}
}
```

至此，完成迁移。

### splice 函数

注意，上述的 `transfer` 并非公开接口。`list`  公开提供的是所谓的接合操作（ `splice` ）：将某连续范围的元素从一个 ` list`  移动到另一个（或同一个）`list`  的某个节点。

为了提供各个接口，list<T>::splice 提供了许多版本。

```c++
template<class T. class Alloc = alloc>
class list {
public:
    //将 x 接合于 position 所指位置之前 x 必须不同于 *this
    void splice(iterator position, std::list& x) {
        if (!x.empty()) {
            transfer(position, x.begin(), x.end());
        }
    }
    //将 i 所指元素接合于 position 所指位置之前。position 和 i 可能指向同一个 list
    void splice(iterator position, list&, iterator i) {
        iterator j = i;
        ++j;
        if (position == i || position == j) return;
        transfer(position, i, j);
    }
    //将[first,last)内的所有元素接合于 position 所指元素之前
    void splice(iterator position, list&, iterator first, iterator last) {
        if (first != last) {
            transfer(position, first, last);
        }
    }
};
```

**测试**

```c++
void ListTest() {
    int iv[5] = {2,4,5,6,7};
    int ivv[5] = {20,40,50,60,70};
    std::list<int> L1(iv, iv+5);
    std::list<int> L2(ivv, ivv+5);
    auto ite = std::find(L2.begin(), L2.end(), 40);
    L2.splice(ite, L1);
    for(auto& e : L2) {
        std::cout<<e<<" ";
    }
}
```

**输出**

```c++
20 2 4 5 6 7 40 50 60 70 
```

<div align="center"> <img src="https://i.loli.net/2020/11/22/HJurghYRn9eB7t8.png"width="500"/> </div><br>

### merge函数

`merge` 函数接受一个 `list` 参数。

`merge` 函数是将传入的 `list` 链表x与原链表按从小到大合并到原链表中(前提是两个链表都是已经从小到大排序了)。

这里 `merge ` 的核心就是 `transfer `函数。

```c++
template <class T, class Alloc>
void list<T, Alloc>::merge(list<T, Alloc>& x) {
  iterator first1 = begin();
  iterator last1 = end();
  iterator first2 = x.begin();
  iterator last2 = x.end();
  while (first1 != last1 && first2 != last2)
    if (*first2 < *first1) {
      iterator next = first2;
      // 将first2到first+1的左闭右开区间插入到first1的前面
      // 这就是将first2合并到first1链表中
      transfer(first1, first2, ++next);
      first2 = next;
    }
    else {
      ++first1;
    }
     // 如果链表x还有元素则全部插入到first1链表的尾部
  	if (first2 != last2) transfer(last1, first2, last2);
}
```

### reverse 函数

`reverse ` 函数是实现将链表翻转的功能。

主要是 `list` 的迭代器基本不会改变的特点，将每一个元素一个个插入到 `begin` 之前，这里注意迭代器不会变，但是`begin`会改变， 它始终指向第一个元素的地址。

```c++
template <class T, class Alloc>
void list<T, Alloc>::reverse() {
  //如果是空链表或者只有一个元素则不进行任何操作
  if (node->next == node || link_type(node->next)->next == node) 
  	return;
  iterator first = begin();
  ++first;
  while (first != end()) {
    iterator old = first;
    ++first;
      // 将元素插入到begin()之前
    transfer(begin(), old, first);
  }
} 
```

