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

> std::forward<T>(u) 有两个参数：T 与 u。 
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