# 第 28 课:高效排序:归并、快速、计数

## 这一课你会学到

- 归并排序:先分成两半各自排好,再合并
- 快速排序:选一个基准,把数据分成"小的"和"大的"两边
- 计数排序:不比较大小也能排序的特殊办法
- 为什么"基于比较的排序"最快只能到 `O(n log n)`

## 1. 上一课的瓶颈

上一课的三种排序都是 `O(n²)`。第 25 课的表格说过,数据 10 万个,`O(n²)` 要跑 100 秒,`O(n log n)` 只要 0.02 秒。这一课学两种 `O(n log n)` 的排序。它们共同的思想是**分治**:

> **分治 = 分开 + 解决 + 合并。** 把大问题拆成两个小问题,分别解决,再把结果合起来。

这正是第 26 课的递归。

## 2. 归并排序

**想法:**

1. 把数据从中间分成左右两半。
2. 左半、右半**各自排好序**(怎么排?递归,同样的方法)。
3. 把两个已经排好序的半边,**合并**成一个有序的整体。

合并是关键。两个已排好序的序列合并很容易:各用一个"指针"指着开头,每次取两边较小的那个放进结果,然后那一边的指针前进一步。

```text
左 [1 4 7]   右 [2 3 9]
比较 1 和 2,取 1       -> 结果 [1]
比较 4 和 2,取 2       -> 结果 [1 2]
比较 4 和 3,取 3       -> 结果 [1 2 3]
比较 4 和 9,取 4       -> 结果 [1 2 3 4]
比较 7 和 9,取 7       -> 结果 [1 2 3 4 7]
左边用完了,把右边剩下的 9 接上 -> [1 2 3 4 7 9]
```

```cpp
// file: merge_sort.cpp
#include <iostream>
#include <vector>

void merge_range(std::vector<int>& v, std::vector<int>& temp, int left, int mid, int right) {
    int i = left;
    int j = mid;
    int k = left;
    while (i < mid && j < right) {
        if (v[i] <= v[j]) {
            temp[k++] = v[i++];
        } else {
            temp[k++] = v[j++];
        }
    }
    while (i < mid) {
        temp[k++] = v[i++];
    }
    while (j < right) {
        temp[k++] = v[j++];
    }
    for (int p = left; p < right; p++) {
        v[p] = temp[p];
    }
}

void merge_sort_range(std::vector<int>& v, std::vector<int>& temp, int left, int right) {
    if (right - left <= 1) {
        return;
    }
    int mid = left + (right - left) / 2;
    merge_sort_range(v, temp, left, mid);
    merge_sort_range(v, temp, mid, right);
    merge_range(v, temp, left, mid, right);
}

void merge_sort(std::vector<int>& v) {
    std::vector<int> temp(v.size());
    merge_sort_range(v, temp, 0, v.size());
}

int main() {
    std::vector<int> v = {5, 3, 8, 1, 9, 2, 7};
    merge_sort(v);
    for (int x : v) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

运行结果:

```text
1 2 3 5 7 8 9 
```

逐处看:

- 范围用**左闭右开**表示:`[left, right)` 包含 `left`,不包含 `right`。和第 19 课的 `begin()`、`end()` 是同一个习惯。这样,长度就是 `right - left`,拆成两半时不容易差一(第 23 课)。
- 终止条件:范围里不足 2 个元素,不用排。
- `mid = left + (right - left) / 2`:取中点。这种写法比 `(left + right) / 2` 稳妥,后者在 `left + right` 很大时可能溢出(第 2 课)。
- 合并时用到一个临时数组 `temp`。它在最外面只创建一次,传引用(第 10 课)给各层使用,避免每次递归都申请内存。
- `v[i] <= v[j]`:相等时取左边的,保证**稳定**(第 27 课)。

**复杂度:**

- 每一层要处理全部 n 个元素(合并时每个元素放进 `temp` 一次)。
- 一共有多少层?每次折半,大约 `log n` 层。
- 总共 `O(n log n)`,而且**不管数据什么样,都是这个速度**。
- 额外空间 `O(n)`(那个 `temp`)。

## 3. 快速排序

**想法:**

1. 从数据里选一个数,叫**基准**。
2. 把其他数分成两堆:**比基准小的放左边,比基准大的放右边**。基准自己就落在了它最终的位置上。
3. 左边一堆、右边一堆,**各自递归地**用同样的方法排序。

像老师给学生排队:选一个同学当"标杆",比他矮的站左边,比他高的站右边,然后两边各自再选标杆,重复。

难点在第 2 步:怎么高效地分两堆?一种常用做法,用一个"小的区域"的边界下标 `store`:

```text
基准放在最右边。从左往右扫描每个数:
  如果这个数比基准小,就和 store 位置的数交换,store 后移一格
扫描完,把基准和 store 位置交换,基准就到了中间

例:{5, 3, 8, 1, 4},基准 4
5  比 4 大,不动          store=0
3  比 4 小,和 v[0] 交换 -> {3, 5, 8, 1, 4}   store=1
8  比 4 大,不动
1  比 4 小,和 v[1] 交换 -> {3, 1, 8, 5, 4}   store=2
最后基准 4 和 v[2] 交换   -> {3, 1, 4, 5, 8}
4 左边都比它小,右边都比它大
```

```cpp
// file: quick_sort.cpp
#include <iostream>
#include <utility>
#include <vector>

int partition(std::vector<int>& v, int left, int right) {
    int pivot_index = left + (right - left) / 2;
    std::swap(v[pivot_index], v[right - 1]);
    int pivot = v[right - 1];

    int store = left;
    for (int i = left; i < right - 1; i++) {
        if (v[i] < pivot) {
            std::swap(v[i], v[store]);
            store++;
        }
    }
    std::swap(v[store], v[right - 1]);
    return store;
}

void quick_sort_range(std::vector<int>& v, int left, int right) {
    if (right - left <= 1) {
        return;
    }
    int p = partition(v, left, right);
    quick_sort_range(v, left, p);
    quick_sort_range(v, p + 1, right);
}

void quick_sort(std::vector<int>& v) {
    quick_sort_range(v, 0, v.size());
}

int main() {
    std::vector<int> v = {5, 3, 8, 1, 9, 2, 7, 4};
    quick_sort(v);
    for (int x : v) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

运行结果:

```text
1 2 3 4 5 7 8 9 
```

- 范围同样是左闭右开。
- `partition` 函数负责"分两堆",返回基准最终的位置 `p`。
- 基准选**中间**的元素,先把它换到最右边,方便扫描。为什么不直接选第一个?下面解释。
- 递归时,基准自己(位置 `p`)已经在正确位置了,所以左边处理 `[left, p)`,右边处理 `[p + 1, right)`。

**复杂度:**

- 理想情况:每次基准都落在中间,分成大小差不多的两半,和归并一样,有 `log n` 层,每层 `O(n)`,合计 `O(n log n)`。
- **最坏情况:** 每次基准都是最大或最小的数,分出来一边是 0 个,一边是 n-1 个,要分 n 层,总共 `O(n²)`。
- 额外空间:只有递归的栈,平均 `O(log n)`。不用像归并那样另开一个数组。

## 4. 基准选不好,会怎么样

如果**总是选第一个或最后一个**当基准,碰上**已经有序**的数据,每次选的都是最小(或最大)的数,正好是最坏情况。有序的数据在实际中很常见,所以这不是什么冷门情形。

我实测了一下:把基准的选法,在"选最右边"和"选中间"之间切换,对 2 万个数据排序:

| 数据 | 基准选法 | 比较次数 | 用时 |
| --- | --- | --- | --- |
| 已经有序 | 最右边 | 199,990,000 | 1724 毫秒 |
| 已经有序 | 中间 | 247,248 | 1.6 毫秒 |
| 随机 | 最右边 | 334,977 | 3.2 毫秒 |
| 随机 | 中间 | 339,342 | 3.1 毫秒 |

(没开优化,你的用时会不同,看相对大小。)

有序数据 + 选最右边当基准:比较了约 2 亿次,正好是 `n²/2`(2 万的平方除以 2),退化成了 `O(n²)`,慢了约 1000 倍。换成选中间,一下恢复正常。随机数据上,两种选法没有差别。

选中间只是**比较简单的补救**。精心构造的数据,仍然可以让"选中间"退化。实际中更稳妥的办法有:

- **随机选**一个位置当基准,这样没人能事先构造出最坏数据。
- **三数取中**:取开头、中间、结尾三个数的中位数当基准。

标准库 `std::sort` 用的是更完善的办法(内部会在退化时自动换成别的排序),**保证最坏也是 `O(n log n)`**。所以工程里直接用 `std::sort`。

## 5. 归并 vs 快速

| | 归并排序 | 快速排序 |
| --- | --- | --- |
| 最坏复杂度 | `O(n log n)`,稳定 | `O(n²)`(基准选得糟) |
| 平均速度 | 快 | 通常更快一点(访问内存更连贯) |
| 额外空间 | `O(n)` | `O(log n)`(递归栈) |
| 稳定吗 | 稳定 | **不稳定** |
| 适合 | 要求稳定、数据在链表或硬盘上 | 内存里的数组,追求速度 |

我把归并、快速排序各和 `std::sort` 用 500 组随机数据(长度 0 到 59,包含大量重复值)逐个对拍,全部一致。

## 6. 不比较也能排:计数排序

前面所有排序,都是通过**比较两个数的大小**来排的。可以证明:**只靠比较,最快也只能做到 `O(n log n)`**(想象一棵决策树:n 个数有 n! 种排列,每次比较最多排除一半的可能,至少要比较 `log(n!)` 次,约等于 `n log n`)。

但如果数据有特殊性,可以不用比较。比如:**一堆整数,范围很小**。要给 100 万个学生的分数(0 到 100)排序。

**想法:** 开一个"计数器"数组,`count[s]` 记录分数 s 出现了几次。扫一遍数据,统计完;再按 0 到 100 的顺序,把每个分数输出 `count[s]` 次。

```cpp
// file: counting_sort.cpp
#include <iostream>
#include <vector>

void counting_sort(std::vector<int>& v, int max_value) {
    std::vector<int> count(max_value + 1, 0);
    for (int x : v) {
        count[x]++;
    }
    int k = 0;
    for (int value = 0; value <= max_value; value++) {
        for (int c = 0; c < count[value]; c++) {
            v[k++] = value;
        }
    }
}

int main() {
    std::vector<int> scores = {90, 85, 70, 90, 100, 0, 85, 60, 90};
    counting_sort(scores, 100);
    for (int s : scores) {
        std::cout << s << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

运行结果:

```text
0 60 70 85 85 90 90 90 100 
```

- 数据个数 n,取值范围 m(这里是 101)。
- 复杂度 `O(n + m)`:扫一遍数据加扫一遍计数器。
- **前提:** 数据必须是**不太大的非负整数**。如果范围是 0 到 10 亿,计数器数组要占几 GB 内存,就不划算了。
- 这是第 25 课说的"用空间换时间"的典型例子。

适用场合:分数、年龄、字母(按 ASCII 码 0 到 255 排序)、日期里的月份……

(这里的写法只适用于整数本身。要排"带信息的对象"并保持稳定,需要更复杂的写法,本课不展开。)

## 7. 实际工程里怎么做

| 需求 | 做法 |
| --- | --- |
| 普通排序 | `std::sort`,最坏 `O(n log n)`,不稳定 |
| 同分要保持原顺序 | `std::stable_sort`,稳定,`O(n log n)` |
| 只要前 k 大/小,不需要全排序 | `std::partial_sort`、`std::nth_element`(第 43 课) |
| 整数且范围很小 | 计数排序 |

## 练习

1. 手动模拟归并排序处理 `{4, 2, 7, 1}`,写出每一次拆分和合并后的样子。
2. 在 `merge_sort.cpp` 里,把 `v[i] <= v[j]` 改成 `v[i] < v[j]`,排序结果还是对的吗?稳定性会受影响吗?用带编号的数据验证。
3. 修改快速排序,让基准**随机**选(`#include <random>`,`std::mt19937 rng; rng() % (right - left)`)。再用有序数据测试,比较次数是否正常。
4. 用 `std::stable_sort` 和一个 lambda(第 19 课),给 `vector<Student>` 按分数排序,验证同分的学生保持原来的顺序。
5. 写一个对拍程序:随机生成数据,让你写的归并、快速、计数排序和 `std::sort` 比对,有错就输出那组数据。
6. 思考:如果要对 1000 万个数排序,选择排序大约要多久?归并排序呢?(参考第 25 课的算法。)

## 小结

- 分治:分开、各自解决、合并结果。
- 归并排序:先排好两半,再合并。稳定,`O(n log n)`,要额外 `O(n)` 空间。
- 快速排序:选基准,分成小的、大的两边,各自递归。平均 `O(n log n)`,但基准选得糟时退化成 `O(n²)`。
- 基准别总选第一个或最后一个,至少选中间,更好的是随机或三数取中。
- 只靠比较的排序,理论上最快 `O(n log n)`。
- 计数排序:数据是范围小的整数时,可以做到 `O(n + m)`。
- 实际工程直接用 `std::sort` 或 `std::stable_sort`。

