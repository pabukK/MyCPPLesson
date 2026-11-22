# 第 31 课:哈希表

## 这一课你会学到

- 哈希表的原理:怎么做到"几乎一步就找到"
- `std::unordered_map` 和 `std::unordered_set`
- 它和第 18 课的 `map`、`set` 有什么区别,怎么选
- 工程里最常见的几种用法:计数、去重、快速查找
- 哈希的陷阱:冲突、顺序不固定、自定义类型的键

## 1. 问题:能不能比二分还快

第 29 课,在有序数据里二分查找,`O(log n)`。已经很快,但有两个不方便:数据必须先排好序;增删元素要保持有序,代价高。

有没有办法:**不要求有序,查找、插入、删除,平均都只要 `O(1)`?** 有,就是**哈希表**。

## 2. 原理:把"名字"变成"下标"

回忆第 29 课,用下标访问数组 `v[5]`,只要 `O(1)`,因为知道位置就能直接取。

如果能把任意的"键"(比如名字、身份证号)**变成一个下标**,就可以直接去那个位置存取,不用一个个找。

做这件事的函数,叫**哈希函数**:输入一个键,输出一个整数。再用这个整数对"格子总数"取余,就得到下标。

```text
准备 7 个格子,下标 0 到 6。
哈希函数(这里用最简单的:把名字的每个字母编号加起来):
  "Tom"  -> 某个数 -> 取余 7 -> 3   存到第 3 个格子
  "Ann"  -> 某个数 -> 取余 7 -> 5   存到第 5 个格子
查 "Tom":再算一遍哈希,得到 3,直接去第 3 个格子,不用从头找。
```

整个过程,和数据有多少无关。所以平均 `O(1)`。

这些格子,通常叫**桶**。

## 3. 冲突

两个不同的键,算出了同一个下标,怎么办?比如 "Bob" 也算出 3。这叫**冲突**。冲突是不可避免的(键的种类远多于格子数)。

常见的解决办法:**同一个桶里,用链子挂多个**。一个桶里有多个元素时,在桶内部顺序查找。

```text
桶 3:  Tom -> Bob
桶 5:  Ann
```

如果哈希函数设计得好,键分得比较均匀,每个桶里平均只有一两个元素,所以仍然很快。

最坏的情况:所有键都落进了同一个桶,哈希表就退化成一个长链,`O(n)`。设计良好的哈希函数,极少遇到。

当元素越来越多,桶不够用了,哈希表会**自动扩容**:申请更多的桶,把所有元素重新分配。偶尔有一次较慢的重新分配,但平均下来,每次插入仍然是 `O(1)`。

这些细节标准库都替你做了,你不用自己写。

## 4. unordered_map

标准库里的哈希表是 `std::unordered_map`(无序的映射)。用法和第 18 课的 `std::map` 几乎一样,头文件换成 `<unordered_map>`。

```cpp
// file: unordered_basic.cpp
#include <iostream>
#include <string>
#include <unordered_map>

int main() {
    std::unordered_map<std::string, int> age;
    age["Tom"] = 20;
    age["Ann"] = 18;
    age["Bob"] = 25;

    std::cout << "Ann:" << age["Ann"] << std::endl;
    std::cout << "人数:" << age.size() << std::endl;

    auto it = age.find("Zed");
    if (it == age.end()) {
        std::cout << "没有 Zed" << std::endl;
    }

    age.erase("Bob");
    std::cout << "删除 Bob 后:" << age.size() << std::endl;
    std::cout << "有 Tom 吗:" << age.count("Tom") << std::endl;
    return 0;
}
```

运行结果:

```text
Ann:18
人数:3
没有 Zed
删除 Bob 后:2
有 Tom 吗:1
```

- `age["Ann"]` 读、写;`find` 查找;`erase` 删除;`count` 判断有没有。都和 `map` 一样。
- 第 18 课的陷阱,在这里同样存在:用方括号读一个不存在的键,会**悄悄新建**一项。只想查询,请用 `find` 或 `count`。

## 5. map 和 unordered_map 怎么选

| | `std::map` | `std::unordered_map` |
| --- | --- | --- |
| 底层 | 有序的树(第 33 课会学) | 哈希表 |
| 查找、插入、删除 | `O(log n)` | 平均 `O(1)`,最坏 `O(n)` |
| 遍历的顺序 | **按键从小到大** | **没有固定顺序** |
| 能找"比某个键大的第一个" | 能(`lower_bound`) | **不能** |
| 键的要求 | 能比较大小 | 能计算哈希值,能判断相等 |

**怎么选:**

- 只需要"按键查值",不关心顺序:`unordered_map`。
- 需要按键的顺序遍历,或者需要找最大最小、范围查询:`map`。
- 不确定时,数据量不大时,两个都行。

**遍历 `unordered_map`,顺序是不确定的**,而且不同的编译器、不同版本,顺序可能不同。千万不要依赖它的顺序。需要稳定的顺序,可以先把键拿出来排序。

```cpp
// file: unordered_order.cpp
#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

int main() {
    std::unordered_map<std::string, int> score = {{"Tom", 90}, {"Ann", 85}, {"Bob", 70}, {"Eve", 95}};

    std::vector<std::string> names;
    for (const auto& pair : score) {
        names.push_back(pair.first);
    }
    std::sort(names.begin(), names.end());

    for (const auto& name : names) {
        std::cout << name << " " << score[name] << std::endl;
    }
    return 0;
}
```

运行结果:

```text
Ann 85
Bob 70
Eve 95
Tom 90
```

先把键拿出来排序,再按排好的顺序输出,结果就稳定了。

## 6. 用法一:计数

第 18 课用 `map` 数过单词出现的次数。换成 `unordered_map`,同样的写法,速度更快:

```cpp
// file: hash_count.cpp
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

int main() {
    std::vector<std::string> words = {"cat", "dog", "cat", "bird", "dog", "cat"};
    std::unordered_map<std::string, int> count;
    for (const auto& w : words) {
        count[w]++;
    }
    std::cout << "cat:" << count["cat"] << " dog:" << count["dog"] << " bird:" << count["bird"] << std::endl;
    return 0;
}
```

运行结果:

```text
cat:3 dog:2 bird:1
```

这里正好利用了"方括号会悄悄新建,值是 0"的特点。

## 7. 用法二:去重和判断是否见过

`std::unordered_set` 是哈希版本的集合,只存键。第 25 课有个问题:"数组里有没有重复的数?"。当时我们用排序,`O(n log n)`。用哈希集合,可以做到 `O(n)`:边扫描边记录"见过的数",遇到已经见过的,就说明重复。

```cpp
// file: has_duplicate.cpp
#include <iostream>
#include <unordered_set>
#include <vector>

bool has_duplicate(const std::vector<int>& v) {
    std::unordered_set<int> seen;
    for (int x : v) {
        if (seen.count(x) > 0) {
            return true;
        }
        seen.insert(x);
    }
    return false;
}

int main() {
    std::cout << has_duplicate({1, 2, 3, 4}) << std::endl;
    std::cout << has_duplicate({1, 2, 3, 2}) << std::endl;
    return 0;
}
```

运行结果:

```text
0
1
```

代价是多用了 `O(n)` 的内存(第 25 课:时间和空间互换)。

## 8. 用法三:两数之和(无序版)

第 30 课的"两头往中间",要求数组有序。如果数组**没排序**,又不想为排序花 `O(n log n)`,可以用哈希表:

对每个数 `x`,需要找的是 `target - x`。把已经扫描过的数,连同它的下标,存进哈希表。扫到 `x` 时,查哈希表里有没有 `target - x`:

```cpp
// file: two_sum_hash.cpp
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>

std::pair<int, int> two_sum(const std::vector<int>& v, int target) {
    std::unordered_map<int, int> index_of;
    for (int i = 0; i < (int)v.size(); i++) {
        auto it = index_of.find(target - v[i]);
        if (it != index_of.end()) {
            return {it->second, i};
        }
        index_of[v[i]] = i;
    }
    return {-1, -1};
}

int main() {
    std::vector<int> v = {7, 2, 11, 5, 3};
    auto [a, b] = two_sum(v, 9);
    std::cout << a << " " << b << std::endl;
    auto [c, d] = two_sum(v, 100);
    std::cout << c << " " << d << std::endl;
    return 0;
}
```

运行结果:

```text
0 1
-1 -1
```

7 + 2 = 9,下标是 0 和 1。

一遍扫描,每次查找和插入平均 `O(1)`,总共 `O(n)`。注意**先查再存**:如果先存再查,`target` 恰好是 `x` 的两倍时,会把自己当成另一个数。

## 9. 实测:到底快多少

我用随机整数,对 `map` 和 `unordered_map` 各做同样的"计数"操作(单位:毫秒):

| n | `map`(开 `-O2`) | `unordered_map`(开 `-O2`) | 倍数 |
| --- | --- | --- | --- |
| 1 万 | 1.0 | 0.5 | 约 2 倍 |
| 10 万 | 15 | 7.9 | 约 2 倍 |
| 100 万 | 461 | 193 | 约 2.4 倍 |

(无优化编译时,比例相似。你的数字会不同,看相对大小。)

结论要老实说:**哈希表更快,但在这个测试里只快约 2 倍,不是几十倍上百倍。** 原因:

- 大 O 只描述"数据变多时,增长的规律",不是具体快多少。`O(1)` 和 `O(log n)` 在 n 为 100 万时,`log n` 大约是 20,但每一步的代价,两种结构并不一样。
- 哈希表要计算哈希、可能要扩容;树要访问很多不连续的内存。
- 哈希表真正的优势,在数据量更大、键更复杂、或者你只需要"查找"时,会更明显。

所以别神话它。**如果你需要有序、需要范围查询,仍然用 `map`;只要按键查找,`unordered_map` 通常更快。**

## 10. 自定义类型当键:要自己提供哈希

`unordered_map` 的键,标准库已经为 `int`、`string` 等常见类型准备好了哈希函数。如果你用自己定义的结构体当键,编译器不知道怎么算它的哈希,会报错。

比如,用坐标 `(x, y)` 当键:

<!-- expect: compile-fail -->
```cpp
// file: point_key_fail.cpp
#include <unordered_set>

struct Point {
    int x;
    int y;
};

int main() {
    std::unordered_set<Point> visited;
    visited.insert({1, 2});
    return 0;
}
```

编译会失败,错误信息又长又难读,但大意是:找不到 `Point` 的哈希。(第 17 课提过:模板报错信息很长,只看第一条。)

解决办法有两步:

1. 告诉它**怎么判断两个 `Point` 相等**:给 `Point` 写 `operator==`。
2. 告诉它**怎么算哈希值**:写一个"哈希函数对象"。

```cpp
// file: point_key.cpp
#include <iostream>
#include <unordered_set>

struct Point {
    int x;
    int y;

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

struct PointHash {
    std::size_t operator()(const Point& p) const {
        return std::hash<int>()(p.x) * 31 + std::hash<int>()(p.y);
    }
};

int main() {
    std::unordered_set<Point, PointHash> visited;
    visited.insert({1, 2});
    visited.insert({3, 4});
    visited.insert({1, 2});

    std::cout << "不同的点:" << visited.size() << std::endl;
    std::cout << "有 (3,4) 吗:" << visited.count({3, 4}) << std::endl;
    std::cout << "有 (5,6) 吗:" << visited.count({5, 6}) << std::endl;
    return 0;
}
```

运行结果:

```text
不同的点:2
有 (3,4) 吗:1
有 (5,6) 吗:0
```

- `bool operator==(const Point& other) const`:**运算符重载**。让 `p1 == p2` 这样的写法对 `Point` 有意义。这是第 6 课"重载"思想的延伸:连 `==` 这样的符号,也能为你的类型定义。
- `PointHash` 是一个带 `operator()` 的结构体,让它"像函数一样被调用"。这样的东西叫**函数对象**。第 19 课的 lambda 本质上就是编译器帮你生成的函数对象。
- `std::hash<int>()(p.x)`:调用标准库已有的 `int` 哈希,得到哈希值。把 x、y 的哈希值**组合**成一个。乘以 31 再加,是一个常用的组合办法。
- 在 `unordered_set<Point, PointHash>` 的尖括号里,第二个类型就是告诉它用哪个哈希函数对象。

这个组合哈希的写法比较简单,够入门使用。工业级的写法更讲究,让分布更均匀,你需要时再查资料。

**坐标当键,在迷宫、网格类问题里非常常见**(第 35 课会用到)。

## 11. 其他要注意的

**① 遍历时修改。** 在用范围 for 遍历一个哈希表的同时,往里**插入**新元素,可能触发扩容,让正在遍历用的"位置"失效,程序行为不确定(和第 12 课的悬空指针是同一类问题)。要修改,先记下来,遍历完再改。

**② 最坏情况是 `O(n)`。** 如果有人故意构造大量会冲突的键(比如在网络服务里,键来自用户输入),哈希表会退化。这一点在对外服务的程序里要留意。遇到这种场景,用 `map`(最坏也是 `O(log n)`)更可靠。

**③ 浮点数不适合当键。** `0.1 + 0.2` 和 `0.3` 在电脑里并不严格相等,用 `double` 当键会出现莫名其妙的"找不到"。要用,先转成整数(比如乘以 100 取整)。

**④ 预留空间。** 如果提前知道大概要放多少个,调用 `u.reserve(n)`,可以避免中途多次扩容,更快。

**⑤ `count` 和 `find`。** 想知道"有没有":`count(key) > 0`。需要同时拿到值:`find`,避免查两次。

## 练习

1. 用 `unordered_map` 统计一个字符串里每个字符出现的次数,输出出现次数最多的字符。
2. 写函数,判断两个字符串是不是"字母异位词"(包含的字母和个数完全一样,顺序可以不同,比如 "listen" 和 "silent")。提示:两个计数表比较,或者各自排序后比较。
3. 给一个数组,找出**出现次数超过一半**的数。先用哈希表做,再想想有没有不需要额外空间的办法。
4. 把第 25 课的 `has_duplicate_slow`、`has_duplicate_fast`,和本课的哈希版本三个放在一起,测 10 万个数据的速度。
5. 用 `Point` 当键,实现一个"访问过的格子"记录:在 10×10 的格子里,从 (0,0) 开始随机走 50 步,统计走过了多少个不同的格子。
6. **进阶**:第 30 课练习 4:给一个数组,求和等于 k 的连续子数组个数。提示:前缀和加哈希表,统计"前缀和 - k"出现过几次。

## 小结

- 哈希表:用哈希函数把键变成下标,平均 `O(1)` 地查找、插入、删除。
- 冲突:不同的键落进同一个桶,用链子挂在一起。
- `std::unordered_map`(键值对)、`std::unordered_set`(只有键)。
- 和 `map` 相比:平均更快,但没有顺序,不能做范围查询,最坏 `O(n)`。
- 实测:大约快 2 倍,别神话。大 O 不等于具体倍数。
- 三大用法:**计数、去重、快速查找**。
- 自定义类型当键,要提供 `operator==` 和哈希函数。
- 不要依赖遍历顺序;浮点数不适合当键;遍历时不要往里插入。
