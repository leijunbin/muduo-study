# C++ 部分用法

## explicit 作用

在 C++ 中，explicit 关键字用来修饰类的构造函数，被修饰的构造函数的类，不能发生相应的隐式类型转换，只能以显式的方式进行类型转换。

explicit 使用注意事项：

+ explicit 关键字只能用于类内部的构造函数声明上。
+ explicit 关键字作用于单个参数的构造函数。

## std::function(void())

c++11 新增了 std::function、std::bind、lambda 表达式等封装使函数调用更加方便。

### std::function

讲 std::function 前首先需要了解下什么是可调用对象

满足以下条件之一就可称为可调用对象：

+ 是一个函数指针。
+ 是一个具有operator()成员函数的类对象(传说中的仿函数)，lambda表达式。
+ 是一个可被转换为函数指针的类对象。
+ 是一个类成员(函数)指针。
+ bind表达式或其它函数对象。

而 std::function 就是上面这种可调用对象的封装器，可以把 std::function 看做一个函数对象，用于表示函数这个抽象概念。std::function 的实例可以存储、复制和调用任何可调用对象，存储的可调用对象称为 std::function 的目标，若 std::function 不含目标，则称它为空，调用空的 std::function 的目标会抛出std::bad_function_call 异常。

使用参考如下实例代码：

```cpp
std::function<void(int)> f; // 这里表示function的对象f的参数是int，返回值是void
#include <functional>
#include <iostream>

struct Foo {
    Foo(int num) : num_(num) {}
    void print_add(int i) const { std::cout << num_ + i << '\n'; }
    int num_;
};

void print_num(int i) { std::cout << i << '\n'; }

struct PrintNum {
    void operator()(int i) const { std::cout << i << '\n'; }
};

int main() {
    // 存储自由函数
    std::function<void(int)> f_display = print_num;
    f_display(-9);

    // 存储 lambda
    std::function<void()> f_display_42 = []() { print_num(42); };
    f_display_42();

    // 存储到 std::bind 调用的结果
    std::function<void()> f_display_31337 = std::bind(print_num, 31337);
    f_display_31337();

    // 存储到成员函数的调用
    std::function<void(const Foo&, int)> f_add_display = &Foo::print_add;
    const Foo foo(314159);
    f_add_display(foo, 1);
    f_add_display(314159, 1);

    // 存储到数据成员访问器的调用
    std::function<int(Foo const&)> f_num = &Foo::num_;
    std::cout << "num_: " << f_num(foo) << '\n';

    // 存储到成员函数及对象的调用
    using std::placeholders::_1;
    std::function<void(int)> f_add_display2 = std::bind(&Foo::print_add, foo, _1);
    f_add_display2(2);

    // 存储到成员函数和对象指针的调用
    std::function<void(int)> f_add_display3 = std::bind(&Foo::print_add, &foo, _1);
    f_add_display3(3);

    // 存储到函数对象的调用
    std::function<void(int)> f_display_obj = PrintNum();
    f_display_obj(18);
}
```

从上面可以看到 std::function 的使用方法，当给 std::function 填入合适的参数表和返回值后，它就变成了可以容纳所有这一类调用方式的函数封装器。std::function 还可以用作回调函数，或者在 C++ 里如果需要使用回调那就一定要使用std::function，特别方便，这方面的使用方式大家可以读下我之前写的关于线程池和定时器相关的文章。

### std::bind

使用 std::bind 可以将可调用对象和参数一起绑定，绑定后的结果使用 std::function 进行保存，并延迟调用到任何我们需要的时候。

std::bind 通常有两大作用：

+ 将可调用对象与参数一起绑定为另一个 std::function 供调用
+ 将n元可调用对象转成 m(m < n) 元可调用对象，绑定一部分参数，这里需要使用 std::placeholders

具体示例：

```cpp
#include <functional>
#include <iostream>
#include <memory>

void f(int n1, int n2, int n3, const int& n4, int n5) {
    std::cout << n1 << ' ' << n2 << ' ' << n3 << ' ' << n4 << ' ' << n5 << std::endl;
}

int g(int n1) { return n1; }

struct Foo {
    void print_sum(int n1, int n2) { std::cout << n1 + n2 << std::endl; }
    int data = 10;
};

int main() {
    using namespace std::placeholders;  // 针对 _1, _2, _3...

    // 演示参数重排序和按引用传递
    int n = 7;
    // （ _1 与 _2 来自 std::placeholders ，并表示将来会传递给 f1 的参数）
    auto f1 = std::bind(f, _2, 42, _1, std::cref(n), n);
    n = 10;
    f1(1, 2, 1001);  // 1 为 _1 所绑定， 2 为 _2 所绑定，不使用 1001
                     // 进行到 f(2, 42, 1, n, 7) 的调用

    // 嵌套 bind 子表达式共享占位符
    auto f2 = std::bind(f, _3, std::bind(g, _3), _3, 4, 5);
    f2(10, 11, 12);  // 进行到 f(12, g(12), 12, 4, 5); 的调用

    // 绑定指向成员函数指针
    Foo foo;
    auto f3 = std::bind(&Foo::print_sum, &foo, 95, _1);
    f3(5);

    // 绑定指向数据成员指针
    auto f4 = std::bind(&Foo::data, _1);
    std::cout << f4(foo) << std::endl;

    // 智能指针亦能用于调用被引用对象的成员
    std::cout << f4(std::make_shared<Foo>(foo)) << std::endl;
}
```

### lambda表达式

lambda 表达式可以说是 c++11 引入的最重要的特性之一，它定义了一个匿名函数，可以捕获一定范围的变量在函数内部使用，一般有如下语法形式：

```cpp
auto func = [capture] (params) opt -> ret { func_body; };
```

其中func是可以当作lambda表达式的名字，作为一个函数使用，capture 是捕获列表，params 是参数表，opt 是函数选项(mutable 之类)， ret 是返回值类型，func_body 是函数体。

一个完整的lambda表达式：

```cpp
auto func1 = [](int a) -> int { return a + 1; };
auto func2 = [](int a) { return a + 2; };
cout << func1(1) << " " << func2(2) << endl;
```

如上代码，很多时候 lambda 表达式返回值是很明显的，c++11 允许省略表达式的返回值定义。

lambda表达式允许捕获一定范围内的变量：

* [] 不捕获任何变量
* [&] 引用捕获，捕获外部作用域所有变量，在函数体内当作引用使用
* [=] 值捕获，捕获外部作用域所有变量，在函数内内有个副本使用
* [=, &a] 值捕获外部作用域所有变量，按引用捕获 a 变量
* [a] 只值捕获 a 变量，不捕获其它变量
* [this] 捕获当前类中的 this 指针

lambda表达式示例代码：

```cpp
int a = 0;
auto f1 = [=](){ return a; }; // 值捕获a
cout << f1() << endl;

auto f2 = [=]() { return a++; }; // 修改按值捕获的外部变量，error
auto f3 = [=]() mutable { return a++; };
```

代码中的 f2 是编译不过的，因为我们修改了按值捕获的外部变量，其实 lambda 表达式就相当于是一个仿函数，仿函数是一个有 operator() 成员函数的类对象，这个 operator() 默认是 const 的，所以不能修改成员变量，而加了 mutable，就是去掉 const 属性。

还可以使用 lambda 表达式自定义 stl 的规则，例如自定义 sort 排序规则：

```cpp
struct A {
    int a;
    int b;
};

int main() {
    vector<A> vec;
    std::sort(vec.begin(), vec.end(), [](const A &left, const A &right) { return left.a < right.a; });
}
```

## 右值引用

### 什么是左值、右值

首先不考虑引用以减少干扰，可以从2个角度判断：左值可以取地址、位于等号左边；而右值没法取地址，位于等号右边。

```cpp
int a = 5;
```

+ a可以通过 & 取地址，位于等号左边，所以a是左值。
+ 5位于等号右边，5没法通过 & 取地址，所以5是个右值。

再举个例子：

```cpp
struct A {
    A(int a = 0) {
        a_ = a;
    }
 
    int a_;
};
 
A a = A();
```

+ 同样的，a可以通过 & 取地址，位于等号左边，所以a是左值。
+ A()是个临时值，没法通过 & 取地址，位于等号右边，所以A()是个右值。

可见左右值的概念很清晰，有地址的变量就是左值，没有地址的字面值、临时值就是右值。

### 什么是左值引用、右值引用

引用本质是别名，可以通过引用修改变量的值，传参时传引用可以避免拷贝，其实现原理和指针类似。个人认为，引用出现的本意是为了降低 C 语言指针的使用难度，但现在指针和左右值引用共同存在，反而大大增加了学习和理解成本。

#### 左值引用

左值引用大家都很熟悉，能指向左值，不能指向右值的就是左值引用：

```cpp
int a = 5;
int &ref_a = a; // 左值引用指向左值，编译通过
int &ref_a = 5; // 左值引用指向了右值，会编译失败
```

引用是变量的别名，由于右值没有地址，没法被修改，所以左值引用无法指向右值。

但是，const左值引用是可以指向右值的：

```cpp
const int &ref_a = 5;  // 编译通过
```

const 左值引用不会修改指向值，因此可以指向右值，这也是为什么要使用 const & 作为函数参数的原因之一，如std::vector 的 push_back：

```cpp
void push_back (const value_type& val);
```

如果没有const，vec.push_back(5) 这样的代码就无法编译通过了。

#### 右值引用

再看下右值引用，右值引用的标志是&&，顾名思义，右值引用专门为右值而生，可以指向右值，不能指向左值：

```cpp
int &&ref_a_right = 5; // ok
 
int a = 5;
int &&ref_a_left = a; // 编译不过，右值引用不可以指向左值
 
ref_a_right = 6; // 右值引用的用途：可以修改右值
```

#### 对左右值引用本质的讨论

下边的论述比较复杂，也是本文的核心，对理解这些概念非常重要。

##### 右值引用有办法指向左值吗？

有办法，std::move：

```cpp
int a = 5; // a是个左值
int &ref_a_left = a; // 左值引用指向左值
int &&ref_a_right = std::move(a); // 通过std::move将左值转化为右值，可以被右值引用指向

cout << a; // 打印结果：5
```

在上边的代码里，看上去是左值 a 通过 std::move 移动到了右值 ref_a_right 中，那是不是 a 里边就没有值了？并不是，打印出 a 的值仍然是5。

std::move 是一个非常有迷惑性的函数，不理解左右值概念的人们往往以为它能把一个变量里的内容移动到另一个变量，但事实上 std::move 移动不了什么，唯一的功能是把左值强制转化为右值，让右值引用可以指向左值。其实现等同于一个类型转换：static_cast<T&&>(lvalue)。 所以，单纯的 std::move(xxx) 不会有性能提升，std::move 的使用场景在第三章会讲。

同样的，右值引用能指向右值，本质上也是把右值提升为一个左值，并定义一个右值引用通过std::move指向该左值：

```cpp
int &&ref_a = 5;
ref_a = 6; 
```

等同于以下代码：

```cpp
int temp = 5;
int &&ref_a = std::move(temp);
ref_a = 6;
```

##### 左值引用、右值引用本身是左值还是右值？

被声明出来的左、右值引用都是左值。 因为被声明出的左右值引用是有地址的，也位于等号左边。仔细看下边代码：

```cpp
// 形参是个右值引用
void change(int&& right_value) {
    right_value = 8;
}
 
int main() {
    int a = 5; // a是个左值
    int &ref_a_left = a; // ref_a_left是个左值引用
    int &&ref_a_right = std::move(a); // ref_a_right是个右值引用
 
    change(a); // 编译不过，a是左值，change参数要求右值
    change(ref_a_left); // 编译不过，左值引用ref_a_left本身也是个左值
    change(ref_a_right); // 编译不过，右值引用ref_a_right本身也是个左值
   
    change(std::move(a)); // 编译通过
    change(std::move(ref_a_right)); // 编译通过
    change(std::move(ref_a_left)); // 编译通过
 
    change(5); // 当然可以直接接右值，编译通过
   
    cout << &a << ' ';
    cout << &ref_a_left << ' ';
    cout << &ref_a_right;
    // 打印这三个左值的地址，都是一样的
}
```

看完后你可能有个问题，std::move 会返回一个右值引用 int &&，它是左值还是右值呢？从表达式 int &&ref = std::move(a) 来看，右值引用 ref 指向的必须是右值，所以 move 返回的 int && 是个右值。所以右值引用既可能是左值，又可能是右值吗？ 确实如此：右值引用既可以是左值也可以是右值，如果有名称则为左值，否则是右值。

或者说：作为函数返回值的 && 是右值，直接声明出来的 && 是左值。 这同样也符合第一章对左值，右值的判定方式：其实引用和普通变量是一样的，int &&ref = std::move(a) 和 int a = 5 没有什么区别，等号左边就是左值，右边就是右值。

最后，从上述分析中我们得到如下结论：

+ 从性能上讲，左右值引用没有区别，传参使用左右值引用都可以避免拷贝。
+ 右值引用可以直接指向右值，也可以通过 std::move 指向左值；而左值引用只能指向左值（const 左值引用也能指向右值）。
+ 作为函数形参时，右值引用更灵活。虽然const左值引用也可以做到左右值都接受，但它无法修改，有一定局限性。

```cpp
void f(const int& n) {
    n += 1; // 编译失败，const左值引用不能修改指向变量
}

void f2(int && n) {
    n += 1; // ok
}

int main() {
    f(5);
    f2(5);
}
```

### 右值引用和 std::move 的应用场景

按上文分析，std::move 只是类型转换工具，不会对性能有好处；右值引用在作为函数形参时更具灵活性，看上去还是挺鸡肋的。他们有什么实际应用场景吗？

#### 实现移动语义

在实际场景中，右值引用和 std::move 被广泛用于在 STL 和自定义类中实现移动语义，避免拷贝，从而提升程序性能。 在没有右值引用之前，一个简单的数组类通常实现如下，有构造函数、拷贝构造函数、赋值运算符重载、析构函数等。深拷贝/浅拷贝在此不做讲解。

```cpp
class Array {
public:
    Array(int size) : size_(size) {
        data = new int[size_];
    }
   
    // 深拷贝构造
    Array(const Array& temp_array) {
        size_ = temp_array.size_;
        data_ = new int[size_];
        for (int i = 0; i < size_; i ++) {
            data_[i] = temp_array.data_[i];
        }
    }
   
    // 深拷贝赋值
    Array& operator=(const Array& temp_array) {
        delete[] data_;
 
        size_ = temp_array.size_;
        data_ = new int[size_];
        for (int i = 0; i < size_; i ++) {
            data_[i] = temp_array.data_[i];
        }
    }
 
    ~Array() {
        delete[] data_;
    }
 
public:
    int *data_;
    int size_;
};
```

该类的拷贝构造函数、赋值运算符重载函数已经通过使用左值引用传参来避免一次多余拷贝了，但是内部实现要深拷贝，无法避免。这时，有人提出一个想法：是不是可以提供一个移动构造函数，把被拷贝者的数据移动过来，被拷贝者后边就不要了，这样就可以避免深拷贝了，如：

```cpp
class Array {
public:
    Array(int size) : size_(size) {
        data = new int[size_];
    }
   
    // 深拷贝构造
    Array(const Array& temp_array) {
        ...
    }
   
    // 深拷贝赋值
    Array& operator=(const Array& temp_array) {
        ...
    }
 
    // 移动构造函数，可以浅拷贝
    Array(const Array& temp_array, bool move) {
        data_ = temp_array.data_;
        size_ = temp_array.size_;
        // 为防止temp_array析构时delete data，提前置空其data_  
        temp_array.data_ = nullptr;
    }
   
 
    ~Array() {
        delete [] data_;
    }
 
public:
    int *data_;
    int size_;
};
```

这么做有2个问题：

+ 不优雅，表示移动语义还需要一个额外的参数(或者其他方式)。
+ 无法实现！temp_array 是个 const 左值引用，无法被修改，所以 temp_array.data_ = nullptr; 这行会编译不过。当然函数参数可以改成非 const：Array(Array& temp_array, bool move){...}，这样也有问题，由于左值引用不能接右值，Array a = Array(Array(), true); 这种调用方式就没法用了。

可以发现左值引用真是用的很不爽，右值引用的出现解决了这个问题，在STL的很多容器中，都实现了以右值引用为参数的移动构造函数和移动赋值重载函数，或者其他函数，最常见的如 std::vector 的 push_back 和 emplace_back。参数为左值引用意味着拷贝，为右值引用意味着移动。

```cpp
class Array {
public:
    ......
 
    // 优雅
    Array(Array&& temp_array) {
        data_ = temp_array.data_;
        size_ = temp_array.size_;
        // 为防止temp_array析构时delete data，提前置空其data_  
        temp_array.data_ = nullptr;
    }
   
 
public:
    int *data_;
    int size_;
};
```

如何使用：

```cpp
// 例1：Array用法
int main(){
    Array a;
 
    // 做一些操作
    .....
   
    // 左值a，用std::move转化为右值
    Array b(std::move(a));
}
```

#### 实例：vector::push_back 使用 std::move 提高性能

```cpp
// 例2：std::vector和std::string的实际例子
int main() {
    std::string str1 = "aacasxs";
    std::vector<std::string> vec;
   
    vec.push_back(str1); // 传统方法，copy
    vec.push_back(std::move(str1)); // 调用移动语义的push_back方法，避免拷贝，str1会失去原有值，变成空字符串
    vec.emplace_back(std::move(str1)); // emplace_back效果相同，str1会失去原有值
    vec.emplace_back("axcsddcas"); // 当然可以直接接右值
}
 
// std::vector方法定义
void push_back (const value_type& val);
void push_back (value_type&& val);
 
void emplace_back (Args&&... args);
```

在 vector 和 string 这个场景，加个 std::move 会调用到移动语义函数，避免了深拷贝。

除非设计不允许移动，STL 类大都支持移动语义函数，即可移动的。另外，编译器会默认在用户自定义的 class 和 struct 中生成移动语义函数，但前提是用户没有主动定义该类的拷贝构造等函数。因此，可移动对象在<需要拷贝且被拷贝者之后不再被需要>的场景，建议使用 std::move 触发移动语义，提升性能。

```cpp
moveable_objecta = moveable_objectb; 
改为： 
moveable_objecta = std::move(moveable_objectb);
```

还有些 STL 类是 move-only 的，比如 unique_ptr，这种类只有移动构造函数，因此只能移动(转移内部对象所有权，或者叫浅拷贝)，不能拷贝(深拷贝):

```cpp
std::unique_ptr<A> ptr_a = std::make_unique<A>();

std::unique_ptr<A> ptr_b = std::move(ptr_a); // unique_ptr只有‘移动赋值重载函数‘，参数是&& ，只能接右值，因此必须用std::move转换类型

std::unique_ptr<A> ptr_b = ptr_a; // 编译不通过
```

std::move 本身只做类型转换，对性能无影响。我们可以在自己的类中实现移动语义，避免深拷贝，充分利用右值引用和std::move 的语言特性。

### 完美转发 std::forward

和 std::move 一样，它的兄弟 std::forward 也充满了迷惑性，虽然名字含义是转发，但他并不会做转发，同样也是做类型转换.

与 move 相比，forward 更强大，move 只能转出来右值，forward 都可以。

> std::forward `<T>`(u) 有两个参数：T 与 u。
>
> + 当T为左值引用类型时，u将被转换为T类型的左值；
> + 否则u将被转换为T类型右值。

举个例子，有 main，A，B 三个函数，调用关系为：main->A->B，建议先看懂对左右值引用本身是左值还是右值的讨论再看这里：

```cpp
void B(int&& ref_r) {
    ref_r = 1;
}
 
// A、B的入参是右值引用
// 有名字的右值引用是左值，因此ref_r是左值
void A(int&& ref_r) {
    B(ref_r);  // 错误，B的入参是右值引用，需要接右值，ref_r是左值，编译失败
   
    B(std::move(ref_r)); // ok，std::move把左值转为右值，编译通过
    B(std::forward<int>(ref_r));  // ok，std::forward的T是int类型，属于条件b，因此会把ref_r转为右值
}
 
int main() {
    int a = 5;
    A(std::move(a));
}
```

例2：

```cpp
void change2(int&& ref_r) {
    ref_r = 1;
}
 
void change3(int& ref_l) {
    ref_l = 1;
}
 
// change的入参是右值引用
// 有名字的右值引用是 左值，因此ref_r是左值
void change(int&& ref_r) {
    change2(ref_r);  // 错误，change2的入参是右值引用，需要接右值，ref_r是左值，编译失败
   
    change2(std::move(ref_r)); // ok，std::move把左值转为右值，编译通过
    change2(std::forward<int &&>(ref_r));  // ok，std::forward的T是右值引用类型(int &&)，符合条件b，因此u(ref_r)会被转换为右值，编译通过
   
    change3(ref_r); // ok，change3的入参是左值引用，需要接左值，ref_r是左值，编译通过
    change3(std::forward<int &>(ref_r)); // ok，std::forward的T是左值引用类型(int &)，符合条件a，因此u(ref_r)会被转换为左值，编译通过
    // 可见，forward可以把值转换为左值或者右值
}
 
int main() {
    int a = 5;
    change(std::move(a));
}
```

## std::thread

### 多线程编程简介

说到多线程编程，就不得不提**并行**和 **并发** ，多线程是实现并发和并行的一种手段。

+ **并行**是指两个或多个独立的操作 **同时进行** 。
+ **并发**是指**一个时间段内**执行多个操作。

在单核时代，多个线程是**并发**的，在一个时间段内轮流执行；在多核时代，多个线程可以实现真正的 **并行** ，在多核上真正独立的并行执行。例如现在常见的**4核4线程**可以并行4个线程；**4核8线程**则使用了超线程技术，把一个物理核模拟为2个逻辑核心，可以并行8个线程。

#### 并发编程的方法

通常，要实现并发有两种方法：多进程并发和多线程并发。

##### 多进程并发

使用多进程并发是将一个应用程序划分为多个独立的进程（每个进程只有一个线程），这些独立的进程间可以互相通信，共同完成任务。

由于操作系统对进程提供了大量的保护机制，以避免一个进程修改了另一个进程的数据，使用多进程比多线程更容易写出**安全**的代码。但这也造就了多进程并发的两个缺点：

+ 在进程件的通信，无论是使用信号、套接字，还是文件、管道等方式，其使用要么比较复杂，要么就是速度较慢或者两者兼而有之。
+ 运行多个线程的开销很大，操作系统要分配很多的资源来对这些进程进行管理。

由于多个进程并发完成同一个任务时，不可避免的是：操作同一个数据和进程间的相互通信，上述的两个缺点也就决定了多进程的并发不是一个好的选择。

##### 多线程并发

多线程并发指的是在同一个进程中执行多个线程。

有操作系统相关知识的应该知道，线程是轻量级的进程，每个线程可以独立的运行不同的指令序列，但是线程不独立的拥有资源，依赖于创建它的进程而存在。也就是说，**同一进程中的多个线程共享相同的地址空间，可以访问进程中的大部分数据，指针和引用可以在线程间进行传递**。这样，同一进程内的多个线程能够很方便的进行数据共享以及通信，也就比进程更适用于并发操作。

由于缺少操作系统提供的保护机制，在多线程共享数据及通信时，就需要程序员做更多的工作以保证对共享数据段的操作是以预想的操作顺序进行的，并且要极力的避免**死锁(deadlock)**。

### std::thread 简介

C++11 之前，windows 和 linux 平台分别有各自的多线程标准，使用 C++ 编写的多线程往往是依赖于特定平台的。

+ Windows 平台提供用于多线程创建和管理的 win32 api；
+ Linux 下则有 POSIX 多线程标准，Threads 或 Pthreads 库提供的 API 可以在类 Unix 上运行；

在 C++11 新标准中，可以简单通过使用 thread 库，来管理多线程。thread 库可以看做对不同平台多线程 API 的一层包装；因此使用新标准提供的线程库编写的程序是跨平台的。

### 一个简单的多线程实现

C++11 的标准库中提供了多线程库，使用时需要 #include `<thread>` 头文件，该头文件主要包含了对线程的管理类 std::thread 以及其他管理线程相关的类。下面是使用 C++ 多线程库的简单示例：

```cpp
#include <iostream>
#include <thread>

using namespace std;

void output(int i)
{
	cout << i << endl;
}

int main()
{

	for (uint8_t i = 0; i < 4; i++)
	{
		thread t(output, i);
		t.detach();
	}
	
	getchar();
	return 0;
}
```

在一个for循环内，创建4个线程分别输出数字0、1、2、3，并且在每个数字的末尾输出换行符。语句 `thread t(output, i)`创建一个线程t，该线程运行 `output`，第二个参数i是传递给 `output`的参数。t在创建完成后自动启动，`t.detach`表示该线程在后台允许，无需等待该线程完成，继续执行后面的语句。这段代码的功能是很简单的，如果是顺序执行的话，其结果很容易预测得到：

```shell
0
1
2
3
```

但是在并行多线程下，其执行的结果就多种多样了，比如下面就是代码一次运行的结果：

```shell
01

2
3
```

这就涉及到多线程编程最核心的问题： **资源竞争** 。

假设CPU有4核，可以同时执行4个线程，但是**控制台却只有一个，同时只能有一个线程拥有这个唯一的控制台**，将数字输出。将上面代码创建的四个线程进行编号：t0, t1, t2, t3，分别输出的数字：0,1,2,3。参照上图的执行结果，控制台的拥有权的转移如下：

+ t0 拥有控制台，输出了数字0，但是其没有来的及输出换行符，控制的拥有权却转移到了 t1；（0）
+ t1 完成自己的输出，t1 线程完成 （1\n）
+ 控制台拥有权转移给 t0，输出换行符 （\n）
+ t2 拥有控制台，完成输出 （2\n）
+ t3 拥有控制台，完成输出 （3\n)

由于控制台是系统资源，这里控制台拥有权的管理是操作系统完成的。但是，假如是多个线程共享进程空间的数据，这就需要自己写代码控制，每个线程何时能够拥有共享数据进行操作。

**共享数据的管理**以及 **线程间的通信** ，是多线程编程的两大核心。

### 线程管理

每个应用程序至少有一个进程，而每个进程至少有一个主线程，除了主线程外，在一个进程中还可以创建多个线程。每个线程都需要一个入口函数，入口函数返回退出，该线程也会退出，主线程就是以 main 函数作为入口函数的线程。

在 C++ 11 的线程库中，将线程的管理放在了类 std::thread 中，使用 std::thread 可以创建、启动一个线程，并可以将线程挂起、结束等操作。

#### 启动一个线程

C++ 11 的线程库启动一个线程是非常简单的，只需要创建一个 std::thread 对象，就会启动一个线程，并使用该 std::thread 对象来管理该线程。

```cpp
do_task();
std::thread(do_task);
```

这里创建 std::thread 传入的函数，实际上其构造函数需要的是可调用（callable）类型，只要是有函数调用类型的实例都是可以的。所以除了传递函数外，还可以使用：

+ lambda表达式: 使用lambda表达式启动线程输出数字

```cpp
for (int i = 0; i < 4; i++)
{
	thread t([i]{
		cout << i << endl;
	});
	t.detach();
}
```

+ 重载了()运算符的类的实例:使用重载了()运算符的类实现多线程数字输出

```cpp
class Task
{
public:
	void operator()(int i)
	{
		cout << i << endl;
	}
};

int main()
{

	for (uint8_t i = 0; i < 4; i++)
	{
		Task task;
		thread t(task, i);
		t.detach();
	}
}
```

把函数对象传入 std::thread 的构造函数时，要注意一个 C++ 的语法解析错误（C++’s most vexing parse）。向 std::thread 的构造函数中传入的是一个临时变量，而不是命名变量就会出现语法解析错误。如下代码：

```cpp
std::thread t(Task());
```

这里相当于声明了一个函数t，其返回类型为 thread，而不是启动了一个新的线程。可以使用新的初始化语法避免这种情况

```cpp
std::thread t{Task()};
```

当线程启动后， **一定要在和线程相关联的 thread 销毁前，确定以何种方式等待线程执行结束** 。

C++11 有两种方式来等待线程结束：

+ **detach 方式** ，启动的线程自主在后台运行，当前的代码继续往下执行，不等待新线程结束。前面代码所使用的就是这种方式。
  + 调用 detach 表示 thread 对象和其表示的线程完全分离；
  + 分离之后的线程是不在受约束和管制，会单独执行，直到执行完毕释放资源，可以看做是一个 daemon 线程；
  + 分离之后 thread 对象不再表示任何线程；
  + 分离之后 joinable() == false，即使还在执行；
+ **join 方式** ，等待启动的线程完成，才会继续往下执行。假如前面的代码使用这种方式，其输出就会0,1,2,3，因为每次都是前一个线程输出完成了才会进行下一个循环，启动下一个新线程。
  + 只有处于活动状态线程才能调用 join，可以通过 joinable() 函数检查;
  + joinable() == true 表示当前线程是活动线程，才可以调用 join 函数；
  + 默认构造函数创建的对象是 joinable() == false;
  + join 只能被调用一次，之后 joinable 就会变为 false，表示线程执行完毕；
  + 调用 ternimate() 的线程必须是 joinable() == false;
  + 如果线程不调用 join() 函数，即使执行完毕也是一个活动线程，即 joinable() == true，依然可以调用 join() 函数；

无论在何种情形，一定要在 thread 销毁前，调用 t.join 或者 t.detach，来决定线程以何种方式运行。

当使用 join 方式时，会阻塞当前代码，等待线程完成退出后，才会继续向下执行；

而使用 detach 方式则不会对当前代码造成影响，当前代码继续向下执行，创建的新线程同时并发执行，这时候需要特别注意： **创建的新线程对当前作用域的变量的使用** ，创建新线程的作用域结束后，有可能线程仍然在执行，这时局部变量随着作用域的完成都已销毁，如果线程继续使用局部变量的 **引用或者指针** ，会出现意想不到的错误，并且这种错误很难排查。例如：

```cpp
auto fn = [](const int *a)
{
    for (int i = 0; i < 10; i++)
    {
        cout << *a << endl;
    }
};

[fn]
{
    int a = 1010;
    thread t(fn, &a);
    t.detach();
}();
```

在 lambda 表达式中，使用 fn 启动了一个新的线程，在装个新的线程中使用了局部变量 a 的指针，并且将该线程的运行方式设置为 detach。这样，在 lambda 表达式执行结束后，变量 a 被销毁，但是在后台运行的线程仍然在使用已销毁变量 a 的指针，这样就可能会导致不正确的结果出现。

所以在以 detach 的方式执行线程时，要将线程访问的局部数据复制到线程的空间（使用值传递），一定要确保线程没有使用局部变量的引用或者指针，除非你能肯定该线程会在局部作用域结束前执行结束。

当然，使用 join 方式的话就不会出现这种问题，它会在作用域结束前完成退出。

#### 异常情况下等待线程完成

当决定以 detach 方式让线程在后台运行时，可以在创建 thread 的实例后立即调用 detach，这样线程就会后 thread 的实例分离，即使出现了异常 thread 的实例被销毁，仍然能保证线程在后台运行。

但线程以 join 方式运行时，需要在主线程的合适位置调用 join 方法，如果调用 join 前出现了异常，thread 被销毁，线程就会被异常所终结。为了避免异常将线程终结，或者由于某些原因，例如线程访问了局部变量，就要保证线程一定要在函数退出前完成，就要保证要在函数退出前调用 join

```cpp
void func() {
	thread t([]{
		cout << "hello C++ 11" << endl;
	});

	try
	{
		do_something_else();
	}
	catch (...)
	{
		t.join();
		throw;
	}
	t.join();
}
```

上面代码能够保证在正常或者异常的情况下，都会调用 join 方法，这样线程一定会在函数 func 退出前完成。但是使用这种方法，不但代码冗长，而且会出现一些作用域的问题，并不是一个很好的解决方法。

一种比较好的方法是资源获取即初始化（RAII,Resource Acquisition Is Initialization)，该方法提供一个类，在析构函数中调用 join。

```cpp
class thread_guard
{
	thread &t;
public :
	explicit thread_guard(thread& _t) :
		t(_t){}

	~thread_guard()
	{
		if (t.joinable())
			t.join();
	}

	thread_guard(const thread_guard&) = delete;
	thread_guard& operator=(const thread_guard&) = delete;
};

void func(){

	thread t([]{
		cout << "Hello thread" <<endl ;
	});

	thread_guard g(t);
}
```

无论是何种情况，当函数退出时，局部变量 g 调用其析构函数销毁，从而能够保证 join 一定会被调用。

#### 向线程传递参数

向线程调用的函数传递参数也是很简单的，只需要在构造 `thread`的实例时，依次传入即可。例如：

```cpp
void func(int *a,int n){}

int buffer[10];
thread t(func,buffer,10);
t.join();
```

需要注意的是，**默认的会将传递的参数以拷贝的方式复制到线程空间，即使参数的类型是引用。** 例如：

```cpp
void func(int a,const string& str);
thread t(func,3,"hello");
```

func 的第二个参数是 string &，而传入的是一个字符串字面量。该字面量以 const char* 类型传入线程空间后，在**线程的空间内转换为 `string`**。

如果在线程中使用引用来更新对象时，就需要注意了。默认的是将对象拷贝到线程空间，其引用的是拷贝的线程空间的对象，而不是初始希望改变的对象。如下：

```cpp
class _tagNode
{
public:
	int a;
	int b;
};

void func(_tagNode &node)
{
	node.a = 10;
	node.b = 20;
}

void f()
{
	_tagNode node;

	thread t(func, node);
	t.join();

	cout << node.a << endl ;
	cout << node.b << endl ;
}
```

在线程内，将对象的字段 a 和 b 设置为新的值，但是在线程调用结束后，这两个字段的值并不会改变。这样由于引用的实际上是局部变量 node 的一个拷贝，而不是 node 本身。在将对象传入线程的时候，调用  std::ref，将 node 的引用传入线程，而不是一个拷贝。例如：thread t(func,std::ref(node));

也可以使用类的成员函数作为线程函数，示例如下

```cpp
class _tagNode{

public:
	void do_some_work(int a);
};
_tagNode node;

thread t(&_tagNode::do_some_work, &node,20);
```

上面创建的线程会调用 node.do_some_work(20)，第三个参数为成员函数的第一个参数，以此类推。

#### 转移线程的所有权

thread 是可移动的 (movable) 的，但不可复制 (copyable)。可以通过 move 来改变线程的所有权，灵活的决定线程在什么时候 join 或者 detach。

```cpp
thread t1(f1);
thread t3(move(t1));
```

将线程从 t1 转移给 t3,这时候 t1 就不再拥有线程的所有权，调用 t1.join 或 t1.detach 会出现异常，要使用 t3 来管理线程。这也就意味着 thread 可以作为函数的返回类型，或者作为参数传递给函数，能够更为方便的管理线程。

线程的标识类型为 std::thread::id，有两种方式获得到线程的id。

+ 通过 thread 的实例调用 get_id() 直接获取
+ 在当前线程上调用 this_thread::get_id() 获取

## 智能指针

### 为什么要使用智能指针

一句话带过：智能指针就是帮我们 C++ 程序员管理动态分配的内存的，它会帮助我们自动释放 new 出来的内存，从而避免内存泄漏！

如下例子就是内存泄露的例子：

```cpp
#include <iostream>
#include <string>
#include <memory>

using namespace std;


// 动态分配内存，没有释放就return
void memoryLeak1() {
	string *str = new string("动态分配内存！");
	return;
}

// 动态分配内存，虽然有些释放内存的代码，但是被半路截胡return了
int memoryLeak2() {
	string *str = new string("内存泄露！");

	// ...此处省略一万行代码

	// 发生某些异常，需要结束函数
	if (1) {
		return -1;
	}
	/
	// 另外，使用try、catch结束函数，也会造成内存泄漏！
	/

	delete str;	// 虽然写了释放内存的代码，但是遭到函数中段返回，使得指针没有得到释放
	return 1;
}


int main(void) {

	memoryLeak1();

	memoryLeak2();

	return 0;
} 
```

memoryLeak1 函数中，new 了一个字符串指针，但是没有 delete 就已经 return 结束函数了，导致内存没有被释放，内存泄露！

memoryLeak2 函数中，new 了一个字符串指针，虽然在函数末尾有些释放内存的代码 delete str，但是在delete 之前就已经 return 了，所以内存也没有被释放，内存泄露！

使用指针，我们没有释放，就会造成内存泄露。但是我们使用普通对象却不会！

思考：如果我们分配的动态内存都交由有生命周期的对象来处理，那么在对象过期时，让它的析构函数删除指向的内存，这看似是一个 very nice 的方案？

智能指针就是通过这个原理来解决指针自动释放的问题！

C++98 提供了 auto_ptr 模板的解决方案

C++11 增加了 unique_ptr、shared_ptr 和 weak_ptr

### auto_ptr

auto_ptr 是 C++98 定义的智能指针模板，其定义了管理指针的对象，可以将 new 获得（直接或间接）的地址赋给这种对象。当对象过期时，其析构函数将使用 delete 来释放内存！

例：我们先定义一个类，类的构造函数和析构函数都输出一个字符串用作提示！

定义一个私有成员变量，赋值20.

再定义一个私有成员方法用于返回这个私有成员变量。

```cpp
class Test {
public:
	Test() { cout << "Test的构造函数..." << endl; }
	~Test() { cout << "Test的析构函数..." << endl; }

	int getDebug() { return this->debug; }

private:
	int debug = 20;
};
```

当我们直接new这个类的对象，却没有释放时。。。

```cpp
int main(void) {
	Test *test = new Test;

	return 0;
}
```

从结果可以看到，只是打印了构造函数这个字符串，而析构函数的字符却没有被打印，说明并没有调用析构函数！这就导致了内存泄露！

解决内存泄露的办法，要么手动 delete，要么使用智能指针！

使用智能指针：

```cpp
// 定义智能指针
auto_ptr<Test> test(new Test);
```

智能指针可以像普通指针那样使用：

```cpp
cout << "test->debug：" << test->getDebug() << endl;
cout << "(*test).debug：" << (*test).getDebug() << endl;
```

这时再试试：

```cpp
int main(void) {
	//Test *test = new Test;
	auto_ptr<Test> test(new Test);
	cout << "test->debug：" << test->getDebug() << endl;
	cout << "(*test).debug：" << (*test).getDebug() << endl;
	return 0;
}
```

自动调用了析构函数。

为什么智能指针可以像普通指针那样使用？？？

因为其里面重载了 * 和 -> 运算符， * 返回普通对象，而 -> 返回指针对象。

#### 智能指针的三个常用函数

get() 获取智能指针托管的指针地址

```cpp
// 定义智能指针
auto_ptr<Test> test(new Test);

Test *tmp = test.get();		// 获取指针返回
cout << "tmp->debug：" << tmp->getDebug() << endl;
```

但我们一般不会这样使用，因为都可以直接使用智能指针去操作，除非有一些特殊情况。

release() 取消智能指针对动态内存的托管

```cpp
// 定义智能指针
auto_ptr<Test> test(new Test);
Test *tmp2 = test.release();	// 取消智能指针对动态内存的托管
delete tmp2;	// 之前分配的内存需要自己手动释放
```

也就是智能指针不再对该指针进行管理，改由管理员进行管理。

reset() 重置智能指针托管的内存地址，如果地址不一致，原来的会被析构掉

```cpp
// 定义智能指针
auto_ptr<Test> test(new Test);

test.reset(); // 释放掉智能指针托管的指针内存，并将其置NULL

test.reset(new Test());	// 释放掉智能指针托管的指针内存，并将参数指针取代之
```

reset函数会将参数的指针(不指定则为 NULL)，与托管的指针比较，如果地址不一致，那么就会析构掉原来托管的指针，然后使用参数的指针替代之。然后智能指针就会托管参数的那个指针了。

#### 使用建议

尽可能不要将 auto_ptr 变量定义为全局变量或指针

```cpp
// 没有意义，全局变量也是一样
auto_ptr<Test> *tp = new auto_ptr<Test>(new Test);
```

除非自己知道后果，不要把 auto_ptr 智能指针赋值给同类型的另外一个智能指针；

```cpp
auto_ptr<Test> t1(new Test);
auto_ptr<Test> t2(new Test);
t1 = t2;	// 不要这样操作...
```

### auto_ptr 被 C++11 抛弃的主要原因

C++11 后 auto_ptr 已经被弃用，已使用 unique_ptr 替代。C++11 后不建议使用 auto_ptr。

+ 复制或者赋值都会改变资源的所有权

```cpp
// auto_ptr 被C++11抛弃的主要原因
auto_ptr<string> p1(new string("I'm Li Ming!"));
auto_ptr<string> p2(new string("I'm age 22."));

cout << "p1：" << p1.get() << endl;
cout << "p2：" << p2.get() << endl;

// p2赋值给p1后，首先p1会先将自己原先托管的指针释放掉，然后接收托管p2所托管的指针，
// 然后p2所托管的指针制NULL，也就是p1托管了p2托管的指针，而p2放弃了托管。
p1 = p2;
cout << "p1 = p2 赋值后：" << endl;
cout << "p1：" << p1.get() << endl;
cout << "p2：" << p2.get() << endl;
```

+ 在 STL 容器中使用 auto_ptr 存在着重大风险，因为容器内的元素必须支持可复制和可赋值

```cpp
vector<auto_ptr<string>> vec;
auto_ptr<string> p3(new string("I'm P3"));
auto_ptr<string> p4(new string("I'm P4"));

// 必须使用std::move修饰成右值，才可以进行插入容器中
vec.push_back(std::move(p3));
vec.push_back(std::move(p4));

cout << "vec.at(0)：" <<  *vec.at(0) << endl;
cout << "vec[1]：" <<  *vec[1] << endl;


// 风险来了：
vec[0] = vec[1];	// 如果进行赋值，问题又回到了上面一个问题中。
cout << "vec.at(0)：" << *vec.at(0) << endl;
cout << "vec[1]：" << *vec[1] << endl;
```

+ 不支持对象数组的内存管理

```cpp
auto_ptr<int[]> array(new int[5]);	// 不能这样定义
```

### unique_ptr

C++11 用更严谨的 unique_ptr 取代了 auto_ptr。unique_ptr 和 auto_ptr 用法几乎一样，除了一些特殊情况。

+ 无法进行左值复制赋值操作，但允许临时右值赋值构造和赋值

```cpp
unique_ptr<string> p1(new string("I'm Li Ming!"));
unique_ptr<string> p2(new string("I'm age 22."));

cout << "p1：" << p1.get() << endl;
cout << "p2：" << p2.get() << endl;

p1 = p2;					// 禁止左值赋值
unique_ptr<string> p3(p2);	// 禁止左值赋值构造

unique_ptr<string> p3(std::move(p1));
p1 = std::move(p2);	// 使用move把左值转成右值就可以赋值了，效果和auto_ptr赋值一样

cout << "p1 = p2 赋值后：" << endl;
cout << "p1：" << p1.get() << endl;
cout << "p2：" << p2.get() << endl;
```

+ 在 STL 容器中使用 unique_ptr，不允许直接赋值

```cpp
vector<unique_ptr<string>> vec;
unique_ptr<string> p3(new string("I'm P3"));
unique_ptr<string> p4(new string("I'm P4"));

vec.push_back(std::move(p3));
vec.push_back(std::move(p4));

cout << "vec.at(0)：" << *vec.at(0) << endl;
cout << "vec[1]：" << *vec[1] << endl;

vec[0] = vec[1];	/* 不允许直接赋值 */
vec[0] = std::move(vec[1]);		// 需要使用move修饰，使得程序员知道后果

cout << "vec.at(0)：" << *vec.at(0) << endl;
cout << "vec[1]：" << *vec[1] << endl;
```

+ 支持对象数组的内存管理

```cpp
// 会自动调用delete [] 函数去释放内存
unique_ptr<int[]> array(new int[5]);	// 支持这样定义
```

除了上面三项外，unique_ptr 的其余用法都与 auto_ptr 用法一致。

#### 构造

```cpp
class Test {
public:
	Test() { cout << "Test的构造函数..." << endl; }
	~Test() { cout << "Test的析构函数..." << endl; }

	void doSomething() { cout << "do something......" << endl; }
};


// 自定义一个内存释放其
class DestructTest {
	public:
	void operator()(Test *pt) {
		pt->doSomething();
		delete pt;
	}
};

// unique_ptr<T> up; 空的unique_ptr，可以指向类型为T的对象
unique_ptr<Test> t1;

// unique_ptr<T> up1(new T());	定义unique_ptr,同时指向类型为T的对象
unique_ptr<Test> t2(new Test);

// unique_ptr<T[]> up;	空的unique_ptr，可以指向类型为T[的数组对象
unique_ptr<int[]> t3;

// unique_ptr<T[]> up1(new T[]);	定义unique_ptr,同时指向类型为T的数组对象
unique_ptr<int[]> t4(new int[5]);

// unique_ptr<T, D> up();	空的unique_ptr，接受一个D类型的删除器D，使用D释放内存
unique_ptr<Test, DestructTest> t5;

// unique_ptr<T, D> up(new T());	定义unique_ptr,同时指向类型为T的对象，接受一个D类型的删除器D，使用删除器D来释放内存
unique_ptr<Test, DestructTest> t6(new Test);
```

#### 赋值

```cpp
unique_ptr<Test> t7(new Test);
unique_ptr<Test> t8(new Test);
t7 = std::move(t8);	// 必须使用移动语义，结果，t7的内存释放，t8的内存管理
t7->doSomething();
```

#### 主动释放对象

```cpp
unique_ptr<Test> t9(new Test);
t9 = NULL;
t9 = nullptr;
t9.reset();
```

#### 放弃对象的控制权

```cpp
Test *t10 = t9.release();
```

#### 重置

```cpp
t9.reset(new Test);
```

#### auto_ptr 与 unique_ptr 智能指针的内存管理陷阱

```cpp
auto_ptr<string> p1;
string *str = new string("智能指针的内存管理陷阱");
p1.reset(str);	// p1托管str指针
{
	auto_ptr<string> p2;
	p2.reset(str);	// p2接管str指针时，会先取消p1的托管，然后再对str的托管
}

// 此时p1已经没有托管内容指针了，为 NULL，在使用它就会内存报错！
cout << "str：" << *p1 << endl;
```

这是由于 auto_ptr 与 unique_ptr 的排他性所导致的。为了解决这样的问题，我们可以使用 shared_ptr 指针。

### shared_ptr

熟悉了 unique_ptr 后，其实我们发现 unique_ptr 这种排他型的内存管理并不能适应所有情况，有很大的局限。如果需要多个指针变量共享怎么办？

如果有一种方式，可以记录引用特定内存对象的智能指针数量，当复制或拷贝时，引用计数加1，当智能指针析构时，引用计数减1，如果计数为零，代表已经没有指针指向这块内存，那么我们就释放它。这就是 shared_ptr 采用的策略。

![](https://pic3.zhimg.com/v2-8d1c6a2e76d0ced5fcf8521048c17fe2_r.jpg)

#### 引用计数的使用

调用use_count函数可以获得当前托管指针的引用计数。

```cpp
class Person {
public:
	Person(int v) {
		this->no = v;
		cout << "构造函数 \t no = " << this->no << endl;
	}

	~Person() {
		cout << "析构函数 \t no = " << this->no << endl;
	}

private:
	int no;
};

// 仿函数，内存删除
class DestructPerson {
public:
	void operator() (Person *pt) {
		cout << "DestructPerson..." << endl;
		delete pt;
	}
};

shared_ptr<Person> sp1;

shared_ptr<Person> sp2(new Person(2));

// 获取智能指针管控的共享指针的数量	use_count()：引用计数
cout << "sp1	use_count() = " << sp1.use_count() << endl;
cout << "sp2	use_count() = " << sp2.use_count() << endl << endl;

// 共享
sp1 = sp2;

cout << "sp1	use_count() = " << sp1.use_count() << endl;
cout << "sp2	use_count() = " << sp2.use_count() << endl << endl;

shared_ptr<Person> sp3(sp1);
cout << "sp1	use_count() = " << sp1.use_count() << endl;
cout << "sp2	use_count() = " << sp2.use_count() << endl;
cout << "sp2	use_count() = " << sp3.use_count() << endl << endl;
```

如上代码，sp1 = sp2; 和 shared_ptr`<Person>` sp3(sp1); 就是在使用引用计数了。

sp1 = sp2; 表示 sp1 和 sp2 共同托管同一个指针，所以他们的引用计数为2。

shared_ptr`<Person>` sp3(sp1); 表示 sp1 和 sp2 和 sp3 共同托管同一个指针，所以他们的引用计数为3；

#### 构造

+ shared_ptr`<T>` sp1; 空的 shared_ptr,可以指向类型为 T 的对象

```cpp
shared_ptr<Person> sp1;
Person *person1 = new Person(1);
sp1.reset(person1);	// 托管person1
```

+ shared_ptr`<T>` sp2(new T()); 定义 shared_ptr,同时指向类型为 T 的对象

```cpp
shared_ptr<Person> sp2(new Person(2));
shared_ptr<Person> sp3(sp1);
```

+ shared_ptr<T[]> sp4; 空的 shared_ptr，可以指向类型为 T[] 的数组对象, C++17 后支持

```cpp
shared_ptr<Person[]> sp4;
```

+ shared_ptr<T[]> sp5(new T[] { … }); 指向类型为 T 的数组对象, C++17 后支持

```cpp
shared_ptr<Person[]> sp5(new Person[5] { 3, 4, 5, 6, 7 });
```

+ shared_ptr`<T>` sp6(NULL, D()); 定义空的 shared_ptr，接受一个 D 类型的删除器，使用 D 释放内存

```cpp
shared_ptr<Person> sp6(NULL, DestructPerson());
```

+ shared_ptr`<T>` sp7(new T(), D()); 定义 shared_ptr,指向类型为 T 的对象，接受一个 D 类型的删除器，使用 D 删除器来释放内存

```cpp
shared_ptr<Person> sp7(new Person(8), DestructPerson());
```

#### 初始化

+ 构造函数

```cpp
shared_ptr<int> up1(new int(10));  // int(10) 的引用计数为1
shared_ptr<int> up2(up1);  // 使用智能指针up1构造up2, 此时int(10) 引用计数为2
```

+ 使用 make_shared 初始化对象，分配内存效率更高(推荐使用): make_shared 函数的主要功能是在动态内存中分配一个对象并初始化它，返回指向此对象的 shared_ptr; 用法：

```cpp
// make_shared<类型>(构造类型对象需要的参数列表);
shared_ptr<int> up3 = make_shared<int>(2); // 多个参数以逗号','隔开，最多接受十个
shared_ptr<string> up4 = make_shared<string>("字符串");
shared_ptr<Person> up5 = make_shared<Person>(9);
```

#### 赋值

```cpp
shared_ptrr<int> up1(new int(10));  // int(10) 的引用计数为1
shared_ptr<int> up2(new int(11));   // int(11) 的引用计数为1
up1 = up2;	// int(10) 的引用计数减1,计数归零内存释放，up2共享int(11)给up1, int(11)的引用计数为2
```

#### 主动释放对象

```cpp
shared_ptr<int> up1(new int(10));
up1 = nullptr ;	// int(10) 的引用计数减1,计数归零内存释放 
// 或
up1 = NULL; // 作用同上 
```

#### 重置

```cpp
// p1是一个指针。
p.reset(); // 将p重置为空指针，所管理对象引用计数减1

p.reset(p1); // 将p重置为p1（的值）,p管控的对象计数减1，p接管对p1指针的管控

p.reset(p1,d); // 将p重置为p1（的值），p管控的对象计数减1并使用d作为删除器
```

#### 交换

```cpp
// p1和p2是智能指针

std::swap(p1,p2); // 交换p1和p2管理的对象，原对象的引用计数不变
p1.swap(p2);    // 交换p1和p2管理的对象，原对象的引用计数不变
```

#### shared_ptr 使用陷阱

shared_ptr作为被管控的对象的成员时，小心因循环引用造成无法释放资源。

如下代码：

```cpp
#include <iostream>
#include <string>
#include <memory>

using namespace std;

class Girl;

class Boy {
public:
	Boy() {
		cout << "Boy 构造函数" << endl;
	}

	~Boy() {
		cout << "~Boy 析构函数" << endl;
	}

	void setGirlFriend(shared_ptr<Girl> _girlFriend) {
		this->girlFriend = _girlFriend;
	}

private:
	shared_ptr<Girl> girlFriend;
};

class Girl {
public:
	Girl() {
		cout << "Girl 构造函数" << endl;
	}

	~Girl() {
		cout << "~Girl 析构函数" << endl;
	}

	void setBoyFriend(shared_ptr<Boy> _boyFriend) {
		this->boyFriend = _boyFriend;
	}

private:
	shared_ptr<Boy> boyFriend;
};


void useTrap() {
	shared_ptr<Boy> spBoy(new Boy());
	shared_ptr<Girl> spGirl(new Girl());

	// 陷阱用法
	spBoy->setGirlFriend(spGirl);
	spGirl->setBoyFriend(spBoy);
	// 此时boy和girl的引用计数都是2
}


int main(void) {
	useTrap();

	system("pause");
	return 0;
}
```

当我们执行 useTrap 函数时，注意，是没有结束此函数，boy 和 girl 指针其实是被两个智能指针托管的，所以他们的引用计数是2

![](https://pic2.zhimg.com/80/v2-b9e15f19374ee726108cdb714202c019_720w.webp)

useTrap 函数结束后，函数中定义的智能指针被清掉，boy 和 girl 指针的引用计数减1，还剩下1，对象中的智能指针还是托管他们的，所以函数结束后没有将 boy 和 gilr 指针释放的原因就是于此。

![](https://pic4.zhimg.com/v2-88ad02d0ddf69636b0b4ac2820b9a13b_r.jpg)

所以在使用shared_ptr智能指针时，要注意避免对象交叉使用智能指针的情况！ 否则会导致内存泄露。

当然，这也是有办法解决的，那就是使用weak_ptr弱指针。

针对上面的情况，还有另一种情况。如果是单方获得管理对方的共享指针，那么这样着是可以正常释放掉的！

```cpp
void useTrap() {
	shared_ptr`<Boy>` spBoy(new Boy());
	shared_ptr`<Girl>` spGirl(new Girl());

    // 单方获得管理
	// spBoy->setGirlFriend(spGirl);
	spGirl->setBoyFriend(spBoy);
}
```

反过来也是一样的。

首先释放 spBoy，但是因为 girl 对象里面的智能指针还托管着 boy，boy 的引用计数为2，所以释放 spBoy 时，引用计数减1，boy 的引用计数为1；

在释放 spGirl，girl 的引用计数减1，为零，开始释放 girl 的内存，因为 girl 里面还包含有托管 boy 的智能指针对象，所以也会进行 boyFriend 的内存释放，boy 的引用计数减1，为零，接着开始释放 boy 的内存。最终所有的内存都释放了。

### weak_ptr

weak_ptr 设计的目的是为配合 shared_ptr 而引入的一种智能指针来协助 shared_ptr 工作, 它只可以从一个 shared_ptr 或另一个 weak_ptr 对象构造, 它的构造和析构不会引起引用记数的增加或减少。同时 weak_ptr 没有重载*和->但可以使用 lock 获得一个可用的 shared_ptr 对象。

弱指针的使用；

```cpp
weak_ptr wpGirl_1; // 定义空的弱指针

weak_ptr wpGirl_2(spGirl); // 使用共享指针构造

wpGirl_1 = spGirl; // 允许共享指针赋值给弱指针
```

弱指针也可以获得引用计数:

```cpp
wpGirl_1.use_count()
```

弱指针不支持 * 和 -> 对指针的访问；

在必要的使用可以转换成共享指针 lock():

```cpp
shared_ptr`<Girl>` sp_girl;
sp_girl = wpGirl_1.lock();

// 使用完之后，再将共享指针置NULL即可
sp_girl = NULL;
```

在类中使用弱指针接管共享指针，在需要使用时就转换成共享指针去使用即可。

#### expired函数的用法

expired：判断当前 weak_ptr 智能指针是否还有托管的对象，有则返回 false，无则返回 true

如果返回 true，等价于 use_count() == 0，即已经没有托管的对象了；当然，可能还有析构函数进行释放内存，但此对象的析构已经临近（或可能已发生）。

示例

演示如何用 expired 检查指针的合法性。

```cpp
#include <iostream>
#include <memory>

std::weak_ptr<int> gw;

void f() {

    // expired：判断当前智能指针是否还有托管的对象，有则返回false，无则返回true
	if (!gw.expired()) {
		std::cout << "gw is valid\n";	// 有效的，还有托管的指针
	} else {
		std::cout << "gw is expired\n";	// 过期的，没有托管的指针
	}
}

int main() {
	{
		auto sp = std::make_shared<int>(42);
		gw = sp;

        f();
	}

    // 当{ }体中的指针生命周期结束后，再来判断其是否还有托管的指针
	f();

    return 0;
}
```

在 { } 中，gw 的生命周期还在，他还在托管着 make_shared 赋值的指针，所以调用 f() 函数时打印" gw is valid\n ";

当执行完 { } 后，gw 的生命周期已经结束，已经调用析构函数释放 make_shared 指针内存，gw 已经没有在托管任何指针了，调用 expired() 函数返回 true，所以打印" gw is expired\n ";

### 智能指针的使用陷阱

+ 不要把一个原生指针给多个智能指针管理。

```cpp
int *x = new int(10);

unique_ptr<int> up1(x);

unique_ptr<int> up2(x);

// 警告! 以上代码使up1 up2指向同一个内存,非常危险
```

或以下形式：

```cpp
up1.reset(x);

up2.reset(x);
```

+ 记得使用 u.release() 的返回值：在调用 u.release() 时是不会释放 u 所指的内存的，这时返回值就是对这块内存的唯一索引，如果没有使用这个返回值释放内存或是保存起来，这块内存就泄漏了。

+ 禁止 delete 智能指针 get 函数返回的指针：如果我们主动释放掉 get 函数获得的指针，那么智能指针内部的指针就变成野指针了，析构时造成重复释放，带来严重后果。

+ 禁止用任何类型智能指针 get 函数返回的指针去初始化另外一个智能指针。

```cpp
shared_ptr<int> sp1(new int(10));
// 一个典型的错误用法 shared_ptr<int> sp4(sp1.get());
```