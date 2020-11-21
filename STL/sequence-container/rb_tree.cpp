/*
RB tree (红黑树)是一种高度平衡二元搜索树(balanced binary search tree) 中常用的一种
平衡二元搜索树的特点：有利于查找和插入，并保持高度平衡-没有任何一个节点过深或者过浅
RB tree 提供遍历操作且迭代器，按正常规则遍历便能得到排序状态。

RB tree 提供两种 insert 操作 insert_unique() 和 insert_equal()
前者表示节点的key一定在整个tree中独一无二否则安插失败
后者表示节点的key可以重复
*/
template <class Key,
        class Value,
        class KeyOfValue,
        class Compare,
        class Alloc = alloc>
class rb_tree {
protected:
    typedef __rb_tree_node<Value> rb_tree_node;
public:
    typedef rb_tree_node* link_type;
protected:
    size_type node_count;//rb_tree 的大小:节点数量
    link_type header;
    Compare key_compare;//key的大小比较准则
};