# list（下篇-sort 源码分析）

## 前言

前两节对 `list` 的 push，pop， insert 等操作做了分析, 本节准备探讨 `list` 怎么实现 `sort` 功能。

 `list`  的 sort 算法实现

## 相关函数实现

在 分析 `sort` 之前先来分析 `transfer` ， `reverse`，`merge` 这几个会被调用的函数。

### Transfer 函数

此函数功能是将一段链表插入到我们指定的位置之前，提供一个迁移的功能。这个函数的实现原理最好搞明白，其实学过链表的话，搞明白它不难，都是指针的指向而已。后面分析的所有函数都是该基础上进行修改的。

输入：该函数接受 3 个迭代器，第一个 position 表示要插入的位置，第二个和第三个表示将 [first, last) 内的所有元素移到 position 之前。

下面就先用一张图来看看它的逻辑。

![list-transfer.png](https://i.loli.net/2020/11/22/SwR9sgNhQOoKa6T.jpg)

其中 A 节点代表 迁移之后 first 节点的前一个节点。

B 节点代表迁移之后 last 节点的前一个节点。

C 节点代表迁移之后 position 节点的前一个节点。

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
	...
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