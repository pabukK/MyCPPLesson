# 第 23 课:调试与单元测试

## 这一课你会学到

- 程序结果不对时,怎么一步步查出原因
- 用 `gdb` 让程序一行一行地运行,边走边看变量
- 写单元测试:让电脑自己检查你的函数对不对

## 1. 结果不对,怎么办

下面这个程序,想算 1 加到 10 的和,正确答案是 55:

```cpp
// file: buggy.cpp
#include <iostream>

int sum_to(int n) {
    int total = 0;
    for (int i = 1; i < n; i++) {
        total += i;
    }
    return total;
}

int main() {
    int result = sum_to(10);
    std::cout << "1 到 10 的和:" << result << std::endl;
    return 0;
}
```

运行结果:

```text
1 到 10 的和:45
```

45,不是 55。程序没有崩溃,也没有报错,就是结果不对。这种错叫**逻辑错误**,编译器看不出来,只能靠你找。

## 2. 最简单的办法:打印

在可疑的地方多输出几个变量,看它们是不是你想的那样:

```cpp
for (int i = 1; i < n; i++) {
    std::cout << "i=" << i << " total=" << total << std::endl;
    total += i;
}
```

这个办法简单有效,小程序常用。缺点是:要改代码、重新编译,而且输出一多就看花眼。

## 3. 用 gdb 一行一行地走

**调试器**可以让程序暂停在你指定的位置,然后一行一行地执行,随时查看变量。本课程用的调试器叫 `gdb`,你的 WSL 里已经装好了。

使用前,编译时要加两个选项:

```bash
g++ -std=c++17 -g -O0 -o buggy buggy.cpp
```

- `-g`:让程序带上行号等信息,调试器才知道每行对应什么。(第 12 课也用过。)
- `-O0`:关闭优化。开启优化后,编译器会重排代码,调试时看到的执行顺序会和源码对不上。

启动 gdb:

```bash
gdb ./buggy
```

进入以后,出现 `(gdb)` 提示符,在这里输入命令。最常用的几个:

| 命令 | 简写 | 作用 |
| --- | --- | --- |
| `break 函数名` | `b` | 在这个函数入口设置**断点**,程序运行到这里会暂停 |
| `run` | `r` | 开始运行程序 |
| `next` | `n` | 执行当前这一行,停在下一行 |
| `step` | `s` | 和 `next` 类似,但如果当前行调用了函数,会走进那个函数里 |
| `print 变量` | `p` | 显示变量现在的值 |
| `finish` | | 一口气运行到当前函数结束,显示返回值 |
| `continue` | `c` | 继续运行,直到遇到下一个断点或程序结束 |
| `quit` | `q` | 退出 gdb |

一次完整的调试过程:

```text
(gdb) break sum_to
Breakpoint 1 at 0x1194: file buggy.cpp, line 4.
(gdb) run
Breakpoint 1, sum_to (n=10) at buggy.cpp:4
4	    int total = 0;
(gdb) next
5	    for (int i = 1; i < n; i++) {
(gdb) next
6	        total += i;
(gdb) next
5	    for (int i = 1; i < n; i++) {
(gdb) next
6	        total += i;
(gdb) print i
$1 = 2
(gdb) print total
$2 = 1
(gdb) finish
Value returned is $3 = 45
```

(每行前面的数字是行号。具体的地址和行号,你的屏幕上可能略有不同。)

解读:

- 断点停在 `sum_to` 开头,参数 `n` 是 10,没问题。
- 一步步走,看到循环里 `i` 从 1 开始。
- 观察这一步:`i = 2` 时,`total = 1`,说明只加了 1,正常。
- 用 `finish` 看返回值是 45。

要看出问题所在,可以继续走几十步直到循环结束,看 `i` 最后是多少。更直接的办法是想想:循环条件是 `i < n`,当 `n` 是 10 时,`i` 到 9 就停了,**10 没有被加进去**。条件应该是 `i <= n`。

改成 `i <= n` 重新编译,得到 55。

调试的核心思路:**不要猜,要看。** 让程序暂停在出错之前,看每个变量是不是和你想的一致,第一个"不一致"的地方,就是 bug 所在。

## 4. 单元测试:让电脑替你检查

调试是"出了错再去找"。**单元测试**是"提前写好检查,每次改完代码就自动跑一遍",有错立刻发现。

做法:对每个函数,写几组"输入"和"应该得到的结果",让程序自己比对。

最简单的写法,用标准库里的 `assert`。它的意思是"断言":括号里的条件必须为真,否则程序立刻终止并报告是哪一行。

```cpp
// file: assert_demo.cpp
#include <cassert>
#include <iostream>

int sum_to(int n) {
    int total = 0;
    for (int i = 1; i <= n; i++) {
        total += i;
    }
    return total;
}

int main() {
    assert(sum_to(1) == 1);
    assert(sum_to(10) == 55);
    assert(sum_to(0) == 0);

    std::cout << "全部测试通过" << std::endl;
    return 0;
}
```

运行结果:

```text
全部测试通过
```

如果有一条不成立,比如把 `i <= n` 改回 `i < n`,会看到类似这样的报告:

```text
assert_demo: assert_demo.cpp:14: int main(): Assertion `sum_to(1) == 1' failed.
```

它告诉你:第 14 行的这条测试没通过,而且是**第一条**就失败了。因为 `i < 1` 时循环一次都不执行,`sum_to(1)` 得到 0 而不是 1。程序在第一条失败的断言处就终止了,后面的不会再检查。

## 5. 测哪些情况

好的测试不只测"正常情况",更要测**边界**:

- 最小的输入:`0`、空字符串、空 `vector`。
- 刚好在边界上的值:比如判断及格的 `59` 和 `60`。
- 不合法的输入:该抛异常的地方,是否真的抛了。

边界是 bug 最爱藏的地方。上面 `i < n` 的错,就是一个典型的"差一"错误。

## 6. 测试要能测"抛异常"

第 20 课的函数 `divide(1, 0)` 应该抛异常。测试它:

```cpp
// file: test_exception.cpp
#include <cassert>
#include <iostream>
#include <stdexcept>

double divide(double a, double b) {
    if (b == 0) {
        throw std::runtime_error("除数不能为 0");
    }
    return a / b;
}

int main() {
    assert(divide(10, 2) == 5);

    bool caught = false;
    try {
        divide(1, 0);
    } catch (const std::runtime_error&) {
        caught = true;
    }
    assert(caught);

    std::cout << "全部测试通过" << std::endl;
    return 0;
}
```

运行结果:

```text
全部测试通过
```

`caught` 记录异常有没有被接住,最后断言它为真。`catch (const std::runtime_error&)` 没有写变量名,因为我们不需要用错误信息。

## 7. assert 的一个陷阱

编译时如果加上 `-DNDEBUG`(很多项目的"发布版"会加,同时开优化),所有 `assert` 都会被**直接删掉,完全不执行**。这样 `assert(1 == 2)` 也不会报错。

所以:

- `assert` 适合写在**测试程序**里,测试程序不要加 `-DNDEBUG`。
- 不要把必须执行的代码放进 `assert` 括号里,比如 `assert(do_work());`。加了 `-DNDEBUG`,`do_work()` 根本不会被调用。

## 8. 更正式的测试框架

`assert` 的缺点:第一条失败就整个终止,后面的不检查;报告也比较简陋。

实际项目里常用测试框架,比如 **GoogleTest**、**Catch2**。它们能列出所有失败的测试、给出更清楚的对比信息。使用它们需要先安装,并配合第 22 课的 CMake。本课程不展开安装过程,你只需要知道:

- 它们的思路和 `assert` 完全一样:准备输入,检查结果。
- 差别只是把每一组测试起个名字,分开运行,汇总报告。

当你写更大的程序时,再去学其中一个。有了本课的基础,上手会很快。

## 9. 一个实用的调试流程

遇到结果不对,按这个顺序来:

1. **复现**:找到一组稳定能出错的输入。偶尔出错的最难查,先想办法让它每次都出错。
2. **缩小**:把输入变小,把代码删到最少,仍然出错。范围越小越好查。
3. **观察**:用输出或 gdb,看变量的真实值,找到第一个和预期不一致的地方。
4. **修改**:只改一处,重新运行。不要同时改好几处。
5. **加测试**:把这个出错的输入写成一条测试,以后不会再犯同样的错。

涉及内存的问题(崩溃、莫名其妙的值),先用第 12 课的 AddressSanitizer 跑一遍,往往直接就告诉你出错的行号。

## 练习

1. 用 gdb 调试 `buggy.cpp`:在 `main` 处设断点,用 `step` 走进 `sum_to`,观察每轮循环里 `i` 和 `total` 的变化。
2. 为第 6 课练习里的 `is_even`、`max_of`、`factorial` 各写几条 `assert` 测试,包含边界值(0、负数)。
3. 写一个函数 `int middle(int a, int b, int c)` 返回三个数中间大的那个,用至少 6 条测试覆盖各种大小顺序。
4. 故意写一个有 bug 的函数,用测试发现它,再用 gdb 找出原因。

## 小结

- 逻辑错误没有报错,要靠你观察。原则:不要猜,要看。
- 编译时加 `-g -O0`,用 gdb 设断点、一步步执行、查看变量。
- 常用 gdb 命令:`break`、`run`、`next`、`step`、`print`、`finish`、`continue`、`quit`。
- 单元测试:准备输入,检查结果,让电脑替你检查。
- 测试要覆盖边界情况。
- `assert` 在加了 `-DNDEBUG` 时会被忽略。
- 调试流程:复现、缩小、观察、修改、加测试。
