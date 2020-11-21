#include <deque>
#include <queue>
#include <algorithm>
#include <iostream>
using namespace std;

//全部是转调用
template<class T, class Sequence=deque<T>>
class queue {
public:
    typedef typename Sequence::value_type;
    typedef typename Sequence::size_type size_type;
    typedef typename Sequence::reference reference;
    typedef typename Sequence::const_reference const_reference;
protected:
    Sequence c; //底层容器
public:
    bool empty() const {    return c.empty();   }
    size_type size() const {    return c.size();    }
    reference front() {     return c.front();   }
    const_reference front() {     return c.front();   }
    reference back() {  return c.back();    }
    const_reference back() {  return c.back();    }
    void push(const value_type& x ) {   c.push_back(x); }
    void pop() {    return c.pop_front();   }
};