# 第 13 课:智能指针,让内存自动归还

## 这一课你会学到

- 为什么手写 `delete` 容易出错
- `std::unique_ptr`:一块内存只有一个主人
- `std::shared_ptr`:一块内存可以有多个主人
- 现代 C++ 的做法:几乎不再手写 `new` 和 `delete`

## 1. 回顾:问题出在哪里

第 11 课说过,栈上的变量有一个好处:出了大括号,自动销毁,内存自动归还,你什么都不用做。

堆上的内存要手动 `delete`,一不小心就会漏掉。问题是:能不能让堆上的内存也享受"出大括号自动归还"的待遇?

可以。办法是:**把堆上的地址,交给一个栈上的变量保管。** 这个栈上的变量出大括号时会自动销毁,销毁的同时,它顺手把堆内存归还。

这种"保管地址、销毁时顺手归还"的变量,就叫**智能指针**。它是 C++ 标准库提供的,你不用自己写。

## 2. unique_ptr:唯一的主人

`std::unique_ptr` 表示:这块堆内存**只有一个主人**。主人消失了,内存就自动归还。用它要加 `#include <memory>`。

```cpp
// file: unique_basic.cpp
#include <iostream>
#include <memory>

int main() {
    std::unique_ptr<int> p = std::make_unique<int>(42);
    std::cout << "值:" << *p << std::endl;

    *p = 100;
    std::cout << "改后:" << *p << std::endl;
    return 0;
}
```

运行结果:

```text
值:42
改后:100
```

注意:这段代码里**没有 `delete`**。`p` 是栈上的变量,`main` 结束时它销毁,堆内存跟着归还。

## 3. 怎么读这行代码

```cpp
std::unique_ptr<int> p = std::make_unique<int>(42);
```

- `std::unique_ptr<int>`:一个"唯一主人"的智能指针,管理的是一个 `int`。尖括号里写被管理的类型,和 `vector<int>` 的写法一样。
- `std::make_unique<int>(42)`:在堆上创建一个 `int`,初始值 42,并交给智能指针。它替你完成了 `new`。
- 使用时,和普通指针一样:`*p` 取值,`*p = 100` 修改。

**以后需要在堆上放东西,优先用 `make_unique`,不要自己写 `new`。**

## 4. 唯一的主人:不能复制

"唯一"的意思是:同一块内存,只能有一个 `unique_ptr` 管着。如果允许复制,两个主人都会在销毁时归还,就变成了第 12 课的"重复归还"。所以编译器直接禁止复制:

<!-- expect: compile-fail -->
```cpp
// file: unique_copy.cpp
#include <memory>

int main() {
    std::unique_ptr<int> a = std::make_unique<int>(1);
    std::unique_ptr<int> b = a;
    return 0;
}
```

编译会报错,大意是"不能复制 unique_ptr"。错误在编译时就被拦住了,比运行时崩溃好得多。

## 5. 转让主人:std::move

如果确实想换一个主人,可以用 `std::move` 把所有权**转让**过去。转让之后,原来的指针变成空的(第 9 课学过的 `nullptr`)。

```cpp
// file: unique_move.cpp
#include <iostream>
#include <memory>

int main() {
    std::unique_ptr<int> a = std::make_unique<int>(7);
    std::unique_ptr<int> b = std::move(a);

    if (a == nullptr) {
        std::cout << "a 现在是空的" << std::endl;
    }
    std::cout << "b 管着:" << *b << std::endl;
    return 0;
}
```

运行结果:

```text
a 现在是空的
b 管着:7
```

就像把仓库钥匙交给别人:交出去以后,你手里就没有钥匙了。`std::move` 的细节在第 15 课会再讲,现在只需要知道:它用来转让所有权。

## 6. shared_ptr:多个主人共同拥有

有时一块数据确实需要被多处同时使用,谁都不是唯一的主人。这时用 `std::shared_ptr`:它在内部记着**有几个主人**,每多一个主人,计数加 1;一个主人消失,计数减 1;计数变成 0,说明没人要了,才归还内存。

```cpp
// file: shared_demo.cpp
#include <iostream>
#include <memory>

int main() {
    std::shared_ptr<int> a = std::make_shared<int>(10);
    std::cout << "主人数:" << a.use_count() << std::endl;

    {
        std::shared_ptr<int> b = a;
        std::cout << "主人数:" << a.use_count() << std::endl;
    }

    std::cout << "主人数:" << a.use_count() << std::endl;
    return 0;
}
```

运行结果:

```text
主人数:1
主人数:2
主人数:1
```

`b = a` 是复制,允许,主人数变 2。内层大括号结束,`b` 销毁,主人数回到 1。`main` 结束时 `a` 销毁,计数变 0,内存归还。

## 7. 怎么选

| 情况 | 选择 |
| --- | --- |
| 只有一个地方负责这块内存 | `unique_ptr`(默认选它) |
| 确实有多处共同拥有,说不清谁最后用完 | `shared_ptr` |

`unique_ptr` 更轻,也更清楚地表达"谁负责归还"。**默认用 `unique_ptr`,确实需要共享时才换 `shared_ptr`。**

## 8. 为什么说它"自动"

对比第 11 课的一个麻烦:函数提前 `return`,会跳过后面的 `delete`。

```cpp
// file: early_return.cpp
#include <iostream>
#include <memory>

bool process(int n) {
    std::unique_ptr<int> data = std::make_unique<int>(n);
    if (*data < 0) {
        return false;
    }
    std::cout << "处理:" << *data << std::endl;
    return true;
}

int main() {
    process(5);
    process(-1);
    return 0;
}
```

运行结果:

```text
处理:5
```

`process(-1)` 在中途就 `return false` 了。如果用 `new` 和 `delete`,这条路径上的 `delete` 被跳过,就泄漏了。但 `data` 是栈上的变量,**无论从哪条路径离开函数,它都会销毁**,内存都会归还。

可以用第 12 课的 valgrind 验证一下:

```bash
g++ -std=c++17 -g -o early_return early_return.cpp
valgrind --leak-check=full ./early_return
```

报告中会看到:

```text
All heap blocks were freed -- no leaks are possible
```

## 9. RAII:这个思路的名字

上面用到的思路,有一个专门的名字,叫 **RAII**。名字很拗口,不用背,只记意思:

> **资源的申请和归还,跟着一个栈上变量的出生和销毁走。**

这里的"资源"不只是内存。打开的文件、网络连接、加了的锁,都是用完必须归还的东西。第 20 课读写文件时,你会再次看到同样的思路。

`vector` 和 `string` 其实也是这样:它们在堆上放数据,销毁时自动归还。所以你前面用 `vector` 时,从来没写过 `delete`,也不会泄漏。

## 10. 使用规则

1. 需要在堆上放单个对象:用 `std::make_unique`。
2. 需要在堆上放一排数据:用 `std::vector`,不要用 `new int[n]`。
3. 确实要共享所有权:用 `std::make_shared`。
4. 不要把同一个 `new` 出来的地址,交给两个智能指针。
5. 仍然需要 `new` 和 `delete` 的场合很少,主要是读别人写的旧代码。读得懂就够了。

只是"借用"而不拥有时,函数参数用引用或者普通指针即可,不用传智能指针。例如:

```cpp
void print_value(const int& x);       // 只看一眼,不拥有
print_value(*p);                       // p 是 unique_ptr<int>
```

## 11. 提醒:shared_ptr 的一个陷阱

如果 A 通过 `shared_ptr` 拥有 B,B 又通过 `shared_ptr` 拥有 A,两个的计数永远不会变成 0,就永远不归还,这是一种泄漏。这叫**循环引用**。解决办法是用 `std::weak_ptr`(只观察,不算主人),但初学阶段很少碰到,知道有这回事就好。需要时再查资料。

## 练习

1. 用 `make_unique` 创建一个 `double`,赋值 3.14,输出。不写 `delete`,用 valgrind 确认没有泄漏。
2. 创建一个 `unique_ptr<int>`,用 `std::move` 把它交给一个函数 `void take(std::unique_ptr<int> p)`,在函数里输出值。思考:函数返回后,内存什么时候归还?
3. 创建一个 `shared_ptr<int>`,放进两个不同的大括号里各复制一份,每一步输出 `use_count()`。

## 小结

- 智能指针是栈上的变量,销毁时自动归还堆内存。
- `unique_ptr`:唯一主人,不能复制,可以用 `std::move` 转让。
- `shared_ptr`:多个主人,有计数,变成 0 时归还。
- `make_unique`、`make_shared` 代替手写 `new`。
- RAII:资源的申请和归还,跟着栈上变量的出生和销毁走。
- 整套内存课的结论:**能用 `vector`、`string`、智能指针,就不要自己 `new` 和 `delete`。**
