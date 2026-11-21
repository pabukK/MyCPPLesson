# 第 12 课:常见内存错误与查错工具

## 这一课你会学到

- 四种最常见的内存错误
- 它们为什么危险
- 用 AddressSanitizer 和 valgrind 找出这些错误

> 这一课的代码都是**故意写错**的,目的是让你认识错误长什么样。
> 平时写程序不要这样写。

## 1. 为什么需要工具

内存错误最麻烦的地方是:**程序可能看起来一切正常**。
错误悄悄发生,过了很久才崩溃,而且崩溃的位置往往和出错的位置毫无关系。
靠眼睛检查很难发现,所以要用工具。

我们用两个工具:

| 工具 | 怎么用 | 特点 |
| --- | --- | --- |
| AddressSanitizer | 编译时加 `-fsanitize=address -g` | 速度快,直接指出出错的行号 |
| valgrind | `valgrind ./程序` | 不用改编译方式,能详细报告泄漏 |

`-g` 的意思是让程序带上行号等信息,报错时才能告诉你是哪一行。

## 2. 错误一:内存泄漏

第 11 课学过:`new` 了不 `delete`,就是泄漏。

```cpp
// file: leak_demo.cpp
#include <iostream>

int main() {
    int* p = new int;
    *p = 1;
    std::cout << *p << std::endl;
    return 0;
}
```

用 valgrind 检查。先正常编译(加 `-g`),再运行:

```bash
g++ -std=c++17 -g -o leak_demo leak_demo.cpp
valgrind --leak-check=full ./leak_demo
```

报告里有这几行:

```text
HEAP SUMMARY:
    in use at exit: 4 bytes in 1 blocks
4 bytes in 1 blocks are definitely lost in loss record 1 of 1
   at 0x4846FA3: operator new(unsigned long)
   by 0x10919E: main (leak_demo.cpp:5)
```

意思是:程序结束时,还有 4 字节(正好是一个 `int`)没有归还,是在 `leak_demo.cpp` 第 5 行申请的。

`definitely lost` 的意思是"确定丢失了",这是你要重点关注的一项。

## 3. 错误二:重复归还(double free)

同一块内存 `delete` 了两次。第一次 `delete` 以后,这块内存已经还给系统了,可能马上就被分给别的地方。第二次 `delete`,等于把别人正在用的东西拆掉。

<!-- expect: crash
flags: -fsanitize=address -g -->
```cpp
// file: double_free.cpp
int main() {
    int* p = new int(5);
    delete p;
    delete p;
    return 0;
}
```

(`new int(5)` 是 `new` 的简写:申请内存,同时把初始值设为 5。)

加上 `-fsanitize=address -g` 编译运行,AddressSanitizer 会报:

```text
ERROR: AddressSanitizer: attempting double-free on 0x502000000010
    #1 main double_free.cpp:5
freed by thread T0 here:
    #1 main double_free.cpp:4
```

它告诉你两件事:第二次归还发生在第 5 行,第一次归还在第 4 行。(行号按文件里包含第一行 `// file:` 注释来数。)

**预防办法:** `delete p;` 之后马上写 `p = nullptr;`。对空指针执行 `delete` 是安全的,什么也不会发生。

## 4. 错误三:归还后继续使用(use after free)

`delete` 之后,这块内存已经不属于你了,但指针里还留着地址。如果继续通过它读写,你就是在碰别人的东西。这样的指针叫**悬空指针**(第 10 课提过这个词)。

<!-- expect: crash
flags: -fsanitize=address -g -->
```cpp
// file: use_after_free.cpp
#include <iostream>

int main() {
    int* p = new int(5);
    delete p;
    std::cout << *p << std::endl;
    return 0;
}
```

AddressSanitizer 会报:

```text
ERROR: AddressSanitizer: heap-use-after-free
READ of size 4
    #0 main use_after_free.cpp:7
freed by thread T0 here:
    #1 main use_after_free.cpp:6
```

不用工具时,这个程序**可能还会输出 5**,因为那块内存里的旧数据还没被覆盖。这正是它危险的地方:有时看起来正常,有时崩溃,有时读到莫名其妙的数。

**预防办法:** 同样是 `delete` 之后设为 `nullptr`。这样再误用,程序会立刻崩溃,而不是悄悄出错。立刻崩溃比悄悄出错好找得多。

## 5. 错误四:越界(buffer overflow)

第 7 课和第 9 课都提到过:访问数组范围之外的位置。在堆上也一样。

<!-- expect: crash
flags: -fsanitize=address -g -->
```cpp
// file: overflow_heap.cpp
int main() {
    int* a = new int[3];
    a[3] = 7;
    delete[] a;
    return 0;
}
```

`new int[3]` 只有下标 0、1、2,`a[3]` 越界了。AddressSanitizer 会报:

```text
ERROR: AddressSanitizer: heap-buffer-overflow
WRITE of size 4
    #0 main overflow_heap.cpp:4
0x50200000001c is located 0 bytes after 12-byte region
```

`0 bytes after 12-byte region` 的意思:你写的位置,紧贴在那块 12 字节(3 个 `int`)的后面。

**预防办法:** 优先用 `std::vector`,配合 `.at()`。

## 6. 两个工具怎么选

- 平时开发:编译时加上 `-fsanitize=address -g`,运行就能发现越界、重复归还、归还后使用。速度只慢一点点。
- 查泄漏:用 `valgrind --leak-check=full ./程序`。
- 两个工具不要同时使用。选一个,跑一次,看结果。

建议你从现在开始养成习惯:**写完涉及内存的代码,先用工具跑一遍再说。**

## 7. 为什么这些错误难找

| 错误 | 典型表现 |
| --- | --- |
| 泄漏 | 程序一切正常,但占用的内存越来越多 |
| 重复归还 | 有时立刻崩溃,有时过一会儿才崩溃 |
| 归还后使用 | 有时读到旧数据"看起来对",有时读到乱码 |
| 越界 | 悄悄改坏旁边的数据,几行之后才出问题 |

共同点:**出错的位置和表现出来的位置不在一起。** 这就是工具的价值。

## 练习

1. 自己写一个泄漏的程序,用 valgrind 找出它在第几行申请的内存。
2. 把本课的 `double_free.cpp` 改对:在第一次 `delete` 后加上 `p = nullptr;`,再用 AddressSanitizer 运行,确认不再报错。
3. 写一个数组越界的程序,用 AddressSanitizer 运行,看看报告怎么描述它。

## 小结

- 四种常见错误:泄漏、重复归还、归还后使用、越界。
- 它们不一定立刻出问题,所以要用工具查。
- 编译时加 `-fsanitize=address -g`,可以查后三种。
- `valgrind --leak-check=full` 可以查泄漏。
- `delete` 之后立刻把指针设为 `nullptr`,能减少一部分错误。
- 下一课会学到一种办法,让这些错误大部分根本不会发生。
