# STL 模板

### 前言

在分析 STL 源码之前，我们需要对 STL 模板有一定的了解，STL 源码包含大量的模板代码，要明白两者之间在哪些方面是相同的，哪些方面是不同的。明白了这一点，我们才能深入后面的内容。

### 目录

STL 模板是什么？

STL 模板有哪几种类型？

STL 模板中 typename 和 class 的区别？



相同之处

一般认为，对模板参数类型来说， typename 和 class 是一样的

比如，你可以定义一个模板类

```c++
template <class T>
class Config {}；
```

同样，你也可以写成

```c++
template <typename T>
class Config {};
```

这两者都是一样的，两者 `typename` 和 `class` 在**参数类型**中没有区别。

那既然相同又为什么定义这两个符号呢？

> 1、
>
> 2、

 

### 何谓偏特化（template partial specialization）？

如果一个类的模板拥有一个以上的模板参数，我们可以针对其中某个模板参数进行特别指定，换句话说，我们可以在泛化设计中提供一个特化版本，就是将泛化版本中的某些模板参数赋予明确的指定。

举个例子

假设有一个 class template 如下：

```c++
template<typename U, typename V, typename T>
class A {...};
```

一个偏特化：

```c++
template<typename T>
class A<T*> {...};
```

这个特化版本适用于 “T 为原生指针” 的情况。也就是说，这个模板只接受模板参数为指针的情况。

这里的 “T 为原生指针”   就是 “T 为任何类型” 的一种特殊情况。

