# 第 21 课:多文件项目与头文件

## 这一课你会学到

- 为什么要把代码拆成多个文件
- 头文件(`.h`)和源文件(`.cpp`)各放什么
- 怎么把多个文件编译成一个程序
- 头文件保护:防止重复包含

## 1. 为什么要拆文件

前面所有程序都写在一个 `.cpp` 里。真实的项目有几千几万行,全放一个文件里,没法读,没法改,多人也没法分工。

所以要把代码按功能拆开。比如一个学生管理程序,可以拆成:

- 学生类的代码放一个文件
- 读写文件的代码放一个文件
- `main` 放一个文件

## 2. 头文件和源文件

C++ 把每个功能分成两个文件:

| 文件 | 后缀 | 放什么 |
| --- | --- | --- |
| 头文件 | `.h` | **声明**:告诉别人"有这些函数、这些类" |
| 源文件 | `.cpp` | **定义**:具体怎么实现 |

第 6 课讲过:声明只说"有这个函数",定义才写具体内容。现在你知道声明为什么有用了:别的文件只要看头文件里的声明,就知道怎么调用,不用关心实现。

举个例子:一个做数学运算的小模块,有两个文件。

头文件 `math_utils.h`:

```cpp
// file: math_utils.h
#pragma once

int add(int a, int b);
int square(int x);
```

源文件 `math_utils.cpp`:

<!-- nobuild: 1 -->
```cpp
// file: math_utils.cpp
#include "math_utils.h"

int add(int a, int b) {
    return a + b;
}

int square(int x) {
    return x * x;
}
```

使用它的 `main.cpp`:

<!-- sources: main.cpp math_utils.cpp -->
```cpp
// file: main.cpp
#include <iostream>
#include "math_utils.h"

int main() {
    std::cout << add(3, 4) << std::endl;
    std::cout << square(5) << std::endl;
    return 0;
}
```

## 3. 编译多个文件

把所有 `.cpp` 一起交给编译器:

```bash
g++ -std=c++17 -Wall -o app main.cpp math_utils.cpp
./app
```

输出:

```text
7
25
```

注意:**只有 `.cpp` 要写在命令里,`.h` 不用写。** `.h` 是通过 `#include` 被带进去的。

## 4. #include 到底做了什么

`#include "math_utils.h"` 的作用很简单:**把那个文件的全部内容,原样复制粘贴到这一行的位置。** 就这样。

所以 `main.cpp` 看到了 `add` 和 `square` 的声明,编译器知道"有这样的函数,参数和返回值是这样",就允许你调用。至于函数具体怎么写,编译器编译 `main.cpp` 时不关心。

完整的过程分两步:

1. **编译**:每个 `.cpp` 文件分别翻译成机器指令的半成品(叫**目标文件**)。
2. **链接**:把所有目标文件拼起来,把"调用"和"定义"对上,生成最终程序。

这两步 g++ 会自动完成。但出错时要分清,因为错误信息不一样:

| 阶段 | 典型错误 | 原因 |
| --- | --- | --- |
| 编译 | `was not declared in this scope` | 没有 `#include` 对应的头文件,编译器不认识这个名字 |
| 链接 | `undefined reference to 'add'` | 声明有了,但命令里漏了 `math_utils.cpp`,找不到定义 |

## 5. 双引号和尖括号

- `#include <iostream>`:尖括号,用于标准库和系统的头文件。
- `#include "math_utils.h"`:双引号,用于你自己项目里的头文件。

## 6. 头文件保护

假设 `a.h` 里包含了 `b.h`,`main.cpp` 里同时包含了 `a.h` 和 `b.h`。因为 `#include` 是复制粘贴,`b.h` 的内容会出现两次,导致"重复定义"的错误。

解决办法:每个头文件第一行写 `#pragma once`,意思是"这个文件只粘贴一次"。上面的 `math_utils.h` 就这样写了。**所有自己写的头文件,第一行都加上它。**

(还有一种更老的写法 `#ifndef`/`#define`/`#endif`,作用相同,但更长。读别人的代码时会看到。)

## 7. 把一个类拆成两个文件

类也是同样的做法:头文件里写类的"样子"(有哪些成员、哪些成员函数),源文件里写成员函数的实现。在源文件里实现成员函数时,函数名前要加 `类名::`,告诉编译器"这是 Student 类的函数"。

头文件 `student.h`:

```cpp
// file: student.h
#pragma once
#include <string>

class Student {
public:
    Student(std::string name, int score);

    std::string get_name() const;
    bool passed() const;

private:
    std::string name;
    int score = 0;
};
```

源文件 `student.cpp`:

<!-- nobuild: 1 -->
```cpp
// file: student.cpp
#include "student.h"

Student::Student(std::string n, int s) : name(n), score(s) {}

std::string Student::get_name() const {
    return name;
}

bool Student::passed() const {
    return score >= 60;
}
```

主程序 `student_main.cpp`:

<!-- sources: student_main.cpp student.cpp -->
```cpp
// file: student_main.cpp
#include <iostream>
#include "student.h"

int main() {
    Student s("小明", 75);
    std::cout << s.get_name() << (s.passed() ? " 及格" : " 不及格") << std::endl;
    return 0;
}
```

编译并运行:

```bash
g++ -std=c++17 -Wall -o student_app student_main.cpp student.cpp
./student_app
```

输出:

```text
小明 及格
```

头文件里的 `Student(std::string name, int score);` 只写声明,参数名可以省略,也可以写上方便阅读。成员函数的实现全部放在 `student.cpp` 里。

## 8. 模板要写在头文件里

第 17 课说过"模板要写在头文件里",现在可以解释原因了。

模板在使用时,编译器才根据具体类型生成代码。生成时,它必须**看得到模板的完整内容**(不只是声明)。如果模板的实现放在 `.cpp` 里,别的文件包含头文件时只看到声明,编译器没法生成代码,链接时就会报 `undefined reference`。

所以:**模板的声明和实现,都写在头文件里。**

## 9. 一个规范的项目长什么样

```text
my_project/
├── src/
│   ├── main.cpp
│   ├── student.cpp
│   └── math_utils.cpp
└── include/
    ├── student.h
    └── math_utils.h
```

头文件和源文件分开放,是常见做法。下一课会用 CMake 来管理这样的结构,不用每次手敲一长串编译命令。

## 练习

1. 把第 14 课的 `Rectangle` 类拆成 `rectangle.h` 和 `rectangle.cpp`,再写 `main.cpp` 使用它,用 g++ 一次编译三个文件。
2. 故意在编译命令里漏掉 `rectangle.cpp`,看看出现什么错误,是编译错误还是链接错误?
3. 把 `#pragma once` 去掉,在 `main.cpp` 里同时 `#include` 同一个头文件两次,看看报什么错。(提示:头文件里要有类的定义。)

## 小结

- 把代码拆成多个文件,每个文件负责一块功能。
- 头文件(`.h`)放声明,源文件(`.cpp`)放实现。
- `#include` 就是把文件内容复制到这个位置。
- 编译命令里列出所有 `.cpp`,不列 `.h`。
- 头文件第一行写 `#pragma once`。
- 成员函数在 `.cpp` 里实现时,写 `类名::函数名`。
- 模板的全部内容写在头文件里。
- 编译错误:编译器不认识某个名字。链接错误:`undefined reference`,找不到定义。
