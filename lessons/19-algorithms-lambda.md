# 第 19 课:算法与 lambda

## 这一课你会学到

- 用标准库的算法完成排序、查找、统计
- lambda:随手写一个没有名字的小函数
- 用 lambda 自定义排序规则

## 1. 为什么用现成的算法

排序、查找最大值、统计个数……这些事你可以自己用循环写,但标准库已经提供了写好的、测试过的版本。用现成的:代码更短,更不容易出错,别人一看就懂。

用算法要加 `#include <algorithm>`。

## 2. 排序和查找

```cpp
// file: sort_demo.cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {5, 2, 9, 1, 7};

    std::sort(v.begin(), v.end());
    for (int n : v) {
        std::cout << n << " ";
    }
    std::cout << std::endl;

    auto it = std::find(v.begin(), v.end(), 9);
    if (it != v.end()) {
        std::cout << "找到了 9,下标是 " << (it - v.begin()) << std::endl;
    }

    std::cout << "最大:" << *std::max_element(v.begin(), v.end()) << std::endl;
    std::cout << "大于 1 的个数:" << std::count_if(v.begin(), v.end(), [](int x) { return x > 1; }) << std::endl;
    return 0;
}
```

运行结果:

```text
1 2 5 7 9 
找到了 9,下标是 4
最大:9
大于 1 的个数:4
```

逐个看:

- `v.begin()` 和 `v.end()` 表示"从头到尾"这个范围。(`end()` 指向最后一个元素的**后面**,不是最后一个。)算法都用这两个位置来表示要处理的范围。
- `std::sort(起点, 终点)`:从小到大排序。
- `std::find(起点, 终点, 值)`:找这个值,找到返回它的位置,没找到返回 `end()`。第 18 课 `map` 的 `find` 是同样的写法。
- `it - v.begin()` 得到的是下标。
- `std::max_element` 返回最大元素的位置,前面加 `*` 取出值(和第 9 课解引用的写法一样)。
- `std::count_if` 统计满足条件的元素个数。条件写在最后一个参数,那个奇怪的 `[](int x) { return x > 1; }` 就是 lambda,下一节解释。

## 3. lambda:没有名字的小函数

`count_if` 需要你告诉它"什么算满足条件"。条件是一小段逻辑,专门为这里写一个有名字的函数,太麻烦。**lambda** 让你在需要的地方,直接写一个临时的小函数。

lambda 的格式:

```text
[捕获](参数) { 函数体 }
```

先看最简单的情况,把它存到变量里再调用:

```cpp
// file: lambda_basic.cpp
#include <iostream>

int main() {
    auto add = [](int a, int b) {
        return a + b;
    };

    std::cout << add(3, 4) << std::endl;
    return 0;
}
```

运行结果:

```text
7
```

- 最前面的 `[]` 是 lambda 的标志。
- `(int a, int b)` 和普通函数的参数一样。
- 大括号里是函数体,和普通函数一样。
- 返回类型编译器自己推断。
- 用 `auto` 保存它,因为 lambda 的类型很复杂,没法手写。

## 4. 捕获:使用外面的变量

lambda 默认看不到它外面的变量。要用,就写在最前面的方括号里,这叫**捕获**。

```cpp
// file: lambda_capture.cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {3, 8, 12, 5, 20};
    int limit = 10;

    int n = std::count_if(v.begin(), v.end(), [limit](int x) {
        return x > limit;
    });

    std::cout << "大于 " << limit << " 的有 " << n << " 个" << std::endl;
    return 0;
}
```

运行结果:

```text
大于 10 的有 2 个
```

`[limit]` 表示把 `limit` 复制一份给 lambda 用。如果想直接用外面变量本身(比如要修改它),写 `[&limit]`,这里的 `&` 和第 10 课引用的含义一样。

## 5. 自定义排序规则

`std::sort` 默认从小到大。想要别的顺序,给它第三个参数:一个 lambda,告诉它"`a` 是否应该排在 `b` 前面"。

```cpp
// file: sort_custom.cpp
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

struct Student {
    std::string name;
    int score;
};

int main() {
    std::vector<Student> students = {
        {"小明", 85}, {"小红", 92}, {"小刚", 78}, {"小美", 92}
    };

    std::sort(students.begin(), students.end(),
              [](const Student& a, const Student& b) {
                  if (a.score != b.score) {
                      return a.score > b.score;
                  }
                  return a.name < b.name;
              });

    for (const auto& s : students) {
        std::cout << s.name << " " << s.score << std::endl;
    }
    return 0;
}
```

运行结果:

```text
小红 92
小美 92
小明 85
小刚 78
```

规则写在 lambda 里:先按分数从高到低(`a.score > b.score` 表示分数高的排前面);分数相同,再按名字排序。这里的参数用 `const Student&`,因为第 10 课说过,大对象用只读引用,避免复制。

## 6. 几个常用算法

| 算法 | 作用 |
| --- | --- |
| `std::sort` | 排序 |
| `std::find` | 查找某个值 |
| `std::find_if` | 查找第一个满足条件的元素 |
| `std::count` / `std::count_if` | 统计个数 |
| `std::max_element` / `std::min_element` | 找最大 / 最小 |
| `std::reverse` | 反转顺序 |
| `std::accumulate` | 求和(需要 `#include <numeric>`) |
| `std::any_of` / `std::all_of` | 是否存在 / 是否全部满足 |

```cpp
// file: algo_more.cpp
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};

    std::cout << "和:" << std::accumulate(v.begin(), v.end(), 0) << std::endl;

    std::reverse(v.begin(), v.end());
    for (int n : v) {
        std::cout << n << " ";
    }
    std::cout << std::endl;

    bool has_even = std::any_of(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
    std::cout << "有偶数吗:" << has_even << std::endl;
    return 0;
}
```

运行结果:

```text
和:15
5 4 3 2 1 
有偶数吗:1
```

`std::accumulate` 的第三个参数是起始值,0 表示从 0 开始累加。

## 练习

1. 给一个 `vector<int>`,用 `std::sort` 加 lambda 让它从大到小排列。
2. 给一个 `vector<string>`,按字符串长度从短到长排序。
3. 用 `std::find_if` 找到第一个大于 50 的数,并输出它的位置。
4. 用 `count_if` 统计一个 `vector<Student>` 里有几人及格(60 分以上)。

## 小结

- 算法用"起点、终点"表示范围:`v.begin()`、`v.end()`。
- `sort`、`find`、`count_if`、`max_element`、`accumulate` 等是常用算法,优先用它们,少手写循环。
- lambda:`[捕获](参数) { 函数体 }`,在需要的地方直接写小函数。
- `[x]` 复制一份外面的变量 `x`,`[&x]` 直接用 `x` 本身。
- 自定义排序:给 `sort` 一个返回"a 是否排在 b 前面"的 lambda。
