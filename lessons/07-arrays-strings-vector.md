# 第 7 课:数组、字符串与 vector

## 这一课你会学到

- 用数组一次保存很多个同类型的值
- 数组的下标和越界
- 用 `std::string` 处理文字
- 用 `std::vector` 保存数量会变化的一组数据

## 1. 为什么需要数组

要保存 5 个学生的分数,创建 5 个变量太麻烦了,要保存 1000 个就更不可能。**数组**可以把多个同类型的值排成一排,放在一起,统一起一个名字。

```cpp
// file: array_demo.cpp
#include <iostream>

int main() {
    int scores[5] = {90, 85, 70, 60, 100};

    std::cout << "第一个:" << scores[0] << std::endl;
    std::cout << "第三个:" << scores[2] << std::endl;

    scores[1] = 88;
    std::cout << "修改后的第二个:" << scores[1] << std::endl;
    return 0;
}
```

运行结果:

```text
第一个:90
第三个:70
修改后的第二个:88
```

- `int scores[5]` 表示:这是一排,共 5 个 `int`。
- 方括号里的数字叫**下标**,表示第几个。**下标从 0 开始数**,所以第一个是 `scores[0]`,最后一个是 `scores[4]`。
- 数组创建时大小就定死了,以后不能变。

## 2. 用循环处理数组

数组和循环是好搭档。用 `for` 让下标从 0 走到最后一个:

```cpp
// file: array_loop.cpp
#include <iostream>

int main() {
    int scores[5] = {90, 85, 70, 60, 100};
    int sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += scores[i];
    }
    std::cout << "总分:" << sum << std::endl;
    std::cout << "平均分:" << sum / 5.0 << std::endl;
    return 0;
}
```

运行结果:

```text
总分:405
平均分:81
```

注意循环条件是 `i < 5`,不是 `i <= 5`。5 个元素的下标是 0 到 4,没有 `scores[5]`。

## 3. 下标越界

如果访问 `scores[5]` 或 `scores[100]`,这叫**越界**:你在访问数组范围之外的内存,那块内存不属于这个数组。

C++ **不会拦着你**。编译器通常不报错,程序也未必马上崩溃。你可能读到一个莫名其妙的数,也可能不小心改坏了别的变量。这是 C++ 里最典型的危险操作之一,内存课里会详细讲它为什么危险。

现在先记住:**下标必须在 0 到 (元素个数 - 1) 之间。**

## 4. 字符串

第 2 课用过 `std::string` 保存文字。它其实就像一排 `char`,所以也能用下标取出单个字符,下标同样从 0 开始。

```cpp
// file: string_demo.cpp
#include <iostream>
#include <string>

int main() {
    std::string word = "hello";
    std::cout << "长度:" << word.size() << std::endl;
    std::cout << "第一个字符:" << word[0] << std::endl;

    word[0] = 'H';
    std::cout << word << std::endl;

    std::string full = word + ", world";
    std::cout << full << std::endl;
    return 0;
}
```

运行结果:

```text
长度:5
第一个字符:h
Hello
Hello, world
```

- `word.size()` 返回字符个数。写法是"变量名、点、函数名",第 14 课讲类的时候你会明白这个点是什么意思。
- 两个字符串可以用 `+` 拼起来。
- 字符串的大小会自动适应,不用像数组那样提前定死。

## 5. 读取一整行

第 3 课说过,`cin >> ` 读到空格就停。读一整行(包括空格)要用 `std::getline`:

<!-- stdin: Tom Smith\n -->
```cpp
// file: getline_demo.cpp
#include <iostream>
#include <string>

int main() {
    std::string name;
    std::cout << "请输入全名:";
    std::getline(std::cin, name);
    std::cout << "你好," << name << std::endl;
    return 0;
}
```

输入 `Tom Smith`,输出:

```text
请输入全名:你好,Tom Smith
```

这里 `std::string name;` 没有写初始值。字符串是特例:不写初始值,它就是空文字,很安全。

一个小提醒:`size()` 数的是字节,不是汉字个数。英文字母占 1 个字节,一个汉字通常占 3 个字节,所以 `std::string("你好").size()` 是 6。本课程用英文字符做下标练习,不会碰到这个问题。

## 6. vector:会变长的数组

数组的大小定死了,但现实中数据的数量常常会变,比如用户想输入多少个分数都行。这时用 `std::vector`。它像数组一样用下标访问,但可以随时增加或删除元素。用它要加 `#include <vector>`。

```cpp
// file: vector_demo.cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> scores;

    scores.push_back(90);
    scores.push_back(85);
    scores.push_back(70);

    std::cout << "个数:" << scores.size() << std::endl;
    std::cout << "第二个:" << scores[1] << std::endl;

    scores.pop_back();
    std::cout << "删除最后一个后,个数:" << scores.size() << std::endl;
    return 0;
}
```

运行结果:

```text
个数:3
第二个:85
删除最后一个后,个数:2
```

- `std::vector<int>` 表示"装 `int` 的 vector"。尖括号里写元素的类型,装文字就写 `std::vector<std::string>`。
- `push_back(x)` 在末尾添加一个元素。
- `pop_back()` 删除最后一个元素。
- `size()` 返回当前的个数。

也可以在创建时直接给出初始内容:

```cpp
std::vector<int> nums = {1, 2, 3, 4};
```

## 7. 遍历:范围 for

要把每个元素都处理一遍,有一种更简洁的写法,不用管下标:

```cpp
// file: range_for.cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums = {10, 20, 30};
    int sum = 0;
    for (int n : nums) {
        sum += n;
    }
    std::cout << "总和:" << sum << std::endl;
    return 0;
}
```

运行结果:

```text
总和:60
```

`for (int n : nums)` 读作"对于 nums 里的每个 n"。每一轮,`n` 依次是 `nums` 里的一个元素。数组、字符串、vector 都能这样用。

## 8. vector 的越界

`vector` 用方括号访问时,同样不检查越界。想要检查,就用 `.at(下标)`。越界时它会让程序明确地报错退出,而不是悄悄访问错误的内存:

<!-- expect: crash -->
```cpp
// file: vector_at.cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums = {1, 2, 3};
    std::cout << nums.at(1) << std::endl;
    std::cout << nums.at(10) << std::endl;
    return 0;
}
```

运行结果:

```text
2
terminate called after throwing an instance of 'std::out_of_range'
  what():  vector::_M_range_check: __n (which is 10) >= this->size() (which is 3)
```

程序先输出 2,然后在访问 `at(10)` 时被终止。这是一个**有意的演示**。程序崩溃了,但崩得清清楚楚,比悄悄出错好找得多。(报错信息的细节取决于编译器,你看到的文字可能略有不同。)

初学阶段,学习时可以多用 `.at()`,更容易发现自己的越界错误。

## 练习

1. 创建一个有 5 个 `int` 的数组,找出其中最大的数。
2. 让用户输入若干个整数(输入结束用 `Ctrl + D`),存进 `vector`,然后输出它们的平均值。(提示:第 5 课的 `while (std::cin >> x)`,注意没有数据时不能除以 0。)
3. 输入一行英文,统计里面有几个字母 `a`。
4. 把一个 `vector<int>` 里的数倒着输出。

## 小结

- 数组:固定大小,下标从 0 开始,越界不会被拦住。
- `std::string`:文字,可以用下标取字符,用 `+` 拼接,用 `getline` 读一整行。
- `std::vector`:可以变长,`push_back` 添加,`size` 取个数,`.at()` 带越界检查。
- `for (int n : nums)` 遍历每一个元素。
- 以后要保存一组数据时,优先用 `vector`,而不是数组。
