# 第 17 课:模板

## 这一课你会学到

- 模板解决什么问题
- 函数模板
- 类模板
- 你已经在用模板了:`vector<int>` 里的尖括号

## 1. 问题:同样的逻辑,不同的类型

写一个返回两数中较大者的函数。对 `int` 是这样:

```cpp
int max_of(int a, int b) {
    return a > b ? a : b;
}
```

(`a > b ? a : b` 是简写的判断:如果 `a > b` 成立,结果是 `a`,否则是 `b`。)

如果还要比较 `double`,或者 `std::string`,就要再写一遍,内容完全相同,只有类型不同。第 6 课学过的重载可以做到,但要写很多份。

**模板**让你只写一份,类型留一个"空位",使用时由编译器去填。

## 2. 函数模板

```cpp
// file: func_template.cpp
#include <iostream>
#include <string>

template <typename T>
T max_of(T a, T b) {
    return a > b ? a : b;
}

int main() {
    std::cout << max_of(3, 8) << std::endl;
    std::cout << max_of(2.5, 1.5) << std::endl;
    std::cout << max_of(std::string("apple"), std::string("pear")) << std::endl;
    return 0;
}
```

运行结果:

```text
8
2.5
pear
```

- `template <typename T>` 的意思是:下面这个函数里有一个"类型空位",名字叫 `T`。`T` 只是个名字,你也可以叫别的,但 `T` 是惯例。
- 函数里的 `T` 就是空位。调用 `max_of(3, 8)` 时,编译器看到两个参数是 `int`,就自动把 `T` 换成 `int`,生成一份对应的函数。
- 调用时不用手动告诉它类型,编译器自己推断。

注意:`max_of(3, 2.5)` 会编译失败,因为两个参数类型不一样,编译器不知道 `T` 该是 `int` 还是 `double`。

## 3. 类模板

类也可以有类型空位。下面写一个最简单的"盒子",能装任意类型的一个值:

```cpp
// file: class_template.cpp
#include <iostream>
#include <string>

template <typename T>
class Box {
public:
    Box(T v) : value(v) {}

    T get() const { return value; }
    void set(T v) { value = v; }

private:
    T value;
};

int main() {
    Box<int> a(42);
    Box<std::string> b("你好");

    std::cout << a.get() << std::endl;
    std::cout << b.get() << std::endl;

    a.set(100);
    std::cout << a.get() << std::endl;
    return 0;
}
```

运行结果:

```text
42
你好
100
```

用类模板创建对象时,必须在类名后面的尖括号里写出具体类型:`Box<int>`、`Box<std::string>`。

**这和 `std::vector<int>`、`std::unique_ptr<int>` 是一回事。** 它们都是标准库写好的类模板,尖括号里填的就是 `T`。你前面已经用了很多次,现在知道它是怎么来的了。

## 4. 一个实用的例子:带检查的栈

"栈"这个词在第 11 课指内存区域。这里是另一个意思:一种数据结构,规则是"后放进去的先拿出来",像叠盘子。两个"栈"没有关系,只是名字相同。

```cpp
// file: stack_template.cpp
#include <iostream>
#include <stdexcept>
#include <vector>

template <typename T>
class Stack {
public:
    void push(const T& v) { items.push_back(v); }

    T pop() {
        if (items.empty()) {
            throw std::runtime_error("栈是空的");
        }
        T top = items.back();
        items.pop_back();
        return top;
    }

    bool empty() const { return items.empty(); }

private:
    std::vector<T> items;
};

int main() {
    Stack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);

    while (!s.empty()) {
        std::cout << s.pop() << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

运行结果:

```text
3 2 1 
```

最后放进去的 3 最先出来。

- 成员用 `std::vector<T>`,所以不需要写析构函数和拷贝函数(第 15 课的零法则)。
- `throw` 是抛出错误。栈空时还去拿数据,是调用者的错误。第 20 课会专门讲异常,现在先看着用。

## 5. 模板的几个特点

- **模板要写在头文件里。** 第 21 课讲多文件项目时会说明原因。
- **模板在编译时才"生成"具体代码。** 你没用过的类型,不会生成对应代码。
- **报错信息会很长。** 模板出错时,编译器的错误信息又长又难读。技巧:只看第一条,以及指向你自己代码的那一行。
- **类型必须支持模板里用到的操作。** `max_of` 里用了 `>`,所以只能用于支持 `>` 的类型。如果你传一个没有定义 `>` 的类型,编译会失败。

## 练习

1. 写函数模板 `swap_values(T& a, T& b)`,交换两个变量,用 `int` 和 `std::string` 测试。(提示:用到第 10 课的引用。)
2. 写函数模板 `sum_of(const std::vector<T>& v)`,返回所有元素之和。(提示:先创建一个 `T total = T();`,`T()` 会得到该类型的零值。)
3. 给 `Box` 添加一个成员函数 `print()`,输出里面的值。用三种不同类型测试。
4. 给 `Stack` 增加 `size()` 成员函数,返回里面有多少个元素。

## 小结

- 模板让同一份代码适用于多种类型,类型用 `T` 这样的名字占位。
- 函数模板:编译器根据参数自动推断类型。
- 类模板:创建对象时在尖括号里写出具体类型。
- `vector<int>`、`unique_ptr<int>` 都是标准库的类模板。
- 模板里用到的操作,要求被填入的类型都支持。
