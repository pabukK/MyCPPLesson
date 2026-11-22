# 第 29 课:查找与二分

## 这一课你会学到

- 顺序查找:一个个找
- 二分查找:在有序数据里,每次扔掉一半
- 二分查找为什么容易写错,以及怎么写对
- 标准库的 `lower_bound`、`upper_bound`、`binary_search`
- "对答案二分":二分的进阶用法

## 1. 顺序查找

最直接的办法:从头到尾,一个个看。

```cpp
// file: linear_search.cpp
#include <iostream>
#include <vector>

int linear_search(const std::vector<int>& v, int target) {
    for (int i = 0; i < (int)v.size(); i++) {
        if (v[i] == target) {
            return i;
        }
    }
    return -1;
}

int main() {
    std::vector<int> v = {7, 3, 9, 1, 5};
    std::cout << linear_search(v, 9) << std::endl;
    std::cout << linear_search(v, 4) << std::endl;
    return 0;
}
```

运行结果:

```text
2
-1
```

找到了返回下标,找不到返回 -1(下标不可能是负数,所以 -1 是一个常用的"没找到"标志)。

复杂度 `O(n)`。数据没有排序时,没有更好的办法,因为你无法知道目标在哪。标准库里对应的是 `std::find`(第 19 课)。

## 2. 二分查找

如果数据**已经排好序**,就有更快的办法。回忆第 25 课的通讯录:先翻到中间,比较,扔掉一半。

猜数字游戏是同样的道理:我心里想一个 1 到 100 的数,你猜 50,我说"大了",你就知道答案在 1 到 49 之间,再猜 25……最多 7 次一定猜到。

**想法:** 用 `left` 和 `right` 表示还可能含有目标的范围。每次看中间的数 `mid`:

- 中间的数等于目标:找到了。
- 中间的数比目标小:目标只可能在右半边,把 `left` 挪到 `mid` 右边。
- 中间的数比目标大:目标只可能在左半边,把 `right` 挪到 `mid` 左边。

范围缩到空了,就说明没有。

```text
在 {1, 3, 5, 7, 9, 11, 13} 里找 11
范围[0, 7)  mid=3, v[3]=7  <  11 -> 去右边,范围[4, 7)
范围[4, 7)  mid=5, v[5]=11 == 11 -> 找到,下标 5
```

```cpp
// file: binary_search.cpp
#include <iostream>
#include <vector>

int binary_search_index(const std::vector<int>& v, int target) {
    int left = 0;
    int right = v.size();
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (v[mid] == target) {
            return mid;
        } else if (v[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return -1;
}

int main() {
    std::vector<int> v = {1, 3, 5, 7, 9, 11, 13};
    std::cout << binary_search_index(v, 11) << std::endl;
    std::cout << binary_search_index(v, 1) << std::endl;
    std::cout << binary_search_index(v, 13) << std::endl;
    std::cout << binary_search_index(v, 6) << std::endl;
    return 0;
}
```

运行结果:

```text
5
0
6
-1
```

和第 28 课一样,用**左闭右开**区间 `[left, right)`:

- `left < right`:范围里还有元素,继续。
- `v[mid] < target`:`mid` 以及它左边都不可能了,`left = mid + 1`。
- `v[mid] > target`:`mid` 本身不可能了,但它是"开"的一端,所以 `right = mid`。

**复杂度 `O(log n)`。** 100 万个数,最多 20 次;10 亿个数,最多 30 次。

## 3. 二分为什么容易写错

二分的思想简单,细节却极容易错。常见的三个错误:

1. **死循环**:`left = mid` 而不是 `mid + 1`,可能让范围永远不缩小。
2. **差一**:该用 `<` 还是 `<=`,`right` 是 `mid` 还是 `mid - 1`,不同的区间写法要对应(第 23 课的"边界")。
3. **溢出**:`(left + right) / 2` 在两数都很大时,相加会超过 `int` 的范围(第 2 课),所以写成 `left + (right - left) / 2`。

**防错的办法:**

- 固定使用一种区间写法,本课程统一使用**左闭右开** `[left, right)`。
- 在纸上用**一个元素、两个元素、空数组**三种极小的情况走一遍。
- 写完后用随机数据对拍(下面会做)。

## 4. 有重复值时:找边界

实际问题里,数据常有重复。比如 `{1, 2, 2, 2, 3, 5}`,要找"2 第一次出现的位置",或者"第一个大于 2 的位置"。上面的版本找到一个就返回,不一定是想要的那个。

更通用的写法,是找**第一个满足某个条件的位置**。

- `lower_bound`:第一个 **大于等于** target 的位置。
- `upper_bound`:第一个 **大于** target 的位置。

```text
v = {1, 2, 2, 2, 3, 5}
          lower_bound(2) 指向下标 1(第一个 >= 2)
                     upper_bound(2) 指向下标 4(第一个 > 2)
2 出现的次数 = upper_bound - lower_bound = 4 - 1 = 3
```

写法几乎一样:每次判断 `v[mid]` 是否"还不够大",不够就往右,够了就记下来往左:

```cpp
// file: lower_upper.cpp
#include <iostream>
#include <vector>

int lower_bound_index(const std::vector<int>& v, int target) {
    int left = 0;
    int right = v.size();
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (v[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

int upper_bound_index(const std::vector<int>& v, int target) {
    int left = 0;
    int right = v.size();
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (v[mid] <= target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

int main() {
    std::vector<int> v = {1, 2, 2, 2, 3, 5};
    int lo = lower_bound_index(v, 2);
    int hi = upper_bound_index(v, 2);
    std::cout << "lower_bound(2) = " << lo << std::endl;
    std::cout << "upper_bound(2) = " << hi << std::endl;
    std::cout << "2 出现了 " << hi - lo << " 次" << std::endl;
    std::cout << "lower_bound(4) = " << lower_bound_index(v, 4) << std::endl;
    std::cout << "lower_bound(9) = " << lower_bound_index(v, 9) << std::endl;
    return 0;
}
```

运行结果:

```text
lower_bound(2) = 1
upper_bound(2) = 4
2 出现了 3 次
lower_bound(4) = 5
lower_bound(9) = 6
```

注意:

- 两个函数**只差一个符号**:`<` 与 `<=`。
- 它们**总是返回一个位置**,不会返回 -1。target 比所有数都大,就返回 `v.size()`(末尾的后面一位,和第 19 课的 `end()` 一个意思)。
- 判断"有没有 target":先 `lower_bound` 得到位置 `p`,检查 `p < v.size() && v[p] == target`。

这两个函数,是二分最实用的形式。实际写二分,**优先用这种"找第一个满足条件的位置"的写法**,不容易错。

我用 3000 组随机有序数据(长度 0 到 29,值域很小所以重复很多),对 -1 到 16 的每个 target,把上面三个函数和标准库的结果对拍,共 54000 次检查,全部一致,并且开着内存检查(第 12 课)没有越界。

## 5. 标准库里的二分

实际写程序,直接用标准库,不用自己写。用法和 `std::sort` 一样,传入"起点、终点"。

```cpp
// file: std_binary.cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {1, 2, 2, 2, 3, 5};

    auto lo = std::lower_bound(v.begin(), v.end(), 2);
    auto hi = std::upper_bound(v.begin(), v.end(), 2);
    std::cout << "lower_bound 下标:" << (lo - v.begin()) << std::endl;
    std::cout << "upper_bound 下标:" << (hi - v.begin()) << std::endl;
    std::cout << "2 的个数:" << (hi - lo) << std::endl;

    std::cout << "有 3 吗:" << std::binary_search(v.begin(), v.end(), 3) << std::endl;
    std::cout << "有 4 吗:" << std::binary_search(v.begin(), v.end(), 4) << std::endl;
    return 0;
}
```

运行结果:

```text
lower_bound 下标:1
upper_bound 下标:4
2 的个数:3
有 3 吗:1
有 4 吗:0
```

- `lower_bound`、`upper_bound` 返回的是"位置"(第 19 课),减去 `v.begin()` 得到下标。
- `binary_search` 只回答"有没有",返回真或假。
- **前提:数据必须已经有序。** 对没排序的数据用二分,结果是错的,而且不会报错。这是最常见的误用。

另外:`std::map` 和 `std::set`(第 18 课)内部就是靠"有序"来查找的,所以它们的 `find` 是 `O(log n)`。

## 6. 二分的进阶:对答案二分

二分不只能在数组里找数。**只要答案有"单调性",就可以对答案二分。**

"单调"的意思是:答案从小到大变化时,"可不可行"的结果只会从"不行"变成"行"(或者反过来),不会来回跳。

**例子:** 要把 `{4, 7, 2, 9, 5}` 这些木头,每根切成长度相同的小段(只能切,不能拼),至少要切出 5 段,问每段最长能是多少?(长度取整数。)

观察:每段长度 `L` 越小,能切出的段数越多。`L` 变大,段数只会变少。这就是单调的。木头 `{4, 7, 2, 9, 5}` 的实际段数:

| L(每段长度) | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 能切出的段数 | 27 | 12 | 7 | 5 | 3 | 2 | 2 | 1 | 1 | 0 |

要求至少 5 段:L 从 1 到 4 都"够",从 5 开始"不够"。只要找到"够"和"不够"的分界点,就知道答案是 4。

判断"长度 L 能不能切出至少 k 段"很容易:每根木头切 `长度 / L` 段(整数除法),加起来。然后在分界线上二分,找最后一个"够"的 `L`。

```cpp
// file: binary_answer.cpp
#include <iostream>
#include <vector>

int pieces(const std::vector<int>& wood, int len) {
    int total = 0;
    for (int w : wood) {
        total += w / len;
    }
    return total;
}

int max_piece_length(const std::vector<int>& wood, int k) {
    int left = 1;
    int right = 0;
    for (int w : wood) {
        if (w > right) {
            right = w;
        }
    }
    right++;

    while (left < right) {
        int mid = left + (right - left) / 2;
        if (pieces(wood, mid) >= k) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left - 1;
}

int main() {
    std::vector<int> wood = {4, 7, 2, 9, 5};
    std::cout << "至少 5 段,最长每段 " << max_piece_length(wood, 5) << std::endl;
    std::cout << "至少 10 段,最长每段 " << max_piece_length(wood, 10) << std::endl;
    return 0;
}
```

运行结果:

```text
至少 5 段,最长每段 4
至少 10 段,最长每段 2
```

验证:长度 4,各切 1+1+0+2+1 = 5 段,够;长度 5,各切 0+1+0+1+1 = 3 段,不够。所以最大是 4。至少 10 段时,从表里看 L = 2 有 12 段够,L = 3 只有 7 段不够,所以是 2。

这段代码的思路是:`[left, right)` 里,"能"的长度都在左边,"不能"的都在右边。找到第一个"不能"的位置 `left`,再减 1,就是最后一个"能"的。

对答案二分的步骤:

1. 确定答案的**范围**(最小可能、最大可能)。
2. 写一个函数 `ok(x)`:判断"答案取 x 可不可行"。
3. 确认 `ok` 是**单调**的。
4. 二分查找"可行与不可行"的分界点。

复杂度:`O(log(范围) × ok 的复杂度)`。这里范围到 9,`ok` 是 `O(n)`。如果木头长度是 10 亿,也只要二分约 30 次。

这类题型在工程里很常见:"最少需要几台服务器才能撑住这个流量"、"最大能设多大的缓存"等等,都是对答案二分。

## 练习

1. 写函数 `bool contains(const std::vector<int>& v, int x)`,用二分查找判断有没有。
2. 给一个有序数组,有重复值,写函数返回某个值**最后一次**出现的下标,没有返回 -1。(提示:`upper_bound - 1`。)
3. 用 `std::lower_bound` 实现:往一个有序 vector 里插入一个新数,保持有序。(提示:`v.insert(位置, 值)`,第 18 课。)
4. 写函数,求 n 的整数平方根(最大的 x 使得 `x * x <= n`),用对答案二分。小心 `x * x` 溢出,用 `long long`。
5. **对拍练习:** 仿照本课,写一个随机测试程序,检验你自己写的二分和标准库一致。
6. 思考:对一个**没排序**的数组使用二分查找,会发生什么?编一组数据试试。

## 小结

- 顺序查找 `O(n)`,不要求有序。
- 二分查找 `O(log n)`,**要求数据有序**。
- 二分最实用的形式:找"第一个满足条件的位置"(`lower_bound` / `upper_bound`)。
- 二分容易错:用统一的区间写法(左闭右开),取中点用 `left + (right - left) / 2`,写完用随机数据对拍。
- 标准库:`std::lower_bound`、`std::upper_bound`、`std::binary_search`,前提是数据有序。
- 对答案二分:答案有单调性时,二分"可行与不可行"的分界点。

