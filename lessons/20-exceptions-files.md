# 第 20 课:异常与文件读写

## 这一课你会学到

- 异常:程序出错时怎么优雅地处理
- `try`、`catch`、`throw`
- 异常和内存的关系:为什么 RAII 更重要了
- 读写文本文件

## 1. 为什么需要异常

函数遇到问题时怎么办?比如要求"除以 0",或者"打开不存在的文件"。

- 输出一句话然后继续?调用者不知道出了问题。
- 返回特殊值(比如 -1)?调用者可能忘了检查,而且 -1 可能本来就是正常结果。

C++ 的办法是**异常**:函数发现问题,就"抛出"一个错误,正常流程立刻中断;调用者可以在合适的地方"接住"它。

## 2. throw、try、catch

```cpp
// file: exception_basic.cpp
#include <iostream>
#include <stdexcept>

double divide(double a, double b) {
    if (b == 0) {
        throw std::runtime_error("除数不能为 0");
    }
    return a / b;
}

int main() {
    try {
        std::cout << divide(10, 2) << std::endl;
        std::cout << divide(1, 0) << std::endl;
        std::cout << "这一行不会执行" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "出错了:" << e.what() << std::endl;
    }
    std::cout << "程序继续" << std::endl;
    return 0;
}
```

运行结果:

```text
5
出错了:除数不能为 0
程序继续
```

- `throw` 抛出一个错误对象。`std::runtime_error` 是标准库提供的,构造时放入错误说明文字。
- `try { ... }`:把可能出错的代码放在里面。
- `catch (...) { ... }`:接住错误。括号里写要接的错误类型。`const ... &` 是第 10 课学过的只读引用。
- `e.what()` 取出错误说明文字。
- 一旦抛出,`try` 里面剩下的代码全部跳过,直接去匹配的 `catch`。
- 如果没人接住,程序会终止。第 7 课 `vector::at` 越界时的崩溃,就是抛出了没人接的异常。

## 3. 捕获多种异常

标准库里有多种异常类型。可以写多个 `catch`,从上到下找第一个匹配的。

```cpp
// file: exception_multi.cpp
#include <iostream>
#include <stdexcept>
#include <vector>

int main() {
    std::vector<int> v = {1, 2, 3};

    try {
        std::cout << v.at(5) << std::endl;
    } catch (const std::out_of_range& e) {
        std::cout << "越界:" << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "其他错误:" << e.what() << std::endl;
    }
    return 0;
}
```

运行结果:

```text
越界:vector::_M_range_check: __n (which is 5) >= this->size() (which is 3)
```

`std::out_of_range` 是 `v.at()` 越界时抛出的类型。`std::exception` 是所有标准异常的共同基类(第 16 课的概念),所以最后一个 `catch` 能接住所有标准异常。**具体的写在前面,宽泛的写在后面。**

(`what()` 的文字内容取决于编译器,你看到的可能略有不同。)

## 4. 异常和内存:RAII 的价值

异常会让函数在中途直接跳走,后面的代码不执行。如果你手写了 `new` 和 `delete`:

```cpp
void risky() {
    int* p = new int[100];
    do_something();        // 如果这里抛出异常,下一行不会执行
    delete[] p;
}
```

`do_something()` 抛出异常,函数立刻退出,`delete[] p;` 被跳过,内存泄漏。这和第 13 课"提前 `return`"的问题是同一类。

用 RAII(`vector`、智能指针)就没有这个问题。异常让函数退出时,栈上的变量照常销毁,它们的析构函数照常运行:

```cpp
// file: exception_raii.cpp
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

void work() {
    std::vector<int> data(100, 0);
    auto p = std::make_unique<int>(5);
    throw std::runtime_error("出错了");
}

int main() {
    try {
        work();
    } catch (const std::exception& e) {
        std::cout << "捕获:" << e.what() << std::endl;
    }
    return 0;
}
```

运行结果:

```text
捕获:出错了
```

`work` 在中途抛出,但 `data` 和 `p` 都会被正确销毁。用 valgrind 检查,没有泄漏。这是"能用 RAII 就不要手写 `delete`"的又一个理由。

## 5. 异常的使用建议

- 异常用来处理**不应该发生的情况**:文件打不开、参数不合理、数据损坏。不要用它表示正常的流程。
- 要抛出的话,抛出 `std::runtime_error`、`std::invalid_argument` 这类标准异常。
- `catch` 时用 `const` 引用接。
- 只在你**知道怎么处理**的地方 `catch`。不知道怎么办,就让它继续往外传。

## 6. 读写文本文件

文件读写也要加头文件 `#include <fstream>`。

- `std::ofstream`:往文件里**写**(output file stream)。
- `std::ifstream`:从文件里**读**(input file stream)。

用法和 `cout`、`cin` 几乎一样,只是把屏幕换成了文件。

```cpp
// file: file_write.cpp
#include <fstream>
#include <iostream>

int main() {
    std::ofstream out("scores.txt");
    if (!out) {
        std::cout << "文件打不开" << std::endl;
        return 1;
    }

    out << "小明 90" << std::endl;
    out << "小红 85" << std::endl;
    out << "小刚 70" << std::endl;

    std::cout << "已写入 scores.txt" << std::endl;
    return 0;
}
```

运行结果:

```text
已写入 scores.txt
```

程序所在的文件夹里会出现 `scores.txt`。如果文件已存在,默认会被**清空重写**,请小心。

- `std::ofstream out("scores.txt");` 创建对象并打开文件。
- `if (!out)` 检查是否打开成功。
- 文件对象 `out` 销毁时,会自动关闭文件。**这又是 RAII**:资源是"打开的文件",归还就是"关闭",由析构函数自动完成。你不用手动关闭。

## 7. 读文件

```cpp
// file: file_read.cpp
#include <fstream>
#include <iostream>
#include <string>

int main() {
    std::ofstream out("scores2.txt");
    out << "小明 90\n小红 85\n小刚 70\n";
    out.close();

    std::ifstream in("scores2.txt");
    if (!in) {
        std::cout << "文件打不开" << std::endl;
        return 1;
    }

    std::string name;
    int score = 0;
    int total = 0;
    int count = 0;
    while (in >> name >> score) {
        std::cout << name << ":" << score << std::endl;
        total += score;
        count++;
    }
    std::cout << "平均分:" << static_cast<double>(total) / count << std::endl;
    return 0;
}
```

运行结果:

```text
小明:90
小红:85
小刚:70
平均分:81.6667
```

- `while (in >> name >> score)`:和第 5 课读键盘的 `while (std::cin >> x)` 完全一样,读到文件末尾就停。
- 这个例子里,先写文件再读,是为了让示例能独立运行。实际用时,文件通常是已经有的。
- `out.close()` 提前手动关闭,因为紧接着要读它。读一整行用 `std::getline(in, line)`,和第 7 课一样。

## 练习

1. 写函数 `safe_divide`,除数为 0 时抛出 `std::invalid_argument`,在 `main` 里接住并输出提示。
2. 让用户输入 5 个整数,写入 `numbers.txt`,每行一个。
3. 读取 `numbers.txt`,输出最大值。文件不存在时给出友好提示。
4. 读一个文本文件,输出里面有多少行。(提示:`std::getline`。)

## 小结

- `throw` 抛出错误,`try` 包住可能出错的代码,`catch` 接住。
- 具体类型的 `catch` 写在前面,`std::exception` 写在最后。
- 异常会跳过中途的代码,所以更需要 RAII 保证资源归还。
- `ofstream` 写文件,`ifstream` 读文件,用法同 `cout` 和 `cin`。
- 文件对象销毁时自动关闭,这是 RAII 的又一个例子。
