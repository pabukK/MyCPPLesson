# 第 14 课:结构体与类

## 这一课你会学到

- 用 `struct` 把相关的数据打包在一起
- 给数据配上函数,组成"类"
- `public` 和 `private`:谁能碰里面的数据
- 构造函数:创建对象时自动做初始化

## 1. 为什么需要打包

要保存一个学生的信息:名字、年龄、分数。用三个独立变量,再有一百个学生,就要三百个变量,很乱。

**结构体**(`struct`)可以把几个变量打包成一个新类型。

```cpp
// file: struct_demo.cpp
#include <iostream>
#include <string>

struct Student {
    std::string name;
    int age;
    double score;
};

int main() {
    Student s = {"小明", 18, 92.5};
    std::cout << s.name << "," << s.age << " 岁," << s.score << " 分" << std::endl;

    s.score = 95;
    std::cout << "改后:" << s.score << std::endl;
    return 0;
}
```

运行结果:

```text
小明,18 岁,92.5 分
改后:95
```

- `struct Student { ... };` 定义了一个新类型,叫 `Student`。注意末尾有分号。
- 大括号里的变量叫这个类型的**成员**。
- `Student s` 创建一个 `Student` 类型的变量。这样的变量也叫**对象**。
- 用点号访问成员:`s.name`、`s.score`。(第 7 课的 `word.size()` 里的点,也是这个意思。)

## 2. 把函数也放进去:成员函数

学生信息常常要配合操作,比如"打印自己的信息"。把函数写在结构体里面,这样的函数叫**成员函数**。数据和操作数据的函数放在一起,就是面向对象的基本想法。

```cpp
// file: member_func.cpp
#include <iostream>
#include <string>

struct Student {
    std::string name;
    int age;
    double score;

    void print() {
        std::cout << name << "," << age << " 岁," << score << " 分" << std::endl;
    }
};

int main() {
    Student a = {"小明", 18, 92.5};
    Student b = {"小红", 17, 88};
    a.print();
    b.print();
    return 0;
}
```

运行结果:

```text
小明,18 岁,92.5 分
小红,17 岁,88 分
```

`print` 里直接用 `name`、`age`、`score`。是谁调用的,就用谁的成员:`a.print()` 里用 `a` 的,`b.print()` 里用 `b` 的。

## 3. struct 和 class

C++ 里还有另一个关键字 `class`,用法几乎和 `struct` 一样。唯一的区别是默认的访问权限:

- `struct`:成员默认**公开**,外面随便用。
- `class`:成员默认**私有**,外面不能直接用。

习惯上,只放数据、没有复杂规则的小东西用 `struct`;有规则要保护的东西用 `class`。

## 4. public 和 private:保护数据

假设年龄不能是负数。如果成员是公开的,谁都能写 `s.age = -5;`,你拦不住。

把成员设为**私有**(`private`),外面就碰不到,只能通过你提供的公开(`public`)函数来操作,你可以在函数里检查合不合理。

```cpp
// file: class_demo.cpp
#include <iostream>
#include <string>

class Student {
public:
    void set_age(int a) {
        if (a < 0) {
            std::cout << "年龄不能为负数,已忽略" << std::endl;
            return;
        }
        age = a;
    }

    int get_age() {
        return age;
    }

private:
    int age = 0;
};

int main() {
    Student s;
    s.set_age(18);
    std::cout << "年龄:" << s.get_age() << std::endl;

    s.set_age(-5);
    std::cout << "年龄:" << s.get_age() << std::endl;
    return 0;
}
```

运行结果:

```text
年龄:18
年龄不能为负数,已忽略
年龄:18
```

- `public:` 后面的成员,外面可以用。
- `private:` 后面的成员,只有类自己的函数能用。如果在 `main` 里写 `s.age = 5;`,编译直接报错。
- `int age = 0;` 给私有成员写了初始值,延续"变量要给初始值"的习惯。

这样做的好处:数据的规则集中写在类里面,不会被外面破坏。

## 5. 构造函数:创建时自动初始化

上面的类,创建出来以后还要自己调用 `set_age`。更好的办法:创建对象的同时就把数据准备好。这件事由**构造函数**负责。

构造函数是一个特殊的成员函数:名字和类名完全相同,没有返回类型。创建对象时,它自动运行。

```cpp
// file: constructor.cpp
#include <iostream>
#include <string>

class Student {
public:
    Student(std::string n, int a) {
        name = n;
        age = a;
        std::cout << "创建了学生:" << name << std::endl;
    }

    void print() {
        std::cout << name << "," << age << " 岁" << std::endl;
    }

private:
    std::string name;
    int age = 0;
};

int main() {
    Student s("小明", 18);
    s.print();
    return 0;
}
```

运行结果:

```text
创建了学生:小明
小明,18 岁
```

`Student s("小明", 18);` 一执行,构造函数就自动运行了,参数 `"小明"` 和 `18` 传给了 `n` 和 `a`。

一旦你写了带参数的构造函数,创建对象时就必须提供这些参数,不能再写 `Student s;`。这样就保证了每个对象一出生就是完整的。

## 6. 初始化列表

构造函数还有一种更标准的写法,把初始化放在冒号后面:

```cpp
Student(std::string n, int a) : name(n), age(a) {
    std::cout << "创建了学生:" << name << std::endl;
}
```

`: name(n), age(a)` 的意思是:创建 `name` 时直接用 `n` 初始化,创建 `age` 时直接用 `a` 初始化。上一种写法是先创建好空的 `name`,再赋值,多了一步。初始化列表更直接,以后写代码时优先使用。

## 7. const 成员函数

有些成员函数只读数据,不修改。在函数括号后面加 `const`,就是向编译器和读代码的人保证:这个函数不会改动对象。

```cpp
// file: const_member.cpp
#include <iostream>
#include <string>

class Student {
public:
    Student(std::string n, int a) : name(n), age(a) {}

    std::string get_name() const {
        return name;
    }

    int get_age() const {
        return age;
    }

private:
    std::string name;
    int age = 0;
};

void show(const Student& s) {
    std::cout << s.get_name() << "," << s.get_age() << " 岁" << std::endl;
}

int main() {
    Student s("小红", 17);
    show(s);
    return 0;
}
```

运行结果:

```text
小红,17 岁
```

注意 `show` 的参数是 `const Student&`(第 10 课:只读的别名)。通过只读的引用,只能调用带 `const` 的成员函数。所以**凡是只读数据的成员函数,都要加 `const`**,否则通过只读引用就用不了。

## 练习

1. 定义 `struct Point { double x; double y; };`,写函数计算两点之间的距离。(提示:需要 `#include <cmath>`,用 `std::sqrt`。)
2. 写一个 `class Rectangle`,有私有的 `width` 和 `height`,构造函数接收这两个值,提供 `area()` 和 `perimeter()` 两个 `const` 成员函数。
3. 给 `Rectangle` 的构造函数加检查:如果宽或高不是正数,输出提示并改成 1。
4. 创建一个 `std::vector<Student>`,放进几个学生,用范围 for 遍历并打印。

## 小结

- `struct` 把相关的数据打包成新类型,用点号访问成员。
- 把函数放进去,就是成员函数。
- `class` 默认私有。`private` 保护数据,`public` 提供操作。
- 构造函数名字和类名相同,创建对象时自动运行,用初始化列表初始化成员。
- 只读不改的成员函数,加 `const`。
