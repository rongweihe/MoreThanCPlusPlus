#include <deque>
#include <algorithm>
#include <iostream>
using namespace std;
/*
[1]deque 的迭代器是随机读取迭代器 是一个class 其中有四个关键的成员函数
https://github.com/steveLauwh/SGI-STL/tree/master/The%20Annotated%20STL%20Sources%20V3.3/container/sequence%20container/deque

连续是假象 分段是事实
*/
template<class T, class Alloc = alloc,size_t BufSiz = 0>
class deque {
private:
    /* data */
public: 
    typedef T value_type;
    typedef __deque_iterator<T, T&, T*, BufSiz> iterator;
protected:
    typedef pointer* map_pointer; //T*
    iterator start;
    iterator finish;
    map_pointer map;
    size_type map_size;
public:
    iterator begin() {  return start;   }
    iterator end() {    return finish;  }
    size_type size() const {    return finish - start;  }
public:
    //deque 聪明的地方在于你安插一个元素会判断距离尾端节点近还是距离头结点近
    /*
    每一次推 都要调用构造函数和析构函数
    */
    iterator insert(iterator position, const value type& x) {
        if (position.cur == start.cur) {
            push_front(x);
            return start;
        } else if (position.cur == finish.cur) {
            push_back(x);
            iterator tmp = finish;
            --tmp;
            return tmp;
        }
    }

    template
};

template<class T, class Ref, class Ptr, size_t BufSiz>
struct __deque_iterator {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T** map_pointer;
    typedef __deque_iterator self;

    T* cur;// 迭代器指向缓冲区的当前元素
    T* first;// 迭代器指向缓冲区的头部
    T* last; // 迭代器指向缓冲区的尾部
    map_pointer node;// 迭代器指向 map 的 node
};

