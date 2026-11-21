# 第 6 课:函数

## 这一课你会学到

- 函数是什么,为什么需要它
- 怎么定义和调用函数
- 参数和返回值
- 函数的声明与重载

## 1. 函数是什么

到目前为止,所有代码都写在 `main` 里。程序变大以后,`main` 会变得又长又乱。

**函数**是一段有名字的代码,可以反复使用。你其实早就用过函数了:`main` 就是一个函数。把一件事写成函数,以后想做这件事,只要喊一下它的名字。

## 2. 定义和调用

```cpp
// file: hello_func.cpp
#include <iostream>

void say_hello() {
    std::cout << "你好!" << std::endl;
}

int main() {
    say_hello();
    say_hello();
    return 0;
}
```

运行结果:

```text
你好!
你好!
```

- `void` 表示这个函数不给调用者返回任何东西。
- `say_hello` 是函数名。
- 在 `main` 里写 `say_hello();`,就叫**调用**函数。调用一次,函数里的代码就执行一遍。

函数要写在调用它的地方**前面**,编译器是从上往下读的,没读到的名字它不认识。

## 3. 参数:给函数传数据

函数常常需要外面给它一些数据才能干活。这些数据写在函数名后面的小括号里,叫**参数**。

```cpp
// file: greet.cpp
#include <iostream>
#include <string>

void greet(std::string name, int times) {
    for (int i = 0; i < times; i++) {
        std::cout << "你好," << name << "!" << std::endl;
    }
}

int main() {
    greet("小明", 2);
    return 0;
}
```

运行结果:

```text
你好,小明!
你好,小明!
```

定义函数时,要写清每个参数的类型和名字,用逗号隔开。调用时,按同样的顺序给出具体的值。这里 `"小明"` 传给了 `name`,`2` 传给了 `times`。

## 4. 返回值:函数把结果交回来

函数算出结果后,可以用 `return` 把结果交还给调用它的地方。函数名前面写的类型,就是返回值的类型。

```cpp
// file: add.cpp
#include <iostream>

int add(int a, int b) {
    return a + b;
}

int main() {
    int result = add(3, 4);
    std::cout << "3 + 4 = " << result << std::endl;
    std::cout << "10 + 20 = " << add(10, 20) << std::endl;
    return 0;
}
```

运行结果:

```text
3 + 4 = 7
10 + 20 = 30
```

`return` 一执行,函数立刻结束,并把值交出去。`add(3, 4)` 这个整体,就等于它返回的值 7,所以可以赋给变量,也可以直接放在 `cout` 里输出。

## 5. 参数是"复印件"

调用函数时,传进去的是值的**拷贝**(复印件)。函数里改的是复印件,外面的原变量不受影响。

```cpp
// file: copy_demo.cpp
#include <iostream>

void add_one(int x) {
    x = x + 1;
    std::cout << "函数里:" << x << std::endl;
}

int main() {
    int n = 5;
    add_one(n);
    std::cout << "函数外:" << n << std::endl;
    return 0;
}
```

运行结果:

```text
函数里:6
函数外:5
```

`n` 还是 5。想让函数真正修改外面的变量,需要用到指针或引用,这是第 9、10 课的内容。现在先记住这条规则:**普通参数是复印件。**

## 6. 变量的作用范围

在大括号里创建的变量,只能在这对大括号里使用,出了大括号就消失了。这个"能用到哪里"的范围叫**作用域**。

```cpp
// file: scope_demo.cpp
#include <iostream>

void test() {
    int inner = 10;
    std::cout << inner << std::endl;
}

int main() {
    test();
    int outer = 20;
    std::cout << outer << std::endl;
    return 0;
}
```

运行结果:

```text
10
20
```

`inner` 只存在于 `test` 里,在 `main` 里写 `inner` 会编译失败。同样,`for` 括号里的 `int i` 只在这个循环里有效。变量在作用域结束时被销毁,它占的内存会被收回。这一点在内存课里非常重要,请记住。

## 7. 先声明,后定义

函数必须先出现才能被调用。但程序大了以后,不方便总把函数排在前面。这时可以先写一行**声明**,只告诉编译器"有这样一个函数",具体内容放到后面。

```cpp
// file: declare.cpp
#include <iostream>

int square(int x);

int main() {
    std::cout << square(5) << std::endl;
    return 0;
}

int square(int x) {
    return x * x;
}
```

运行结果:

```text
25
```

声明就是函数的第一行加分号,没有大括号。带大括号、写了具体代码的,叫**定义**。第 21 课做多文件项目时,声明会很有用。

## 8. 默认参数与重载

参数可以有默认值,调用时不写就用默认的:

```cpp
// file: default_arg.cpp
#include <iostream>

double area(double width, double height = 1.0) {
    return width * height;
}

int main() {
    std::cout << area(3.0, 4.0) << std::endl;
    std::cout << area(3.0) << std::endl;
    return 0;
}
```

运行结果:

```text
12
3
```

C++ 还允许几个函数同名,只要参数的个数或类型不同,编译器会根据你传的参数自动选择。这叫**重载**:

```cpp
// file: overload.cpp
#include <iostream>

void show(int x) {
    std::cout << "整数:" << x << std::endl;
}

void show(double x) {
    std::cout << "小数:" << x << std::endl;
}

int main() {
    show(5);
    show(5.5);
    return 0;
}
```

运行结果:

```text
整数:5
小数:5.5
```

## 练习

1. 写函数 `is_even(int n)`,返回 `bool`,判断是否为偶数,在 `main` 里测试。
2. 写函数 `max_of(int a, int b)`,返回较大的数。
3. 写函数 `factorial(int n)`,返回 n 的阶乘(1 乘到 n)。
4. 写一个函数尝试交换两个整数,看看为什么交换不成功。(答案在第 10 课揭晓。)

## 小结

- 函数是有名字、可重复使用的代码。
- 参数传进去的是复印件,改它不影响原变量。
- `return` 交回结果,函数立刻结束。
- 大括号里的变量出了大括号就消失。
- 声明只说"有这个函数",定义才写具体内容。
- 同名函数只要参数不同,就是重载。

