# 第 15 课:析构、拷贝与移动

## 这一课你会学到

- 析构函数:对象销毁时自动运行的函数
- 类自己管理堆内存时,会遇到什么问题
- 拷贝构造函数:让复制出来的对象各自独立
- 移动:不复制,直接把内容"搬"过去
- 最省心的办法:让标准库替你管

## 1. 析构函数

上一课学了构造函数:对象创建时自动运行。与它对应的是**析构函数**:对象销毁时自动运行。

析构函数的名字是类名前面加 `~`,没有参数,没有返回类型。

```cpp
// file: destructor.cpp
#include <iostream>
#include <string>

class Noisy {
public:
    Noisy(std::string n) : name(n) {
        std::cout << "创建 " << name << std::endl;
    }
    ~Noisy() {
        std::cout << "销毁 " << name << std::endl;
    }

private:
    std::string name;
};

int main() {
    Noisy a("A");
    {
        Noisy b("B");
        std::cout << "内层大括号结束前" << std::endl;
    }
    std::cout << "内层大括号结束后" << std::endl;
    return 0;
}
```

运行结果:

```text
创建 A
创建 B
内层大括号结束前
销毁 B
内层大括号结束后
销毁 A
```

规律:对象离开它所在的大括号时销毁,销毁时自动调用析构函数。先创建的后销毁(`B` 先销毁,`A` 后销毁)。

这正是第 13 课 RAII 的原理:智能指针、`vector` 都是靠析构函数,在销毁时归还堆内存。现在你可以自己写一个这样的类了。

## 2. 自己管理堆内存的类

下面这个类在堆上放一排 `int`,并在析构函数里归还:

```cpp
// file: int_array.cpp
#include <iostream>

class IntArray {
public:
    IntArray(int n) : size(n), data(new int[n]) {
        for (int i = 0; i < size; i++) {
            data[i] = 0;
        }
    }
    ~IntArray() {
        delete[] data;
    }

    void set(int i, int v) { data[i] = v; }
    int get(int i) const { return data[i]; }

private:
    int size;
    int* data;
};

int main() {
    IntArray a(3);
    a.set(0, 10);
    std::cout << a.get(0) << std::endl;
    return 0;
}
```

运行结果:

```text
10
```

构造函数 `new`,析构函数 `delete[]`,配对工整。到这里没有问题。

## 3. 复制带来的问题

如果复制这个对象,会怎样?默认情况下,编译器生成的复制,是**把每个成员原样抄一份**。对 `size` 来说没问题。对指针 `data` 来说,抄的是**地址**,不是地址指向的内容:

```text
对象 a:  size=3, data -> [堆上那一排 int]
对象 b = a 之后:
对象 b:  size=3, data -> [同一排 int]   <- 两个指针指向同一块堆内存
```

`a` 和 `b` 共用一块堆内存。它们销毁时,各自在析构函数里 `delete[] data`,同一块内存归还了两次。这就是第 12 课讲过的**重复归还**。

<!-- expect: crash
flags: -fsanitize=address -g -->
```cpp
// file: bad_copy.cpp
#include <iostream>

class IntArray {
public:
    IntArray(int n) : size(n), data(new int[n]) {}
    ~IntArray() { delete[] data; }

private:
    int size;
    int* data;
};

int main() {
    IntArray a(3);
    IntArray b = a;
    std::cout << "复制完成" << std::endl;
    return 0;
}
```

这段代码**能编译**,但运行时会崩溃。用 AddressSanitizer 检查,会看到 `attempting double-free`。

## 4. 解决办法一:自己写拷贝构造函数

要让复制出来的对象**各自拥有独立的堆内存**,需要自己写**拷贝构造函数**:它规定"用一个已有对象创建新对象"时怎么做。做法是:申请新的内存,再把内容逐个抄过来。

```cpp
// file: deep_copy.cpp
#include <iostream>

class IntArray {
public:
    IntArray(int n) : size(n), data(new int[n]) {
        for (int i = 0; i < size; i++) {
            data[i] = 0;
        }
    }

    IntArray(const IntArray& other) : size(other.size), data(new int[other.size]) {
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    ~IntArray() { delete[] data; }

    void set(int i, int v) { data[i] = v; }
    int get(int i) const { return data[i]; }

private:
    int size;
    int* data;
};

int main() {
    IntArray a(3);
    a.set(0, 10);

    IntArray b = a;
    b.set(0, 99);

    std::cout << "a[0] = " << a.get(0) << std::endl;
    std::cout << "b[0] = " << b.get(0) << std::endl;
    return 0;
}
```

运行结果:

```text
a[0] = 10
b[0] = 99
```

拷贝构造函数的参数是 `const IntArray&`(只读的别名),这是固定写法。它里面 `new` 了一块新内存,所以 `a` 和 `b` 各管各的,修改 `b` 不影响 `a`,销毁时也各自归还各自的,不会重复。

这种"连指向的内容一起复制"的做法,叫**深拷贝**。上一节那种只抄地址的,叫**浅拷贝**。

还有一个同类的问题:`b = a;`(给已经存在的对象赋值)走的是另一个函数,叫**拷贝赋值运算符**,也需要同样处理。本课不展开写它,因为下一节有更好的办法。

## 5. 解决办法二(推荐):不自己管内存

深拷贝要写不少代码,而且很容易漏。`vector` 早就帮你做好了这一切:复制时自动深拷贝,销毁时自动归还。

```cpp
// file: use_vector.cpp
#include <iostream>
#include <vector>

class IntArray {
public:
    IntArray(int n) : data(n, 0) {}

    void set(int i, int v) { data.at(i) = v; }
    int get(int i) const { return data.at(i); }

private:
    std::vector<int> data;
};

int main() {
    IntArray a(3);
    a.set(0, 10);

    IntArray b = a;
    b.set(0, 99);

    std::cout << "a[0] = " << a.get(0) << std::endl;
    std::cout << "b[0] = " << b.get(0) << std::endl;
    return 0;
}
```

运行结果:

```text
a[0] = 10
b[0] = 99
```

结果和手写深拷贝完全一样,但:没有 `new`,没有 `delete`,没有析构函数,没有拷贝构造函数。(`data(n, 0)` 的意思:创建 `n` 个元素,每个都是 0。)

这引出一条重要的经验,叫"零法则":

> **如果类的成员都是 `vector`、`string`、智能指针这样自己会管理内存的类型,你就什么特殊函数都不用写。**

编译器默认生成的复制、销毁,会自动调用各个成员自己的版本,全部正确。

## 6. 移动:不复制,直接搬

复制有成本:一个装了一百万个元素的 `vector`,复制就要把一百万个元素全抄一遍。但有时候,原来的对象马上就不用了,比如函数返回的临时结果。这时候没必要抄,直接把堆内存的"所有权"搬给新对象就行,原对象留下一个空壳。

这叫**移动**。第 13 课你已经见过 `std::move`,它的作用就是告诉编译器:"这个对象我不要了,可以直接搬走它的内容。"

```cpp
// file: move_vector.cpp
#include <iostream>
#include <utility>
#include <vector>

int main() {
    std::vector<int> a = {1, 2, 3, 4, 5};
    std::vector<int> b = std::move(a);

    std::cout << "b 的个数:" << b.size() << std::endl;
    std::cout << "a 的个数:" << a.size() << std::endl;
    return 0;
}
```

运行结果:

```text
b 的个数:5
a 的个数:0
```

`b` 直接接管了 `a` 的堆内存,没有复制任何元素。`a` 被搬空了。

搬走以后,**不要再使用 `a` 里原有的内容**。给 `a` 重新赋一个新值,或者让它销毁,都是安全的。

好消息是:函数返回局部的 `vector` 或 `string` 时,编译器会自动选择搬而不是抄,你不用手写 `std::move`:

```cpp
// file: return_vector.cpp
#include <iostream>
#include <vector>

std::vector<int> make_numbers(int n) {
    std::vector<int> result;
    for (int i = 1; i <= n; i++) {
        result.push_back(i);
    }
    return result;
}

int main() {
    std::vector<int> nums = make_numbers(5);
    std::cout << "个数:" << nums.size() << std::endl;
    return 0;
}
```

运行结果:

```text
个数:5
```

直接 `return result;`,不需要 `std::move`。这里的结果既不会复制一份,也不会有"返回局部变量引用"的悬空问题(第 10 课),因为返回的是值,不是引用。

**所以函数想返回大对象,就直接按值返回,不用担心慢。**

## 7. 三种情况小结

| 你做的事 | 默认会怎样 | 要不要自己写 |
| --- | --- | --- |
| 成员都是 `vector`、`string` | 复制、移动、销毁都自动正确 | **什么都不用写** |
| 成员里有 `unique_ptr` | **不能复制**,只能移动(复制会编译报错) | **什么都不用写** |
| 成员里有自己 `new` 出来的裸指针 | 复制会出错 | 需要写析构、拷贝构造、拷贝赋值,最好改用 `vector` 或智能指针 |
| 类里有别的资源(文件、连接) | 取决于成员类型 | 同样优先用能自己管理的类型 |

自己写移动构造函数,属于更深的话题,本课程不展开。照着上面的经验走,你基本不会需要它。

## 练习

1. 在 `Noisy` 类里加一个拷贝构造函数,让它在被复制时输出"复制 X"。写代码复制它,观察输出顺序。
2. 把第 14 课的 `Student` 放进 `std::vector<Student>`,再复制整个 vector,修改副本里的学生,确认原来的不受影响。
3. 写一个类 `Buffer`,成员是 `std::unique_ptr<int>`。尝试复制它,看编译器怎么说。再试着用 `std::move` 移动它。

## 小结

- 析构函数 `~类名()` 在对象销毁时自动运行。
- 类里直接 `new` 出来的指针,复制时只抄地址(浅拷贝),会造成重复归还。
- 拷贝构造函数可以做深拷贝:各自 `new` 新内存,把内容抄过来。
- 最省事的办法:成员用 `vector`、`string`、智能指针,什么特殊函数都不用写。
- `std::move` 表示"这个对象我不要了,可以直接搬走内容",搬走后原对象不要再用。
- 函数返回大对象,直接按值返回就行。
