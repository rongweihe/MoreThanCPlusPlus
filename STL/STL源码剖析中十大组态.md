# SGI STL  C++ 组态常量

以下列出的组态常量基本都是和 template 参数推导，偏特化有关，先来学习下面几个常量，有助于后续 SGI STL 源码的进一步的理解。

## 组态 1：定义 static data members

```c++
template <typename T>
class testClass {
public:
    static int _data;
};
//为 static data members 进行定义,配置内存,设定初值
template <> int testClass<int>::_data = 1; //C++11 需要加上 template <>
template <> int testClass<char>::_data = 2;

int main() {

    std::cout<< testClass<int>::_data<< std::endl;//print => 1
    std::cout<< testClass<char>::_data << std::endl;//print => 2

    testClass<int> obji1, obji2;
    testClass<char> objc1, objc2;
    std::cout << obji1._data << std::endl;//print => 1
    std::cout << obji2._data << std::endl;//print => 1
    std::cout << objc1._data << std::endl;//print => 2
    std::cout << objc2._data << std::endl;//print => 2
    return 0;
}
```

## 组态 2：测试 class template partial specialization - 在 class template 的一般化设计之外，特别针对某些 template 参数做特殊化设计

```c++
//一般化设计
template <class I, class O>
class testClass {
public:
    testClass() { std::cout<< "I O" << std::endl; }
};
//特殊化设计
template <class T>
class testClass<T*, T*> {
public:
    testClass() { std::cout<< "T*T*" << std::endl; }
};
//特殊化设计
template <class T>
class testClass<const T*,T*> {
public:
    testClass() { std::cout<< "const T* T*" << std::endl; }
};

int main() {
    testClass<int, char> obj1; //print=> I O
    testClass<int*, int*> obj2; //print=> T*T*
    testClass<const int*, int*> obj3; //print=> const T* T*
    return 0;
}
```

## 组态 3 ：测试 class template 是否可以嵌套

```c++
class alloc {
};
//一般化设计
template <class T, class Alloc = alloc>
class vv {
public:
    typedef T value_type;
    typedef value_type* iterator;
    template<class I>
        void insert(iterator position, I first, I last) {
            std::cout<< "insert()" << std::endl;
        }
};

int main() {
    int ia[5] = {1,2,3,4,5};
    vv<int> v;
    vv<int>::iterator ite;
    v.insert(ite, 1, 5);//print=> insert()
    v.insert(ite, ia, ia+5);//print=> insert()
    return 0;
}
```

## 组态 4：测试 template 参数可否根据前一个 template 参数而设定默认值

```c++
class alloc {
};
template <class T, class Alloc = alloc, size_t BufSiz = 0 >
class testClass1 {
public:
    testClass1() {   std::cout<< "test1" <<std::endl;    }
};
//根据前一个参数值T设定下一个参数Sequence的默认值为deque<T>
template <class T, class Sequence = testClass1<T> >
class testClass2 {
public:
    testClass2() {   std::cout<< "test2" <<std::endl;    }
private:
    Sequence se;//调用testClass1的构造函数初始化
};

int main() {
    testClass2<int>t;//print=> test1
                     //print=> test2
    return 0;
}
```

## 组态 5：测试 class template 可否拥有 non-type 模板参数

```c++
class alloc {
};

inline size_t __deque_buf_size(size_t n,size_t sz) {
    return n!=0?n:(sz<512?size_t(512/sz):size_t(1));
}

template <class T,class Ref,class Ptr,size_t Bufsiz>
struct __deque_iterator {
    typedef __deque_iterator<T,T&,T*,Bufsiz> iterator;
    typedef __deque_iterator<T,const T&,const T*,Bufsiz> const_iterator;
    static size_t buffer_size() {return __deque_buf_size(Bufsiz,sizeof(T));}
};

template <class T,class Alloc=alloc,size_t Bufsiz=0>
class testDeque {
public:
    typedef __deque_iterator<T,T&,T*,Bufsiz> iterator;
};


int main() {
    std::cout<<testDeque<int>::iterator::buffer_size()<<std::endl;//print=> 128
    std::cout<<testDeque<int,alloc,64>::iterator::buffer_size()<<std::endl;//print=64
    return 0;
}
/*
/Users/herongwei/CLionProjects/C++Code/cmake-build-debug/C__Code
128
64
 * */
```

