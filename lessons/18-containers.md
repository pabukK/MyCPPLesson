# 第 18 课:容器:vector、map、set

## 这一课你会学到

- `vector` 的更多用法:插入、删除、查找
- `map`:用"名字"查"值"
- `set`:自动去重的集合
- 怎么在三者之间选择

## 1. 什么是容器

能装很多个元素的类型,统称为**容器**。第 7 课的 `vector` 就是容器。标准库里还有很多种,各有擅长的事情。本课学最常用的三种。

它们都是类模板(第 17 课),都在销毁时自动归还内存,所以你不用写 `new` 和 `delete`。

## 2. vector 的更多用法

```cpp
// file: vector_more.cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {10, 20, 30, 40};

    v.insert(v.begin() + 1, 15);
    v.erase(v.begin() + 3);

    for (int n : v) {
        std::cout << n << " ";
    }
    std::cout << std::endl;

    std::cout << "第一个:" << v.front() << std::endl;
    std::cout << "最后一个:" << v.back() << std::endl;
    std::cout << "是否为空:" << v.empty() << std::endl;

    v.clear();
    std::cout << "清空后个数:" << v.size() << std::endl;
    return 0;
}
```

运行结果:

```text
10 15 20 40 
第一个:10
最后一个:40
是否为空:0
清空后个数:0
```

- `v.begin()` 指向第一个元素的位置,`v.begin() + 1` 是第二个的位置。
- `insert(位置, 值)`:在那个位置前面插入。
- `erase(位置)`:删除那个位置的元素。
- 在中间插入或删除,后面的元素都要往后或往前挪,元素多时比较慢。只在末尾增删(`push_back`、`pop_back`)是很快的。

## 3. map:用键查值

假设要记录每个学生的分数,想"输入名字,查出分数"。用 vector 要从头一个个找。`map` 专门干这个:每个元素是一对,左边叫**键**,右边叫**值**,通过键能直接找到值。

```cpp
// file: map_demo.cpp
#include <iostream>
#include <map>
#include <string>

int main() {
    std::map<std::string, int> scores;

    scores["小明"] = 90;
    scores["小红"] = 85;
    scores["小刚"] = 70;

    std::cout << "小红:" << scores["小红"] << std::endl;

    scores["小红"] = 95;
    std::cout << "改后:" << scores["小红"] << std::endl;

    std::cout << "人数:" << scores.size() << std::endl;
    return 0;
}
```

运行结果:

```text
小红:85
改后:95
人数:3
```

- `std::map<std::string, int>`:键是 `string`,值是 `int`。尖括号里写两个类型。
- 用方括号放键,就能读或写对应的值。
- 键不会重复。给已有的键赋值,是修改它的值。

## 4. map 的陷阱:方括号会悄悄添加

如果用方括号查一个**不存在**的键,`map` 不会报错,而是**悄悄新建**这一项,值是 0(或者空)。

```cpp
// file: map_trap.cpp
#include <iostream>
#include <map>
#include <string>

int main() {
    std::map<std::string, int> scores;
    scores["小明"] = 90;

    std::cout << scores["小李"] << std::endl;
    std::cout << "人数:" << scores.size() << std::endl;
    return 0;
}
```

运行结果:

```text
0
人数:2
```

只是想查一下,结果多出了一个"小李"。只想查、不想添加时,先用 `count` 判断键是否存在,或者用 `find`:

```cpp
// file: map_find.cpp
#include <iostream>
#include <map>
#include <string>

int main() {
    std::map<std::string, int> scores;
    scores["小明"] = 90;

    auto it = scores.find("小李");
    if (it == scores.end()) {
        std::cout << "没有小李" << std::endl;
    } else {
        std::cout << "分数:" << it->second << std::endl;
    }
    std::cout << "人数:" << scores.size() << std::endl;
    return 0;
}
```

运行结果:

```text
没有小李
人数:1
```

`find` 返回一个"位置"。如果等于 `scores.end()`,表示没找到。找到了,`it->first` 是键,`it->second` 是值。(`->` 在第 16 课见过,用于指针。这里的"位置"用法类似指针。)

## 5. 遍历 map

范围 for 也能用在 map 上。每个元素是一对,有 `first`(键)和 `second`(值)。`map` 会自动按键的大小顺序排好。

```cpp
// file: map_loop.cpp
#include <iostream>
#include <map>
#include <string>

int main() {
    std::map<std::string, int> count;
    count["banana"] = 2;
    count["apple"] = 5;
    count["cherry"] = 1;

    for (const auto& pair : count) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    return 0;
}
```

运行结果:

```text
apple: 5
banana: 2
cherry: 1
```

输出按字母顺序排列,和放进去的顺序无关。

一个常见用途是数数:统计每个词出现了几次。

```cpp
// file: word_count.cpp
#include <iostream>
#include <map>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> words = {"cat", "dog", "cat", "bird", "dog", "cat"};
    std::map<std::string, int> count;

    for (const auto& w : words) {
        count[w]++;
    }

    for (const auto& pair : count) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    return 0;
}
```

运行结果:

```text
bird: 1
cat: 3
dog: 2
```

这里正好利用了第 4 节的"悄悄添加":第一次见到某个词,`count[w]` 自动新建并且值是 0,再 `++` 变成 1。

## 6. set:不重复的集合

`set` 只存键,不存值。它会自动去掉重复的元素,并且按大小排序。

```cpp
// file: set_demo.cpp
#include <iostream>
#include <set>

int main() {
    std::set<int> s;
    s.insert(5);
    s.insert(2);
    s.insert(8);
    s.insert(2);
    s.insert(5);

    std::cout << "个数:" << s.size() << std::endl;
    for (int n : s) {
        std::cout << n << " ";
    }
    std::cout << std::endl;

    std::cout << "有 8 吗:" << s.count(8) << std::endl;
    std::cout << "有 9 吗:" << s.count(9) << std::endl;
    return 0;
}
```

运行结果:

```text
个数:3
2 5 8 
有 8 吗:1
有 9 吗:0
```

放进去五次,重复的被忽略,只剩 3 个。`count(x)` 返回 1 表示有,0 表示没有。

## 7. 怎么选

| 你的需求 | 选择 |
| --- | --- |
| 一排数据,按位置访问,主要在末尾增加 | `vector`(默认选它) |
| 通过名字(或编号)查找对应的值 | `map` |
| 只关心有没有,或者需要去重 | `set` |

**不确定时,先用 `vector`。** 它最简单,在大多数情况下也足够快。

## 练习

1. 输入若干个整数,放进 `set`,输出去重后的结果。
2. 输入一句话(用空格隔开的单词),用 `map` 统计每个单词出现的次数。
3. 做一个简易电话本:用 `map<string, string>` 存名字和号码,让用户输入名字查询,查不到时提示"没有这个人",且不要悄悄添加。

## 小结

- `vector`:一排数据,末尾增删快,中间增删慢。
- `map`:键和值成对,通过键查值,键不重复,按键排序。
- 用方括号读 `map` 里不存在的键,会悄悄新增。只查不加,用 `find`。
- `set`:自动去重、自动排序。
- 这些容器都自动管理内存,不用手写 `new` 和 `delete`。
