首先来谈谈this指针的用处：

（1）一个对象的this指针并不是对象本身的一部分，不会影响sizeof(对象)的结果。

（2）this作用域是在类内部，当在类的非静态成员函数中访问类的非静态成员的时候，编译器会自动将对象本身的地址作为一个隐含参数传递给函数。也就是说，即使你没有写上this指针，编译器在编译的时候也是加上this的，它作为非静态成员函数的隐含形参，对各成员的访问均通过this进行。

其次，this指针的使用：

（1）在类的非静态成员函数中返回类对象本身的时候，直接使用 return *this。

（2）当参数与成员变量名相同时，如this->n = n （不能写成n = n)。

另外，在网上大家会看到this会被编译器解析成`A *const`，`A const *`，究竟是哪一个呢？下面通过断点调试分析：

现有如下例子：

```c++

class Person {
public:
    enum SexType {
        BOY = 0,
        GIRL
    };
    Person(char *name, int age, SexType s) {
        name_ = new char[strlen(name) + 1];
        strcpy(name_, name);
        age_ = age;
        s_ = s;
    }
    int GetAge() const {
        return this->age_;
    }
    Person& AddAge(int n) {
        age_ += n;
        return *this;
    }
    ~Person() {
        delete [] name_;
    }

private:
    char *name_;
    int age_;
    SexType s_;
};

int main() {
    Person p("xiaohe", 25, Person::BOY);
    std::cout << p.GetAge() << std::endl;// print => 25
    std::cout << p.AddAge(1).GetAge() << std::endl;//print => 26
    return 0;
}
```

对于上面这个简单的程序，相信大家没得问题吧，就是定义了一个 `Person` 类，然后初始化构造函数，并获取这个人的年龄，设置后，再获取。

