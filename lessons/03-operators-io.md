# 第 3 课:运算与输入输出

## 这一课你会学到

- 加减乘除和取余
- 整数除法的陷阱
- 让程序读取用户从键盘输入的内容
- 类型转换

## 1. 算术运算

C++ 的基本运算符号和数学里差不多:

| 符号 | 意思 | 例子 | 结果 |
| --- | --- | --- | --- |
| `+` | 加 | `7 + 2` | 9 |
| `-` | 减 | `7 - 2` | 5 |
| `*` | 乘 | `7 * 2` | 14 |
| `/` | 除 | `7 / 2` | 3(注意!) |
| `%` | 取余数 | `7 % 2` | 1 |

```cpp
// file: arithmetic.cpp
#include <iostream>

int main() {
    int a = 7;
    int b = 2;
    std::cout << a + b << std::endl;
    std::cout << a - b << std::endl;
    std::cout << a * b << std::endl;
    std::cout << a / b << std::endl;
    std::cout << a % b << std::endl;
    return 0;
}
```

运行结果:

```text
9
5
14
3
1
```

## 2. 整数除法的陷阱

`7 / 2` 的结果是 3,不是 3.5。**两个整数相除,结果还是整数,小数部分直接丢掉。**

想得到 3.5,至少要有一个数是 `double`:

```cpp
// file: division.cpp
#include <iostream>

int main() {
    std::cout << 7 / 2 << std::endl;
    std::cout << 7.0 / 2 << std::endl;
    return 0;
}
```

运行结果:

```text
3
3.5
```

`%` 取余数,只能用在整数上。它很有用,比如判断一个数是不是偶数:`n % 2 == 0` 成立就是偶数。

## 3. 简写形式

给变量加一个数,再存回自己,这种事很常见,所以有简写:

| 简写 | 等同于 |
| --- | --- |
| `x += 3;` | `x = x + 3;` |
| `x -= 3;` | `x = x - 3;` |
| `x *= 3;` | `x = x * 3;` |
| `x /= 3;` | `x = x / 3;` |
| `x++;` | `x = x + 1;` |
| `x--;` | `x = x - 1;` |

```cpp
// file: shorthand.cpp
#include <iostream>

int main() {
    int x = 10;
    x += 5;
    std::cout << x << std::endl;
    x++;
    std::cout << x << std::endl;
    return 0;
}
```

运行结果:

```text
15
16
```

## 4. 读取键盘输入:std::cin

前面的程序只会输出。`std::cin` 让程序能接收用户输入的内容。`cin` 读作"c-in",意思是"输入"。它和 `cout` 的箭头方向相反:

<!-- stdin: 8\n -->
```cpp
// file: input_demo.cpp
#include <iostream>

int main() {
    int age = 0;
    std::cout << "请输入你的年龄:";
    std::cin >> age;
    std::cout << "明年你就是 " << age + 1 << " 岁了" << std::endl;
    return 0;
}
```

运行后,程序会停下来等你输入。输入 `8` 再按回车,得到:

```text
请输入你的年龄:明年你就是 9 岁了
```

读取之前,变量要先创建好并给初始值(第 2 课讲过的好习惯)。`cin >> age` 把你输入的数放进 `age` 里。

可以一次读多个值,用空格或回车隔开:

<!-- stdin: 3 4\n -->
```cpp
// file: input_two.cpp
#include <iostream>

int main() {
    int width = 0;
    int height = 0;
    std::cout << "请输入长和宽:";
    std::cin >> width >> height;
    std::cout << "面积:" << width * height << std::endl;
    return 0;
}
```

输入 `3 4`,输出:

```text
请输入长和宽:面积:12
```

自己运行时,你敲的字会显示在提示后面,所以屏幕上看起来是分行的。上面的输出把输入省略了。

读文字用 `std::string`,但 `cin >> ` 读到空格就停了,所以只能读一个词。读一整行以后会讲到。

## 5. 类型转换

整数和小数混着算时,C++ 会自动把整数变成小数再算,结果是小数:

```cpp
// file: convert.cpp
#include <iostream>

int main() {
    int total = 7;
    int count = 2;
    double average = static_cast<double>(total) / count;
    std::cout << "平均值:" << average << std::endl;
    return 0;
}
```

运行结果:

```text
平均值:3.5
```

`static_cast<double>(total)` 的意思是:把 `total` 当成 `double` 用。这样 `7` 变成 `7.0`,再除以 2,就是 3.5。如果直接写 `total / count`,两个都是整数,结果就是 3(本课第 2 节的陷阱)。

反过来,把小数放进 `int` 变量,小数部分会被直接丢掉(不是四舍五入):

```cpp
int n = 3.9;   // n 的值是 3
```

## 练习

1. 输入一个整数,输出它是偶数还是奇数的余数(`n % 2` 的结果)。
2. 输入两个整数,输出它们的平均值,要求结果带小数。
3. 输入三个数,输出它们的和。

## 小结

- 两个整数相除,结果是整数。要小数,就让其中一个是 `double`。
- `%` 取余数,只用于整数。
- `cout <<` 输出,`cin >>` 输入,箭头方向像数据流动的方向。
- 用 `static_cast<double>(x)` 把值当作小数来算。
