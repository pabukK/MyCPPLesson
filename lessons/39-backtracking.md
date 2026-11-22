# 第 39 课:回溯

## 这一课你会学到

- 回溯是什么:一条路走到底,不行就退回来换一条
- 它和 DFS、递归的关系
- 三个经典问题:全排列、组合、子集
- 剪枝:怎么把"试遍所有可能"变得可行
- 数独、N 皇后这类约束问题

## 1. 回溯是什么

有些问题不是"求最优",而是"**把所有可能的方案都列出来**",或者"找出满足一堆条件的解"。

办法只有一个字:**试**。系统地试每一种可能,试到违反条件就停下来。

**回溯 = 递归地试 + 不行就退回来。**

它和第 35 课的 DFS 是同一件事:在"所有可能构成的树"上做深度优先搜索。这棵树叫**决策树**。

```text
给 {1, 2, 3} 排顺序,决策树:

                (空)
        /        |        \
       1         2         3
      / \       / \       / \
     2   3     1   3     1   2
     |   |     |   |     |   |
     3   2     3   1     2   1

叶子就是全部答案:123, 132, 213, 231, 312, 321
```

## 2. 全排列

```cpp
// file: permutations.cpp
#include <iostream>
#include <vector>

void backtrack(std::vector<int>& current, std::vector<bool>& used,
               const std::vector<int>& nums, std::vector<std::vector<int>>& result) {
    if ((int)current.size() == (int)nums.size()) {
        result.push_back(current);
        return;
    }
    for (int i = 0; i < (int)nums.size(); i++) {
        if (used[i]) {
            continue;
        }
        used[i] = true;
        current.push_back(nums[i]);

        backtrack(current, used, nums, result);

        current.pop_back();
        used[i] = false;
    }
}

std::vector<std::vector<int>> permute(const std::vector<int>& nums) {
    std::vector<std::vector<int>> result;
    std::vector<bool> used(nums.size(), false);
    std::vector<int> current;
    backtrack(current, used, nums, result);
    return result;
}

int main() {
    for (const auto& p : permute({1, 2, 3})) {
        for (int x : p) {
            std::cout << x;
        }
        std::cout << " ";
    }
    std::cout << std::endl;
    std::cout << "个数:" << permute({1, 2, 3}).size() << std::endl;
    std::cout << "空数组的排列数:" << permute({}).size() << std::endl;
    return 0;
}
```

运行结果:

```text
123 132 213 231 312 321 
个数:6
空数组的排列数:1
```

**回溯的固定三段式**,记住这个形状:

```cpp
if (达到叶子) { 记录答案; return; }
for (每个可选的选择) {
    做选择;              // 修改 current 和 used
    递归(进入下一层);
    撤销选择;            // 做选择的逆操作,回到进入前的状态
}
```

**"撤销选择"是回溯的灵魂。** 递归返回后,必须把状态恢复原样,否则下一个分支会带着上一个分支的残留。上面代码里,`push_back` 对应 `pop_back`,`used[i] = true` 对应 `used[i] = false`,顺序也正好相反(后进先出,第 32 课的栈)。

"空数组的排列数是 1"这个结果:空数组有一个排列,就是空排列本身。这在数学上是对的(0! = 1)。

## 3. 复杂度:为什么不能乱用

n 个不同元素的全排列有 `n!` 个。

| n | n! |
| --- | --- |
| 10 | 362 万 |
| 11 | 3995 万 |
| 12 | 4.8 亿 |
| 15 | 1.3 万亿 |

第 25 课说过,一秒大约能跑 1 亿步。所以**全排列最多做到 n = 11 左右**,再多就等不下去了。

组合和子集是 `2ⁿ`:

| n | 2ⁿ |
| --- | --- |
| 20 | 105 万 |
| 25 | 3355 万 |
| 30 | 10.7 亿 |

**子集类问题最多做到 n = 25 左右。**

这是回溯的硬限制。做题或做工程,第一件事是**估数据规模,看枚举量受不受得了**(第 25 课)。

## 4. 子集与组合

**问题:** 从 n 个里选 k 个,列出所有选法。这叫组合。子集就是"k 可以是任何值"。

全排列要防止"重复选同一个元素",用 `used` 数组。组合不需要 `used`,但需要防止"重复选同一组":规定**只能往后选**,不能回头。

```cpp
// file: combinations.cpp
#include <iostream>
#include <vector>

void backtrack(int start, int n, int k, std::vector<int>& current, std::vector<std::vector<int>>& result) {
    if ((int)current.size() == k) {
        result.push_back(current);
        return;
    }
    for (int i = start; i <= n; i++) {
        current.push_back(i);
        backtrack(i + 1, n, k, current, result);
        current.pop_back();
    }
}

std::vector<std::vector<int>> combine(int n, int k) {
    std::vector<std::vector<int>> result;
    std::vector<int> current;
    backtrack(1, n, k, current, result);
    return result;
}

int main() {
    for (const auto& c : combine(4, 2)) {
        for (int x : c) {
            std::cout << x;
        }
        std::cout << " ";
    }
    std::cout << std::endl;
    std::cout << "C(4,2) 个数:" << combine(4, 2).size() << std::endl;
    std::cout << "C(4,0) 个数:" << combine(4, 0).size() << std::endl;
    std::cout << "C(4,5) 个数:" << combine(4, 5).size() << std::endl;
    return 0;
}
```

运行结果:

```text
12 13 14 23 24 34 
C(4,2) 个数:6
C(4,0) 个数:1
C(4,5) 个数:0
```

- `start` 参数:本轮只能从 `start` 开始往后选。递归时传 `i + 1`,不许回头。这样 `1,2` 会出现,`2,1` 不会,天然去掉了顺序造成的重复。
- `n` 参数:一共是从 `1` 到 `n` 里选,所以循环的上界写的是 `n`,不能写死成一个常数。
- 三个边界都对:`k = 0` 只有空组合(1 个);`k > n` 选不出来(0 个)。

**子集**(所有可能的选法,包括空集和全集),把"达到 k 个才记录"改成"每一层都记录":

```cpp
// file: subsets.cpp
#include <iostream>
#include <vector>

void backtrack(int start, std::vector<int>& current, const std::vector<int>& nums,
               std::vector<std::vector<int>>& result) {
    result.push_back(current);
    for (int i = start; i < (int)nums.size(); i++) {
        current.push_back(nums[i]);
        backtrack(i + 1, current, nums, result);
        current.pop_back();
    }
}

int main() {
    std::vector<int> nums = {1, 2, 3};
    std::vector<std::vector<int>> result;
    std::vector<int> current;
    backtrack(0, current, nums, result);

    for (const auto& s : result) {
        std::cout << "{";
        for (int i = 0; i < (int)s.size(); i++) {
            std::cout << (i ? "," : "") << s[i];
        }
        std::cout << "} ";
    }
    std::cout << std::endl;
    std::cout << "个数:" << result.size() << "(应为 2^3 = 8)" << std::endl;
    return 0;
}
```

运行结果:

```text
{} {1} {1,2} {1,2,3} {1,3} {2} {2,3} {3} 
个数:8(应为 2^3 = 8)
```

正好 8 个,和 `2ⁿ` 对上。这个"每一层都记录"的写法,是子集问题的标准形状。

## 5. 有重复元素时:剪枝

**问题:** 给 `{1, 2, 2}`,列出所有不重复的子集。

如果直接套用上面的代码,`{1,2}` 会出现两次(选第一个 2,或选第二个 2)。要去重,先**排序**,让相同的数挨在一起,然后加一条规则:

> 同一层里,相同的数只选第一个。

```cpp
// file: subsets_dup.cpp
#include <algorithm>
#include <iostream>
#include <vector>

void backtrack(int start, std::vector<int>& current, const std::vector<int>& nums,
               std::vector<std::vector<int>>& result) {
    result.push_back(current);
    for (int i = start; i < (int)nums.size(); i++) {
        if (i > start && nums[i] == nums[i - 1]) {
            continue;
        }
        current.push_back(nums[i]);
        backtrack(i + 1, current, nums, result);
        current.pop_back();
    }
}

int main() {
    std::vector<int> nums = {1, 2, 2};
    std::sort(nums.begin(), nums.end());

    std::vector<std::vector<int>> result;
    std::vector<int> current;
    backtrack(0, current, nums, result);

    for (const auto& s : result) {
        std::cout << "{";
        for (int i = 0; i < (int)s.size(); i++) {
            std::cout << (i ? "," : "") << s[i];
        }
        std::cout << "} ";
    }
    std::cout << std::endl;
    std::cout << "个数:" << result.size() << std::endl;
    return 0;
}
```

运行结果:

```text
{} {1} {1,2} {1,2,2} {2} {2,2} 
个数:6
```

`{1,2,2}` 去重后的全部子集就是这 6 个:`{}`、`{1}`、`{2}`、`{1,2}`、`{2,2}`、`{1,2,2}`。对照着数一遍,确认没有漏也没有重。

- `if (i > start && nums[i] == nums[i - 1]) continue;` 就是剪枝。
- 为什么是 `i > start` 而不是 `i > 0`?因为**同一层**里,相同的数只取第一个;但**不同层**里,相同的数可以都取(比如 `{2,2}` 要保留)。`i > start` 精确表达了"在同一层里往后看"。
- 少了排序,这条剪枝就无效。
- `{1,2}` 只出现一次,而不是两次。

**这一条剪枝,是回溯题里最容易写错的一行。** 三种常见错法:写成 `i > 0`(会误杀 `{2,2}`)、不排序(剪不掉)、写成 `nums[i] == nums[i+1]`(方向错)。

## 6. N 皇后:约束类回溯

**问题:** n×n 的棋盘,放 n 个皇后,使任意两个都不在同一行、同一列、同一条斜线。列出所有摆法。

皇后可攻击:同行、同列、同对角线。对角线的判断有个技巧:

```text
格子 (r, c):
  主对角线(左上到右下):r - c 相同
  副对角线(右上到左下):r + c 相同
```

所以只要记录"哪些列、哪些 `r-c`、哪些 `r+c` 已经被占了",就能 `O(1)` 判断。

```cpp
// file: n_queens.cpp
#include <iostream>
#include <vector>

void backtrack(int row, int n, std::vector<int>& queens,
               std::vector<bool>& col_used, std::vector<bool>& diag_used,
               std::vector<bool>& anti_used, int& count) {
    if (row == n) {
        count++;
        return;
    }
    for (int c = 0; c < n; c++) {
        int diag = row - c + (n - 1);
        int anti = row + c;
        if (col_used[c] || diag_used[diag] || anti_used[anti]) {
            continue;
        }
        col_used[c] = diag_used[diag] = anti_used[anti] = true;
        queens.push_back(c);

        backtrack(row + 1, n, queens, col_used, diag_used, anti_used, count);

        queens.pop_back();
        col_used[c] = diag_used[diag] = anti_used[anti] = false;
    }
}

int solve(int n) {
    int count = 0;
    std::vector<int> queens;
    std::vector<bool> col_used(n, false);
    std::vector<bool> diag_used(2 * n - 1, false);
    std::vector<bool> anti_used(2 * n - 1, false);
    backtrack(0, n, queens, col_used, diag_used, anti_used, count);
    return count;
}

int main() {
    for (int n : {1, 2, 3, 4, 5, 6, 8}) {
        std::cout << "n=" << n << " 摆法:" << solve(n) << std::endl;
    }
    return 0;
}
```

运行结果:

```text
n=1 摆法:1
n=2 摆法:0
n=3 摆法:0
n=4 摆法:2
n=5 摆法:10
n=6 摆法:4
n=8 摆法:92
```

几个要点:

- **逐行放置**:每行放一个,行号就是递归的深度,天然避免了"同一行有两个皇后"。这是约束类回溯的常用简化:把一部分约束用结构本身消掉。
- `diag = row - c + (n - 1)`:因为 `row - c` 可能是负数,而数组下标不能为负,所以整体平移 `n-1`,变成 `0` 到 `2n-2`。这类"把可能为负的下标平移"的写法,在网格、棋盘问题里很常见。
- 三个 `used` 数组,设置时一起设,撤销时一起撤(顺序反过来也一样,因为它们是独立的)。
- `n=2`、`n=3` 是 0 种:**无解**。`n=8` 是 92 种,这是这道经典题的公认答案,可以拿来验证自己的程序对不对。
- 这版只数个数,没存具体摆法。要输出摆法,把 `queens` 在 `row == n` 时拷进结果。

**回溯的通用套路**(把 N 皇后抽象出来):

1. 确定"一层"代表什么决策(N 皇后:一行放哪个位置)。
2. 维护"已经被占用的资源"(列、两条对角线)。
3. 每层:遍历所有候选 → 检查是否违反约束 → 做选择 → 递归 → 撤销。
4. 到达最后一层:记录答案。

数独(第 7 节的练习)是同一个套路,只是约束换成"同行、同列、同 3×3 宫内 1 到 9 各出现一次"。

## 7. 回溯 vs 动态规划

| | 回溯 | 动态规划 |
| --- | --- | --- |
| 目的 | 列出**所有**方案,或找任一可行方案 | 求**最优值**(个数、最大、最小) |
| 做法 | 枚举决策树,不行就退 | 填表,不重复计算 |
| 能接受的数据规模 | 很小(n ≤ 20 左右) | 较大(n 到 10⁴、10⁵) |
| 关系 | 同一个问题,常常两种都能做 |  |

比如"凑数":给一些数,问能凑出多少种组合使总和为 target。

- 要**列出所有组合** → 回溯。
- 只要**种数** → 背包 DP,`O(n × target)`。

**判断标准:题目要"所有方案"还是"一个数字"。** 后者能 DP,就别枚举。

## 练习

1. 给 `permute` 加剪枝,处理有重复元素的输入(`{1, 1, 2}` 的全排列,应得 3 个)。
2. 实现组合总和:给一组数和目标值,找出所有和为 target 的组合,每个数可以重复使用。想想 `start` 该传 `i` 还是 `i + 1`。
3. 解数独:给一个 9×9 的部分填好的数独,输出完整解。
4. 括号生成:生成 n 对括号的所有合法组合。`n = 3` 应有 5 种。想想剪枝条件(左括号剩余数、右括号剩余数)。
5. 单词搜索:给一个二维字母网格和一个单词,判断单词能不能在网格里沿上下左右连出来。注意同一个格子不能在一条路径里用两次。
6. 把 N 皇后改成输出具体的摆法(用 `*` 和 `.` 画棋盘),验证 `n = 4` 的两种。
7. 实测:`solve(12)` 要多少秒?`solve(14)` 呢?体会枚举的增长速度。

## 小结

- 回溯 = 递归地试 + 不行就退回来撤销。
- 固定三段式:做选择 → 递归 → **撤销选择**。忘了撤销,后面的分支就带着脏状态。
- 全排列用 `used` 防重复选;组合和子集用 `start` 防回头选。
- 有重复元素要先排序,剪枝条件是 `i > start && nums[i] == nums[i-1]`。
- 复杂度是 `n!` 或 `2ⁿ`,**数据规模必须很小**(n 到 20 左右)。
- N 皇后的对角线技巧:`r - c` 相同是主对角线,`r + c` 相同是副对角线;下标可能为负,要平移。
- 要"所有方案"用回溯;只要"一个数字"用 DP。
- `n = 8` 皇后有 92 种解,可以拿来验证程序。
