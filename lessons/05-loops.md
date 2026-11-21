# 第 5 课:循环

## 这一课你会学到

- 用 `while` 让一段代码重复执行
- 用 `for` 写次数明确的循环
- 用 `break` 和 `continue` 控制循环
- 避免"死循环"

## 1. 为什么需要循环

要输出 100 遍"你好",总不能写 100 行 `std::cout`。**循环**能让同一段代码重复执行,直到条件不再满足为止。

## 2. while 循环

`while` 的意思是"当……的时候"。每次执行前先检查条件,为真就执行一遍大括号里的代码,然后回头再检查,直到条件为假。

```cpp
// file: while_demo.cpp
#include <iostream>

int main() {
    int i = 1;
    while (i <= 5) {
        std::cout << "第 " << i << " 次" << std::endl;
        i++;
    }
    std::cout << "结束" << std::endl;
    return 0;
}
```

运行结果:

```text
第 1 次
第 2 次
第 3 次
第 4 次
第 5 次
结束
```

执行过程:`i` 从 1 开始,每轮输出后加 1。`i` 变成 6 时,`i <= 5` 为假,循环结束。

**如果忘了写 `i++;`,`i` 永远是 1,条件永远为真,程序就会一直循环下去,停不下来。** 这叫**死循环**。万一遇到,在终端按 `Ctrl + C` 可以强行停止程序。

## 3. for 循环

循环常常有固定的三步:先设定起点,再判断是否继续,每轮结束后改变数值。`for` 把这三步写在同一行,更紧凑。

```cpp
// file: for_demo.cpp
#include <iostream>

int main() {
    for (int i = 1; i <= 5; i++) {
        std::cout << "第 " << i << " 次" << std::endl;
    }
    return 0;
}
```

它的输出和上面的 `while` 完全一样。括号里用分号分成三段:

1. `int i = 1`:开始前执行一次,设定起点。
2. `i <= 5`:每轮开始前检查,为真才继续。
3. `i++`:每轮结束后执行,让 `i` 加 1。

**次数明确时用 `for`,不确定次数、只知道"直到某个条件"时用 `while`。**

## 4. 用循环求和

循环最常见的用法:把每一轮的结果累加到一个变量里。

```cpp
// file: sum_demo.cpp
#include <iostream>

int main() {
    int sum = 0;
    for (int i = 1; i <= 100; i++) {
        sum += i;
    }
    std::cout << "1 加到 100 等于 " << sum << std::endl;
    return 0;
}
```

运行结果:

```text
1 加到 100 等于 5050
```

`sum` 一开始是 0(记得第 2 课说的:变量要给初始值),每轮把 `i` 加进去。

## 5. break 和 continue

两个词可以在循环中途改变流程:

- `break`:立刻结束整个循环。
- `continue`:跳过这一轮剩下的代码,直接进入下一轮。

```cpp
// file: break_continue.cpp
#include <iostream>

int main() {
    for (int i = 1; i <= 10; i++) {
        if (i % 2 == 0) {
            continue;
        }
        if (i > 7) {
            break;
        }
        std::cout << i << std::endl;
    }
    return 0;
}
```

运行结果:

```text
1
3
5
7
```

偶数被 `continue` 跳过;`i` 到了 9,大于 7,`break` 结束整个循环。

## 6. 循环里再放循环

循环的大括号里可以再放一个循环,叫**嵌套循环**。外层转一圈,内层要完整转完一遍。

```cpp
// file: nested.cpp
#include <iostream>

int main() {
    for (int row = 1; row <= 3; row++) {
        for (int col = 1; col <= row; col++) {
            std::cout << "*";
        }
        std::cout << std::endl;
    }
    return 0;
}
```

运行结果:

```text
*
**
***
```

第 1 行,内层转 1 次;第 2 行,转 2 次;第 3 行,转 3 次。每行结束后,外层的 `std::endl` 换行。

## 7. 用循环检查输入

不知道用户要输入几次时,用 `while` 配合 `cin`。`cin >> x` 读取成功时结果为真,读到输入结束时为假。

<!-- stdin: 3 5 2\n -->
```cpp
// file: read_all.cpp
#include <iostream>

int main() {
    int x = 0;
    int sum = 0;
    while (std::cin >> x) {
        sum += x;
    }
    std::cout << "总和:" << sum << std::endl;
    return 0;
}
```

运行时依次输入 `3 5 2`,然后按 `Ctrl + D`(在 WSL 里表示"输入结束"),得到:

```text
总和:10
```

## 练习

1. 用 `for` 输出 1 到 20 中所有的偶数。
2. 求 1 到 n 的乘积(n 由用户输入,n 取 10 以内)。
3. 输出九九乘法表。(提示:两层循环。)
4. 猜数字:程序里藏一个数,让用户反复输入猜测,直到猜对为止,每次提示"大了"或"小了"。

## 小结

- `while`:条件为真就一直重复。循环里要有让条件最终变假的代码,否则死循环。
- `for`:把起点、条件、每轮变化写在一行,适合次数明确的情况。
- `break` 结束循环,`continue` 跳到下一轮。
- 循环里可以再放循环。
