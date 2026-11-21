# 第 10 课:引用

## 这一课你会学到

- 引用是什么,和指针有什么区别
- 用引用让函数修改外面的变量
- 用 `const` 引用避免不必要的复制

## 1. 引用就是"别名"

指针要写 `*` 和 `&`,有点麻烦。C++ 还提供了一种更简单的方式:**引用**。

引用是变量的**另一个名字**。就像一个人有大名和小名,叫哪个都是同一个人。

```cpp
// file: ref_basic.cpp
#include <iostream>

int main() {
    int a = 10;
    int& r = a;

    r = 20;
    std::cout << "a = " << a << std::endl;
    std::cout << "r = " << r << std::endl;
    return 0;
}
```

运行结果:

```text
a = 20
r = 20
```

`int& r = a;` 的意思是:`r` 是 `a` 的另一个名字。改 `r` 就是改 `a`,因为它们是同一块内存。

这里的 `&` 写在类型后面,表示"引用"。第 8 课里 `&a` 的 `&` 写在变量前面,表示"取地址"。位置不同,意思不同。

## 2. 引用的规则

- **创建时必须绑定一个变量。** 不能只写 `int& r;`。
- **绑定以后不能换。** `r` 一旦成了 `a` 的别名,就永远是 `a` 的别名。
- **没有"空引用"。** 所以不需要检查是不是空的。

和指针对比一下:

| | 指针 | 引用 |
| --- | --- | --- |
| 能为空吗 | 能(`nullptr`) | 不能 |
| 能改指向吗 | 能 | 不能 |
| 使用时 | 要写 `*p` | 直接用,像普通变量 |

引用更简单,也更安全。**能用引用解决的,就优先用引用。** 只有需要"可以为空"或"中途换对象"时才用指针。

## 3. 引用做函数参数

引用最常见的用途:让函数直接操作外面的变量,不是它的复印件。第 6 课留下的交换问题,现在能彻底解决了:

```cpp
// file: swap_ref.cpp
#include <iostream>

void swap_values(int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
}

int main() {
    int x = 3;
    int y = 8;
    swap_values(x, y);
    std::cout << "x = " << x << ", y = " << y << std::endl;
    return 0;
}
```

运行结果:

```text
x = 8, y = 3
```

参数 `int& a` 是 `x` 的别名,`int& b` 是 `y` 的别名。函数里交换 `a` 和 `b`,就是交换 `x` 和 `y`。调用时不用写 `&`,直接传变量就行,比指针写法清爽。

## 4. 复制的代价和 const 引用

普通参数传进去会**复制**一份。对 `int` 来说无所谓,但如果传的是一个装了一百万个元素的 `vector`,或者一大段文字,每次调用都复制一遍,既慢又费内存。

用引用就不用复制。再加上 `const`,函数就只能读,不能改,调用者可以放心:

```cpp
// file: const_ref.cpp
#include <iostream>
#include <string>
#include <vector>

int sum(const std::vector<int>& nums) {
    int total = 0;
    for (int n : nums) {
        total += n;
    }
    return total;
}

void print_name(const std::string& name) {
    std::cout << "名字:" << name << std::endl;
}

int main() {
    std::vector<int> data = {1, 2, 3, 4, 5};
    std::cout << "总和:" << sum(data) << std::endl;
    print_name("小明");
    return 0;
}
```

运行结果:

```text
总和:15
名字:小明
```

`const std::vector<int>& nums` 读作:"一个 vector 的只读别名"。没有复制,也不会被改动。

**经验规则:**

- `int`、`double`、`char`、`bool` 这类小类型:直接按值传(复印件)。
- `string`、`vector` 这类大类型,只读用:`const 类型&`。
- 需要函数修改外面的变量:`类型&`。

## 5. 千万别返回局部变量的引用

第 6 课说过,函数里创建的变量,函数结束就销毁。如果返回它的引用,就是把一个已经拆掉的房子的门牌号交给别人:

```cpp
int& bad() {
    int x = 10;
    return x;     // 错误:x 在函数结束时就没了
}
```

编译器通常会给出警告。这类错误叫**悬空引用**。指针也有同样的问题,叫**悬空指针**,后面的课会详细讲。现在记住:**不要返回局部变量的引用或指针。**

## 练习

1. 写函数 `void double_it(int& x)`,把 `x` 变成原来的 2 倍。
2. 写函数 `int max_in(const std::vector<int>& v)`,返回最大的元素。
3. 写函数 `void trim_last(std::string& s)`,删掉字符串最后一个字符。(提示:`s.pop_back()`。)

## 小结

- 引用是变量的别名,`int& r = a;`。
- 引用必须一开始就绑定,不能为空,不能换对象。
- 要让函数修改外面的变量,参数写 `类型&`。
- 大对象只读使用时,参数写 `const 类型&`,避免复制。
- 不要返回局部变量的引用。
