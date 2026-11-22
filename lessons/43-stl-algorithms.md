# 第 43 课:标准库算法进阶

## 这一课你会学到

- 标准库算法的统一套路:"起点、终点"
- 部分排序、取第 k 小、划分:比全排序更省的做法
- 堆操作、二分、累积:常用但容易被忽略的一组
- 迭代器:为什么算法都长一个样
- 每个算法的复杂度,以及哪些地方不该用标准库

## 1. 统一套路

第 19 课开始,标准库的算法都是这个形状:

```cpp
std::sort(v.begin(), v.end());
std::find(v.begin(), v.end(), 值);
std::lower_bound(v.begin(), v.end(), 值);
```

**用"起点、终点"表示范围**,而不是传容器本身。这个设计带来三个好处:

1. 同一个算法能用于 `vector`、`deque`、`list`、数组、甚至字符串。
2. 可以只处理一部分:`std::sort(v.begin(), v.begin() + 10)` 只排前 10 个。
3. 不用为每种容器重写一套算法。

`v.end()` 是**最后一个元素的后面**(第 28 课提过),又是左闭右开。这门课反复出现这个约定,因为它让"范围长度 = 终点 - 起点",空范围也不需要特殊处理。

## 2. 不需要的全排序

**场景:** 只要最大的 10 个,或者只要"中位数",或者想把小于某个值的放前面。全排序都做了多余的工作。

```cpp
// file: partial_ops.cpp
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

int main() {
    std::vector<int> v = {9, 3, 7, 1, 8, 2, 6, 4, 5};

    std::vector<int> a = v;
    std::partial_sort(a.begin(), a.begin() + 3, a.end());
    std::cout << "前 3 小:";
    for (int i = 0; i < 3; i++) std::cout << a[i] << " ";
    std::cout << std::endl;

    std::vector<int> b = v;
    std::nth_element(b.begin(), b.begin() + b.size() / 2, b.end());
    std::cout << "中位数:" << b[b.size() / 2] << std::endl;

    std::vector<int> c = v;
    std::partition(c.begin(), c.end(), [](int x) { return x < 5; });
    auto mid = std::find_if(c.begin(), c.end(), [](int x) { return x >= 5; });
    std::cout << "小于 5 的个数:" << (mid - c.begin()) << std::endl;

    std::vector<int> d = v;
    std::sort(d.begin(), d.end());
    std::cout << "全排序:";
    for (int x : d) std::cout << x << " ";
    std::cout << std::endl;
    return 0;
}
```

运行结果:

```text
前 3 小:1 2 3 
中位数:5
小于 5 的个数:4
全排序:1 2 3 4 5 6 7 8 9 
```

- `std::partial_sort(起点, 分界, 终点)`:保证 `[起点, 分界)` 是排好序的最小那几个。**分界之后的元素是乱的**,不要指望。
- `std::nth_element(起点, 目标位置, 终点)`:把"排序后本该在目标位置的那个元素"放到那儿,左边都比它小,右边都比它大,**但两边内部无序**。求中位数、取第 k 大,用它,平均 `O(n)`。
- `std::partition(起点, 终点, 条件)`:满足条件的挪到前面,不满足的挪到后面,两边内部无序。第 28 课快速排序的"分两堆",标准库里就是这个函数。
- 这三个都比 `std::sort` 便宜:`partial_sort` 是 `O(n log k)`,`nth_element` 和 `partition` 是 `O(n)`。

## 3. 迭代器:算法为什么能通用

`v.begin()` 返回的东西叫**迭代器**(iterator)。可以理解成"一个能指向容器里某个元素的位置",第 18、19 课已经这么用了。

它比普通指针多一层:不同容器的"移动"含义不同。

| 操作 | 含义 |
| --- | --- |
| `*it` | 取出这个位置的元素 |
| `++it` | 移到下一个位置 |
| `it1 == it2` | 是否指向同一位置 |
| `it2 - it1` | 相距多少个元素(**只有部分容器支持**) |

关键区别:

- `vector`、`deque`、数组的迭代器是**随机访问**的:能 `it + 5` 一下跳 5 格,也能相减求距离。
- `list`、`set`、`map` 的迭代器只能 `++it` **一格一格走**。所以 `std::sort` 不能用于 `list`(它要求随机访问);`list` 要用自己的成员函数 `l.sort()`。
- 第 32 课的 `std::list` 例子中,我不能写 `it + 2`,只能连写两个 `++it`,就是这个原因。

**迭代器失效**(第 12 课、第 42 课提过):对 `vector` 做 `push_back`、`insert`、`erase` 之后,之前拿到的迭代器**全部作废**,继续用是未定义行为。`list` 只作废被删那个节点的迭代器。这是标准库最常见的一类运行时错误,而且往往不报错,只是悄悄出错。

## 4. 常用算法一览

| 算法 | 作用 | 复杂度 | 要求 |
| --- | --- | --- | --- |
| `sort` | 全排序 | `O(n log n)` | 随机访问 |
| `stable_sort` | 稳定排序 | `O(n log n)` | 随机访问 |
| `partial_sort` | 只排前 k 个 | `O(n log k)` | 随机访问 |
| `nth_element` | 把第 k 小放到位置 k | 平均 `O(n)` | 随机访问 |
| `partition` | 按条件分成两堆 | `O(n)` | 随机访问 |
| `binary_search` | 有没有 | `O(log n)` | **已排序** |
| `lower_bound` / `upper_bound` | 第一个 ≥ / > 的位置 | `O(log n)` | **已排序** |
| `equal_range` | 上面两个一起返回 | `O(log n)` | **已排序** |
| `find` / `find_if` | 找第一个 | `O(n)` | — |
| `count` / `count_if` | 统计个数 | `O(n)` | — |
| `any_of` / `all_of` / `none_of` | 是否存在 / 全部满足 / 全不满足 | `O(n)` | — |
| `max_element` / `min_element` | 最大 / 最小的位置 | `O(n)` | — |
| `accumulate` | 求和(或任意累积) | `O(n)` | — |
| `reverse` | 反转 | `O(n)` | — |
| `rotate` | 循环移位 | `O(n)` | — |
| `unique` | 去掉**相邻**重复 | `O(n)` | 通常先排序 |
| `inplace_merge` | 合并两段已排序的 | `O(n)` | 两段各自有序 |
| `make_heap` / `push_heap` / `pop_heap` | 堆操作 | `O(n)` / `O(log n)` | — |
| `includes` | a 是否包含 b 的全部 | `O(n+m)` | 两者已排序 |
| `set_union` / `set_intersection` / `set_difference` | 集合运算 | `O(n+m)` | **两者已排序** |
| `lexicographical_compare` | 按字典序比较两段 | `O(n)` | — |
| `next_permutation` | 下一个排列 | `O(n)` | 通常先排序 |

## 5. 几个容易用错的

**① `unique` 不删除,只是"往前挤"。**

```cpp
// file: unique_demo.cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {1, 2, 2, 3, 3, 3, 4};
    auto last = std::unique(v.begin(), v.end());
    std::cout << "unique 之后个数:" << v.size() << std::endl;
    for (int x : v) std::cout << x << " ";
    std::cout << std::endl;

    v.erase(last, v.end());
    std::cout << "erase 之后个数:" << v.size() << std::endl;
    for (int x : v) std::cout << x << " ";
    std::cout << std::endl;

    std::vector<int> u = {3, 1, 3, 2, 1};
    u.erase(std::unique(u.begin(), u.end()), u.end());
    std::cout << "没排序就去重:";
    for (int x : u) std::cout << x << " ";
    std::cout << std::endl;
    return 0;
}
```

运行结果:

```text
unique 之后个数:7
1 2 3 4 3 3 4 
erase 之后个数:4
1 2 3 4 
没排序就去重:3 1 3 2 1 
```

- `unique` 返回"去重后范围的终点",**元素个数没变**,尾部留着垃圾值。要真正删掉,必须再 `v.erase(last, v.end())`。这是标准库里最经典的误用。
- 第三行说明:`unique` 只去掉**相邻**的重复。`{3,1,3,2,1}` 里有两个 3、两个 1,但不相邻,所以一个都没去掉。**用之前必须先排序**(第 18 课的 `map`/`set` 才是自动去重)。

**② `accumulate` 的第三个参数决定累加的类型。**

```cpp
// file: accumulate_demo.cpp
#include <iostream>
#include <numeric>
#include <vector>

int main() {
    std::vector<int> big = {1000000000, 1000000000, 1000000000};
    std::cout << "用 0 起步:" << std::accumulate(big.begin(), big.end(), 0) << std::endl;
    std::cout << "用 0LL 起步:" << std::accumulate(big.begin(), big.end(), 0LL) << std::endl;

    std::vector<int> small = {2, 3, 4, 5};
    std::cout << "乘法(0LL 起步):" << std::accumulate(small.begin(), small.end(), 0LL,
                                                       [](long long a, int b) { return a + b; }) << std::endl;
    std::cout << "乘法(1 起步):" << std::accumulate(small.begin(), small.end(), 1LL,
                                                     [](long long a, int b) { return a * b; }) << std::endl;

    std::cout << "三个 10 亿相乘:" << std::accumulate(big.begin(), big.end(), 1LL,
                                                      [](long long a, int b) { return a * b; }) << std::endl;
    return 0;
}
```

运行结果:

```text
用 0 起步:-1294967296
用 0LL 起步:3000000000
乘法(0LL 起步):14
乘法(1 起步):120
三个 10 亿相乘:-6930898827444486144
```

- 第一行**溢出**了:`0` 是 `int`,所以累加按 `int` 做,三个 10 亿加起来超过 `int` 上限,变成了负数(第 2 课、第 38 课)。
- 第二行写 `0LL`,累加就按 `long long` 做,得到正确答案 30 亿。
- 第三、四行说明:**初始值还是累加的起点**。求和从 0 开始,求积从 1 开始(乘 0 就全变 0 了)。
- 最后一行是我特意试的:`10⁹ × 10⁹ × 10⁹ = 10²⁷`,而 `long long` 的上限只有约 `9.2 × 10¹⁸`,所以**溢出成了负数**。注意它没有报错,悄悄给了一个看起来"像数字"的错误结果。

**`accumulate` 的两条规矩:**

1. 初始值的类型,决定整个累加的类型。求和大数写 `0LL`,不要写 `0`。
2. 估算结果的最大值,确认装得下。`long long` 也就 19 位,连乘三四个大数就超了(第 38 课的斐波那契也一样)。装不下就要用第 44 课提到的大整数库,或者在题目要求下取模。

**③ 二分和集合运算要求已排序,但不会检查。** `binary_search`、`lower_bound`、`set_intersection` 对未排序的数据**不报错,只是结果错**。这是最难发现的误用之一。

**④ 谓词必须严格。** 传给 `sort` 的比较函数,必须满足"严格弱序":`cmp(a, a)` 必须为假。如果写成 `<=`,排序可能崩溃或死循环。第 28 课归并排序里用 `<=` 是安全的,但那是我们自己写的合并,不是给 `sort` 的比较函数。

```cpp
// 对
std::sort(v.begin(), v.end(), [](int a, int b) { return a < b; });
// 错,可能崩溃
std::sort(v.begin(), v.end(), [](int a, int b) { return a <= b; });
```

## 6. 什么时候不该用标准库

| 情况 | 原因 |
| --- | --- |
| `std::sort` 用于 `list` | 编译不过,它要求随机访问。用 `l.sort()` |
| 在 `list`、`set`、`map` 上用 `std::distance`、`std::find` 的跨度计算 | 一格一格走,`O(n)`,不是你以为的 `O(log n)` |
| `std::unordered_map` 需要顺序 | 它没有顺序,用 `map`(第 31 课) |
| 大数据量下用 `vector` 频繁在中间插入 | `O(n)` 挪动,考虑 `deque` 或换思路(第 18、32 课) |
| 用 `std::string` 存超大文本再反复 `substr` | 每次都复制一份,`O(n)`。C++17 的 `std::string_view` 只存"指针+长度",不复制 |
| 递归版算法处理大规模数据 | 栈溢出(第 26、32、35 课) |

`std::string_view` 值得单独说一句:它是"**我不拥有这段字符串,只是引用它的一段**"。传参、切片时避免复制,性能差别很大。但它和引用一样,**不能比原字符串活得更久**(第 10 课"别返回局部变量的引用"、第 12 课悬空指针),否则就是悬空。

## 7. 复杂度速记

判断该用哪个算法,先问三个问题:

1. **数据有序吗?** 有序 → 二分(`O(log n)`)。无序但反复查 → 先排序,或改用 `map`/`unordered_map`。
2. **要全部有序,还是只要极值/前 k 个?** 全部 → `sort`;前 k 个 → `partial_sort` 或堆(第 34 课);只要第 k 小 → `nth_element`。
3. **要查找还是只要判断存在?** 查找位置 → `lower_bound`;判断存在 → `binary_search` 或哈希表。

## 练习

1. 用 `nth_element` 求一组数的第 k 大,和第 34 课的"小顶堆取前 k 大"对拍,并比较两者在 100 万个数上的耗时。
2. 用 `std::partition` 重写第 28 课的快速排序,体会标准库版本和自己版本的差别。
3. 给两个已排序的数组,用 `set_intersection` 求交集,和用双指针(第 30 课)手写版本比较。
4. 用 `next_permutation` 生成 `{1,2,3}` 的全部排列,和第 39 课的回溯结果对比。
5. 写一段代码,故意在 `vector` 上 `push_back` 之后继续使用之前保存的迭代器,用 AddressSanitizer(第 12 课)看它报不报。
6. 把第 24 课的成绩管理程序里,手工写的排序和查找,换成 `std::sort` 和 `std::lower_bound`,比较代码行数。

## 小结

- 标准库算法统一用"起点、终点"(左闭右开)表示范围,所以能通用于各种容器和部分范围。
- 不需要全排序时:`partial_sort`(前 k 个有序)、`nth_element`(只要第 k 小)、`partition`(分两堆),都比 `sort` 省。
- 迭代器分两类:`vector`、`deque` 能随机跳跃;`list`、`set`、`map` 只能一格一格走。
- **迭代器失效**:vector 增删后旧迭代器全部作废;list 只作废被删的那个。
- `unique` 不删除元素,只往前挤,必须配合 `erase` 用,而且只对**相邻**重复有效。
- `accumulate` 的初始值类型决定累加类型,求和大数要写 `0LL`。
- 二分和集合运算要求已排序,**不检查、不报错,只是结果错**。
- 给 `sort` 的比较函数必须严格(`<` 而不是 `<=`)。
- 大量切片传参用 `std::string_view` 避免复制,但不能比原串活得久。
