# 第 30 课:双指针、滑动窗口、前缀和

## 这一课你会学到

- 双指针:用两个"位置"配合,一遍扫描完成任务
- 滑动窗口:在一段连续的范围上,边滑动边统计
- 前缀和:预先算好累加,以后每次查区间和只要 `O(1)`
- 这三招都是把"两层循环 `O(n²)`"改成"一层 `O(n)`"的常用手段

## 先说明一个词

这一课里的"指针",**不是**第 9 课的指针。这里指的是**下标**:用两个整数变量,分别记录数组里的两个位置。它们像两只手指,在数组上移动,所以习惯上叫"双指针"。别搞混。

## 1. 双指针(一):两头往中间

**问题:** 一个**已经排好序**的数组,找两个数,它们的和等于 `target`。返回这两个数的下标。

最直接的办法:两层循环,试遍所有的两两组合,`O(n²)`。

有序的性质,可以让我们做得更好。一个手指放在最左边(最小的数),一个放在最右边(最大的数):

- 两数之和**太大**:右边的数换小一点,右手指往左移。
- 两数之和**太小**:左边的数换大一点,左手指往右移。
- **刚好**:找到了。

```text
{1, 3, 4, 6, 8, 11},target = 10
左=1(下标0)  右=11(下标5)  和 12 太大 -> 右手指左移
左=1(下标0)  右=8(下标4)   和  9 太小 -> 左手指右移
左=3(下标1)  右=8(下标4)   和 11 太大 -> 右手指左移
左=3(下标1)  右=6(下标3)   和  9 太小 -> 左手指右移
左=4(下标2)  右=6(下标3)   和 10 找到!
```

每一步,必有一个手指移动,总共最多移动 n 步。`O(n)`。

```cpp
// file: two_sum_sorted.cpp
#include <iostream>
#include <utility>
#include <vector>

std::pair<int, int> two_sum_sorted(const std::vector<int>& v, int target) {
    int left = 0;
    int right = (int)v.size() - 1;
    while (left < right) {
        int sum = v[left] + v[right];
        if (sum == target) {
            return {left, right};
        } else if (sum < target) {
            left++;
        } else {
            right--;
        }
    }
    return {-1, -1};
}

int main() {
    std::vector<int> v = {1, 3, 4, 6, 8, 11};
    auto [a, b] = two_sum_sorted(v, 10);
    std::cout << a << " " << b << std::endl;
    auto [c, d] = two_sum_sorted(v, 100);
    std::cout << c << " " << d << std::endl;
    return 0;
}
```

运行结果:

```text
2 3
-1 -1
```

- `std::pair<int, int>` 是"装两个值的小盒子",用它一次返回两个整数。`return {left, right};` 是简便写法。
- `auto [a, b] = ...` 叫**结构化绑定**,把返回的一对值,分别拿给 `a` 和 `b` 两个变量。这是 C++17 的写法。
- 这里 `right` 用 `(int)v.size() - 1`:先把 `size()` 转成 `int` 再减,如果 `v` 是空的,就是 -1,循环条件 `left < right` 直接不成立,不会出错。如果不转换,`v.size()` 是无符号数,空数组时 `size() - 1` 会变成一个巨大的数,出大问题。**这是一个经典的坑:无符号数减法。**

## 2. 双指针(二):快慢指针去重

**问题:** 有序数组里有重复的数,原地去掉重复,返回去重后的长度。比如 `{1, 1, 2, 2, 2, 3}` 变成 `{1, 2, 3, ...}`,长度 3。

想法:一个手指 `slow` 指着"已经整理好的部分的末尾",另一个手指 `fast` 往前探路,遇到和 `slow` 不同的新数,就放到 `slow` 后面。

```cpp
// file: remove_duplicates.cpp
#include <iostream>
#include <vector>

int remove_duplicates(std::vector<int>& v) {
    if (v.empty()) {
        return 0;
    }
    int slow = 0;
    for (int fast = 1; fast < (int)v.size(); fast++) {
        if (v[fast] != v[slow]) {
            slow++;
            v[slow] = v[fast];
        }
    }
    return slow + 1;
}

int main() {
    std::vector<int> v = {1, 1, 2, 2, 2, 3, 5, 5};
    int len = remove_duplicates(v);
    for (int i = 0; i < len; i++) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "长度:" << len << std::endl;
    return 0;
}
```

运行结果:

```text
1 2 3 5 
长度:4
```

一遍扫描,`O(n)`,不需要额外空间。注意要先处理空数组,不然 `v[0]` 越界(第 7 课)。

## 3. 滑动窗口

**问题:** 给一个整数数组,找出**连续的 k 个数**,使它们的和最大。

直接做:对每个起点,把后面 k 个数加一遍,`O(n × k)`。

观察:从位置 i 的窗口,滑到 i+1 的窗口,**只是去掉了最左边的一个数,加上了新进来的右边一个数**,中间的 k-1 个数完全没变,没必要重新加。

```text
{2, 1, 5, 1, 3, 2},k = 3
窗口 [2 1 5] 和 8
  去掉 2,加上 1 -> [1 5 1] 和 7
  去掉 1,加上 3 -> [5 1 3] 和 9
  去掉 5,加上 2 -> [1 3 2] 和 6
最大是 9
```

```cpp
// file: max_window_sum.cpp
#include <algorithm>
#include <iostream>
#include <vector>

int max_window_sum(const std::vector<int>& v, int k) {
    if (k <= 0 || k > (int)v.size()) {
        return 0;
    }
    int window = 0;
    for (int i = 0; i < k; i++) {
        window += v[i];
    }
    int best = window;
    for (int i = k; i < (int)v.size(); i++) {
        window += v[i] - v[i - k];
        best = std::max(best, window);
    }
    return best;
}

int main() {
    std::vector<int> v = {2, 1, 5, 1, 3, 2};
    std::cout << max_window_sum(v, 3) << std::endl;
    std::cout << max_window_sum(v, 1) << std::endl;
    std::cout << max_window_sum(v, 6) << std::endl;
    return 0;
}
```

运行结果:

```text
9
5
14
```

- 先算出第一个窗口的和。
- 然后每次滑动一格:加上新进来的 `v[i]`,减去刚离开的 `v[i - k]`。
- 总共 `O(n)`。

## 4. 滑动窗口:窗口长度可变

**问题:** 给一个正整数数组和一个目标值 `target`,找**和不小于 target 的最短连续子数组**的长度。没有就返回 0。

做法:右手指不断向右扩张,把数加进窗口。只要窗口的和够了,就尝试把左手指向右收缩,看能不能缩得更短而仍然够用。

```text
{2, 3, 1, 2, 4, 3},target = 7
窗口 [2 3 1 2] 和 8 >= 7 够了,长度 4 -> 收缩:去掉 2 -> [3 1 2] 和 6 不够
继续右扩:[3 1 2 4] 和 10 够,长度 4 -> 收缩:[1 2 4] 和 7 够,长度 3 -> 再缩:[2 4] 和 6 不够
继续右扩:[2 4 3] 和 9 够,长度 3 -> 收缩:[4 3] 和 7 够,长度 2 -> 再缩:[3] 不够
答案 2
```

```cpp
// file: min_subarray.cpp
#include <algorithm>
#include <iostream>
#include <vector>

int min_subarray_len(const std::vector<int>& v, int target) {
    int left = 0;
    int sum = 0;
    int best = (int)v.size() + 1;
    for (int right = 0; right < (int)v.size(); right++) {
        sum += v[right];
        while (sum >= target) {
            best = std::min(best, right - left + 1);
            sum -= v[left];
            left++;
        }
    }
    return best == (int)v.size() + 1 ? 0 : best;
}

int main() {
    std::vector<int> v = {2, 3, 1, 2, 4, 3};
    std::cout << min_subarray_len(v, 7) << std::endl;
    std::cout << min_subarray_len(v, 100) << std::endl;
    return 0;
}
```

运行结果:

```text
2
0
```

复杂度:看起来有两层循环,但每个下标,`right` 经过一次,`left` 也最多经过一次,所以总共最多 `2n` 步,`O(n)`。**判断复杂度要数"总共移动了多少步",而不是看循环有几层。** 这是第 25 课"藏起来的循环"的反面例子。

注意这个办法要求数是**正数**。如果有负数,"窗口变大和一定变大"这条性质不成立,收缩就不一定对了。

## 5. 前缀和

**问题:** 给一个数组,要回答很多次查询:"下标 `l` 到 `r` 之间所有数的和是多少?"

- 每次查询都从 `l` 加到 `r`:一次 `O(n)`,查询 q 次,总共 `O(n × q)`。

**想法:** 预先算好"从开头加到每个位置"的累加和 `prefix`,以后任何区间和,**一个减法**搞定。

```text
v      = {3, 1, 4, 1, 5, 9}
prefix = {0, 3, 4, 8, 9, 14, 23}
         prefix[i] = v[0] + ... + v[i-1],即前 i 个数的和

区间 [2, 5) 的和,即 v[2] + v[3] + v[4] = 4 + 1 + 5 = 10
= prefix[5] - prefix[2] = 14 - 4 = 10
```

为什么对:"前 5 个的和" 减去 "前 2 个的和",剩下的正好是第 3 个到第 5 个。

`prefix` 比 `v` 多一个元素,开头放一个 0,这样查询从 0 开始的区间(`prefix[r] - prefix[0]`)不用特殊处理。区间同样用左闭右开 `[l, r)`。

```cpp
// file: prefix_sum.cpp
#include <iostream>
#include <vector>

class PrefixSum {
public:
    PrefixSum(const std::vector<int>& v) : prefix(v.size() + 1, 0) {
        for (std::size_t i = 0; i < v.size(); i++) {
            prefix[i + 1] = prefix[i] + v[i];
        }
    }

    long long range_sum(int l, int r) const {
        return prefix[r] - prefix[l];
    }

private:
    std::vector<long long> prefix;
};

int main() {
    std::vector<int> v = {3, 1, 4, 1, 5, 9};
    PrefixSum ps(v);
    std::cout << ps.range_sum(2, 5) << std::endl;
    std::cout << ps.range_sum(0, 6) << std::endl;
    std::cout << ps.range_sum(3, 3) << std::endl;
    return 0;
}
```

运行结果:

```text
10
23
0
```

- 预处理 `O(n)`,之后每次查询 `O(1)`。查询 q 次,总共 `O(n + q)`。
- `prefix` 用 `long long`,因为很多数相加可能超过 `int` 的范围(第 2 课)。
- `range_sum(3, 3)` 是空区间,结果 0,自然成立,不需要特殊处理,这正是左闭右开的好处。
- 这个类的成员是 `vector`,所以不需要写析构函数和拷贝(第 15 课的零法则)。

## 6. 对拍结果

我把本课的四个算法(有序两数之和、去重、固定窗口最大和、可变窗口最短子数组),各自和**最笨的暴力做法**,用 5000 组随机数据对拍(含空数组、大量重复值),开着内存检查(第 12 课),结果全部一致。

## 7. 怎么选

| 看到的问题特征 | 用什么 |
| --- | --- |
| 有序数组,找满足某个关系的**两个**数 | 双指针,两头往中间 |
| 原地整理数组(去重、删除某些值) | 快慢指针 |
| 连续的**固定长度**一段的统计 | 固定窗口 |
| 连续的一段,要求"和/个数满足条件,最短/最长" | 可变窗口(要求数是正数,条件有单调性) |
| 反复查询**区间和** | 前缀和 |

它们的共同点:**利用已经算过的结果,不重复计算**。窗口滑动时,只处理进来和出去的那一个;前缀和把累加提前算好。这是第 25 课"用空间换时间"、第 26 课"记忆化"思想的又一次出现。

## 练习

1. 写函数,判断一个字符串是不是回文(正反读一样),用双指针从两头比较。
2. 写函数,把数组里所有的 0 移到末尾,其他数保持原来的顺序。要求原地、一遍扫描。(提示:快慢指针。)
3. 给一个有序数组,找出**三个**数,和为 0 的所有组合。(提示:先固定一个数,剩下的用"两头往中间"。)
4. 用前缀和写函数:给一个数组,问有多少个子区间的和等于 k。先写一个 `O(n²)` 的版本(双层循环,用前缀和算每个区间),再想想怎么优化。(进阶:需要第 31 课的哈希表。)
5. 给一个字符串,找**最长的、不含重复字符的子串**的长度。(提示:可变窗口,配合一个记录字符是否在窗口里的数组。)
6. 把 `max_window_sum` 改成"找平均数最大的连续 k 个数",输出这个平均数。

## 小结

- 这一课的"指针"是**下标**,不是第 9 课的指针。
- 双指针(两头往中间):有序数组找两数之和,`O(n)`。
- 双指针(快慢):原地去重、整理数组。
- 滑动窗口:窗口滑动一格,只处理进、出的那一个数,不重算。`O(n)`。
- 前缀和:预处理 `O(n)`,每次区间和查询 `O(1)`。`prefix[r] - prefix[l]`,区间左闭右开。
- 判断复杂度,要数"总共移动多少步"。
- 注意无符号数的减法:`v.size() - 1` 在空 vector 时会变成巨大的数。
- 写完用暴力做法对拍。
