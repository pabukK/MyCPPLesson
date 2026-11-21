# 第 4 课:条件判断

## 这一课你会学到

- 比较两个值,得到"真"或"假"
- 用 `if`、`else` 让程序做选择
- 用 `&&`、`||`、`!` 组合多个条件
- 用 `switch` 处理多个固定选项

## 1. 比较运算

程序经常要问"这个数大于 60 吗?"。比较的结果只有两种:真(`true`)或假(`false`),类型就是第 2 课学过的 `bool`。

| 符号 | 意思 | 例子 | 结果 |
| --- | --- | --- | --- |
| `==` | 等于 | `3 == 3` | 真 |
| `!=` | 不等于 | `3 != 4` | 真 |
| `>` | 大于 | `3 > 4` | 假 |
| `<` | 小于 | `3 < 4` | 真 |
| `>=` | 大于或等于 | `4 >= 4` | 真 |
| `<=` | 小于或等于 | `5 <= 4` | 假 |

**最常见的错误:把 `==` 写成 `=`。** 一个等号是赋值(把值放进变量),两个等号才是比较。

## 2. if 和 else

`if` 的意思是"如果"。条件为真,就执行大括号里的代码;为假,就跳过。`else` 的意思是"否则",条件为假时执行它。

```cpp
// file: if_else.cpp
#include <iostream>

int main() {
    int score = 75;
    if (score >= 60) {
        std::cout << "及格" << std::endl;
    } else {
        std::cout << "不及格" << std::endl;
    }
    return 0;
}
```

运行结果:

```text
及格
```

条件写在小括号里,要执行的代码写在大括号里。即使大括号里只有一行,也建议写上大括号,以后加代码时不容易出错。

## 3. 多个分支:else if

判断不止两种结果时,用 `else if` 接着往下问。程序从上往下检查,遇到第一个为真的条件就执行它,后面的全部跳过。

```cpp
// file: else_if.cpp
#include <iostream>

int main() {
    int score = 85;
    if (score >= 90) {
        std::cout << "优秀" << std::endl;
    } else if (score >= 80) {
        std::cout << "良好" << std::endl;
    } else if (score >= 60) {
        std::cout << "及格" << std::endl;
    } else {
        std::cout << "不及格" << std::endl;
    }
    return 0;
}
```

运行结果:

```text
良好
```

85 分先不满足 `>= 90`,再检查 `>= 80`,为真,输出"良好"后就结束了,不会再检查后面的条件。所以条件的顺序很重要。

## 4. 组合条件

有时一个条件不够。三个符号可以把条件组合起来:

| 符号 | 读作 | 意思 |
| --- | --- | --- |
| `&&` | 并且 | 两边都为真,结果才为真 |
| `\|\|` | 或者 | 两边只要有一个为真,结果就为真 |
| `!` | 不 | 真变假,假变真 |

```cpp
// file: combine.cpp
#include <iostream>

int main() {
    int age = 20;
    bool has_ticket = true;

    if (age >= 18 && has_ticket) {
        std::cout << "可以进场" << std::endl;
    }
    if (age < 6 || age > 60) {
        std::cout << "有优惠" << std::endl;
    }
    if (!has_ticket) {
        std::cout << "请先买票" << std::endl;
    }
    return 0;
}
```

运行结果:

```text
可以进场
```

20 岁、有票:第一个条件两边都为真,所以输出。第二个条件两边都为假(20 既不小于 6,也不大于 60),不输出。第三个条件里 `has_ticket` 是真,加了 `!` 变成假,不输出。

注意:判断"年龄在 10 到 20 之间"不能写成 `10 <= age <= 20`,要写成 `age >= 10 && age <= 20`。

## 5. switch:多个固定选项

如果是拿一个整数或字符,去和一堆固定的值比较,可以用 `switch`,比一长串 `else if` 清楚。

```cpp
// file: switch_demo.cpp
#include <iostream>

int main() {
    int day = 3;
    switch (day) {
        case 1:
            std::cout << "星期一" << std::endl;
            break;
        case 2:
            std::cout << "星期二" << std::endl;
            break;
        case 3:
            std::cout << "星期三" << std::endl;
            break;
        default:
            std::cout << "其他" << std::endl;
            break;
    }
    return 0;
}
```

运行结果:

```text
星期三
```

- `case 3:` 表示"如果 `day` 等于 3,从这里开始执行"。
- `break;` 表示"到此结束,跳出 `switch`"。**每个 `case` 结尾都要写 `break`。** 忘写的话,程序会接着往下执行下一个 `case` 的代码,这是常见的错误。
- `default:` 表示"以上都不是"时执行,相当于 `else`。

`switch` 只能用于整数和字符,不能用于 `double` 或 `std::string`。

## 练习

1. 输入一个整数,输出它是正数、负数还是零。
2. 输入一个年份,判断是不是闰年。规则:能被 4 整除并且不能被 100 整除,或者能被 400 整除。(提示:用 `%` 和 `&&`、`||`,注意加括号。)
3. 输入 1 到 7,用 `switch` 输出对应的星期。

## 小结

- 比较的结果是真或假。`==` 是比较,`=` 是赋值。
- `if`、`else if`、`else` 按从上到下的顺序检查,执行第一个为真的分支。
- `&&` 并且,`||` 或者,`!` 取反。
- `switch` 里每个 `case` 结尾要写 `break`。
