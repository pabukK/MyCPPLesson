# 第 32 课:链表、栈、队列

## 这一课你会学到

- 链表:一个节点连着一个节点,和数组有什么不同
- 自己写一个单向链表,并且不泄漏内存
- 栈:后进先出,用在哪里
- 队列:先进先出,用在哪里
- 标准库里对应的容器:`std::stack`、`std::queue`、`std::deque`、`std::list`

## 1. 数组的一个不方便

第 18 课说过,`vector` 在中间插入、删除,要把后面的元素全部挪一遍,`O(n)`。

**链表**换了一种存放数据的方式,让"插入、删除"不用挪动别的元素。

## 2. 链表是什么

数组把元素紧挨着放在一起(第 8 课)。链表则让每个元素**单独放在一个"节点"里**,节点可以在内存里的任何位置。每个节点除了存数据,还存**下一个节点的地址**(第 9 课:指针就是装地址的变量)。

```text
头指针
  |
  v
[数据 3 | 指向 -> ] --> [数据 7 | 指向 -> ] --> [数据 9 | 指向 -> ] --> 空(nullptr)
```

- 从头指针出发,顺着"下一个"一个个走,就能访问所有元素。
- 最后一个节点的"下一个"是 `nullptr`,表示结束。

在节点之间插入一个新节点,只需要改两个指针,不用挪别的元素:

```text
要在 7 后面插入 8:
[3] --> [7] --> [9]
        ^ 让 8 指向 9,再让 7 指向 8
[3] --> [7] --> [8] --> [9]
```

## 3. 代价

| | 数组 / `vector` | 链表 |
| --- | --- | --- |
| 按下标取第 i 个 | `O(1)` | `O(n)`,要从头一个个走 |
| 在**已知位置**插入、删除 | `O(n)`,要挪动 | `O(1)`,改指针 |
| 在末尾添加 | `O(1)`(平均) | 看有没有记录尾部 |
| 查找某个值 | `O(n)` | `O(n)` |
| 内存 | 连续,CPU 访问快 | 分散,每个节点还多存一个地址 |

**实际情况比表格上更偏向 `vector`:** 因为现代电脑访问连续内存的速度远快于访问分散的内存,很多时候,在中间插入的 `vector`,实测也比链表快。所以工程里**默认还是用 `vector`**,只有确实需要"在已知位置频繁插入删除,且不需要下标访问"时,才考虑链表。

学链表的主要价值:它是**很多别的数据结构的基础**(树、图、哈希表的桶),并且是练习指针和内存管理的最好场合。

## 4. 自己写一个单向链表

用 `unique_ptr` 管理节点(第 13 课)。每个节点"拥有"它的下一个节点,链表头拥有第一个节点。这样整条链都有明确的主人,不需要手写 `delete`。

```cpp
// file: linked_list.cpp
#include <iostream>
#include <memory>

class LinkedList {
public:
    LinkedList() = default;
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    ~LinkedList() {
        while (head != nullptr) {
            head = std::move(head->next);
        }
    }

    void push_front(int value) {
        auto node = std::make_unique<Node>(value);
        node->next = std::move(head);
        head = std::move(node);
        count++;
    }

    bool remove_first(int value) {
        Node* prev = nullptr;
        Node* cur = head.get();
        while (cur != nullptr) {
            if (cur->value == value) {
                if (prev == nullptr) {
                    head = std::move(cur->next);
                } else {
                    prev->next = std::move(cur->next);
                }
                count--;
                return true;
            }
            prev = cur;
            cur = cur->next.get();
        }
        return false;
    }

    bool contains(int value) const {
        for (Node* cur = head.get(); cur != nullptr; cur = cur->next.get()) {
            if (cur->value == value) {
                return true;
            }
        }
        return false;
    }

    int size() const { return count; }

    void print() const {
        for (Node* cur = head.get(); cur != nullptr; cur = cur->next.get()) {
            std::cout << cur->value << " ";
        }
        std::cout << std::endl;
    }

private:
    struct Node {
        int value;
        std::unique_ptr<Node> next;
        Node(int v) : value(v) {}
    };

    std::unique_ptr<Node> head;
    int count = 0;
};

int main() {
    LinkedList list;
    list.push_front(3);
    list.push_front(2);
    list.push_front(1);
    list.print();

    list.remove_first(2);
    list.print();
    std::cout << "有 3 吗:" << list.contains(3) << ",个数:" << list.size() << std::endl;
    std::cout << "删除不存在的 9:" << list.remove_first(9) << std::endl;
    return 0;
}
```

运行结果:

```text
1 2 3 
1 3 
有 3 吗:1,个数:2
删除不存在的 9:0
```

逐处看:

- `struct Node` 放在 `private` 里,是链表自己的内部细节,外面不需要知道。
- `std::unique_ptr<Node> next`:节点里的"下一个",是一个智能指针。拥有关系是:`head` 拥有第一个节点,第一个节点拥有第二个……
- `head.get()` 取出**普通指针**(不转移所有权)。遍历时我们只是"看一眼",用普通指针 `Node*` 就行。第 13 课说过:只"借用"而不拥有时,用普通指针。
- `push_front`:新节点的 `next` 接管原来的整条链(`std::move(head)`),然后新节点成为 `head`。`std::move` 在第 13 课、第 15 课用过:转让所有权。
- `remove_first`:找到目标后,把它的 `next` 整条链"搬"给前一个节点(或 `head`)。**这一步会让目标节点失去主人,自动销毁**,不用手写 `delete`。

## 5. 一个必须知道的坑:链太长,销毁时会崩溃

上面的 `LinkedList` 里,有一个看起来多余的析构函数:

```cpp
~LinkedList() {
    while (head != nullptr) {
        head = std::move(head->next);
    }
}
```

它为什么存在?我先试了**不写**它。结果是:链表有 1 千、1 万个节点时没问题;**10 万个节点时,程序崩溃了**(无优化编译,栈大小限制 8 MB)。

原因:不写析构函数,销毁链表时,编译器自动生成的版本这样工作:销毁 `head` → `head` 拥有的第一个节点被销毁 → 它的 `next` 要先被销毁 → 第二个节点被销毁 → ……一层套一层,**是一个深度等于链表长度的递归**。第 26 课说过:递归太深,栈会溢出。

修复办法,就是自己写析构函数,用**循环**一个一个地释放:`head = std::move(head->next);` 这一句,让原来的 `head` 节点失去主人而销毁,同时 `head` 指向下一个。此时被销毁的节点,它的 `next` 已经被搬走了(是空的),所以销毁它不会再引发递归。

写了这个析构函数后,我测了 500 万个节点,没有崩溃。并且:

- 用 AddressSanitizer(第 12 课)运行,没有报错。
- 用 valgrind 检查,`All heap blocks were freed -- no leaks are possible`,没有泄漏。

另外还加了两行:

```cpp
LinkedList(const LinkedList&) = delete;
LinkedList& operator=(const LinkedList&) = delete;
```

`= delete` 的意思是"明确禁止"。链表里有 `unique_ptr`,本来就不能复制(第 13 课),编译器会自动禁止,所以这两行不是必须的。写出来,是为了让读代码的人一眼看到:**这个链表不允许复制**。

这个例子,把前面学的好几件事串在了一起:智能指针、`std::move`、析构函数、递归与栈、内存检查工具。

下面是**去掉析构函数**的版本。它建一个有 20 万个节点的链表,然后销毁,会崩溃(这是有意的演示):

<!-- expect: crash -->
```cpp
// file: list_no_dtor.cpp
#include <iostream>
#include <memory>

struct Node {
    int value;
    std::unique_ptr<Node> next;
    Node(int v) : value(v) {}
};

int main() {
    std::unique_ptr<Node> head;
    for (int i = 0; i < 200000; i++) {
        auto node = std::make_unique<Node>(i);
        node->next = std::move(head);
        head = std::move(node);
    }
    std::cout << "建好了" << std::endl;
    return 0;
}
```

输出"建好了"之后,`main` 结束,`head` 被销毁,引发深度 20 万的递归析构,栈溢出,程序崩溃。开了优化(`-O2`)有时候不会崩,但不要依赖它。

## 6. 双向链表和标准库的 `std::list`

我们写的是**单向链表**:每个节点只知道下一个。如果每个节点还记着**上一个**,就是**双向链表**,可以从两个方向走,也能在已知位置 `O(1)` 删除自己,不用先找前一个。

标准库已经提供了双向链表 `std::list`(头文件 `<list>`):

```cpp
// file: std_list.cpp
#include <iostream>
#include <list>

int main() {
    std::list<int> l = {1, 2, 3};
    l.push_front(0);
    l.push_back(4);

    auto it = l.begin();
    ++it;
    ++it;
    l.insert(it, 99);

    for (int x : l) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    l.remove(3);
    for (int x : l) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

运行结果:

```text
0 1 99 2 3 4 
0 1 99 2 4 
```

- `l.begin()` 是第一个元素的位置;`++it` 让位置前进一格。不能写 `it + 2`,因为链表不支持"跳着走",只能一个个走,这正是链表"不能按下标访问"的体现。
- `insert(位置, 值)` 在该位置**前面**插入。
- `remove(值)` 删除所有等于该值的元素。

**还是那句话:需要链表的场合很少,默认用 `vector`。** 但你现在知道它的原理了。

## 7. 栈:后进先出

第 17 课已经写过一个简易的栈,当时解释过:后放进去的,先拿出来,像叠盘子。

**栈**只允许在**一端**(栈顶)操作:

- `push`:放到栈顶。
- `pop`:拿走栈顶。
- `top`:看一眼栈顶是什么,不拿走。

标准库里有现成的 `std::stack`(头文件 `<stack>`):

```cpp
// file: std_stack.cpp
#include <iostream>
#include <stack>

int main() {
    std::stack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);

    std::cout << "栈顶:" << s.top() << std::endl;
    s.pop();
    std::cout << "弹出后栈顶:" << s.top() << std::endl;
    std::cout << "大小:" << s.size() << std::endl;

    while (!s.empty()) {
        std::cout << s.top() << " ";
        s.pop();
    }
    std::cout << std::endl;
    return 0;
}
```

运行结果:

```text
栈顶:3
弹出后栈顶:2
大小:2
2 1 
```

注意:`std::stack` 的 `pop()` **不返回**被弹出的值,只是丢掉。要先用 `top()` 取,再 `pop()`。**对空栈调用 `top()` 或 `pop()`,是未定义行为(第 12 课的那类危险),先检查 `empty()`。**

### 栈的经典用途:括号匹配

**问题:** 判断一个字符串里的括号是否正确配对,比如 `"([]{})"` 对,`"([)]"` 错,`"(("` 错。

想法:遇到左括号,压栈;遇到右括号,看栈顶是不是对应的左括号。是就弹出,不是就错。最后栈必须是空的。

最近的左括号最先被匹配,正好符合"后进先出"。

```cpp
// file: brackets.cpp
#include <iostream>
#include <stack>
#include <string>

bool is_balanced(const std::string& s) {
    std::stack<char> st;
    for (char c : s) {
        if (c == '(' || c == '[' || c == '{') {
            st.push(c);
        } else if (c == ')' || c == ']' || c == '}') {
            if (st.empty()) {
                return false;
            }
            char open = st.top();
            st.pop();
            if ((c == ')' && open != '(') || (c == ']' && open != '[') || (c == '}' && open != '{')) {
                return false;
            }
        }
    }
    return st.empty();
}

int main() {
    for (std::string s : {"([]{})", "([)]", "((", "", "a(b)c", ")("}) {
        std::cout << "\"" << s << "\" -> " << (is_balanced(s) ? "对" : "错") << std::endl;
    }
    return 0;
}
```

运行结果:

```text
"([]{})" -> 对
"([)]" -> 错
"((" -> 错
"" -> 对
"a(b)c" -> 对
")(" -> 错
```

几个值得注意的边界(第 23 课):

- 右括号来了但栈是空的(`")("`):必须先检查 `st.empty()`,否则对空栈 `top()` 就出事了。
- 全部扫完,栈里还有剩的(`"(("`):用 `return st.empty()` 判断。
- 空字符串:没有括号,当然是配对的。

**栈还用在:** 函数调用(第 26 课递归用的就是栈)、撤销(Ctrl+Z)、浏览器的"后退"、表达式求值、深度优先搜索(第 35 课)。

## 8. 队列:先进先出

和栈相反,**队列**像排队买票:先来的先服务。

- 从**队尾**加入(`push`)。
- 从**队首**离开(`pop`)。
- `front`:看队首;`back`:看队尾。

标准库:`std::queue`(头文件 `<queue>`)。

```cpp
// file: std_queue.cpp
#include <iostream>
#include <queue>
#include <string>

int main() {
    std::queue<std::string> q;
    q.push("Tom");
    q.push("Ann");
    q.push("Bob");

    std::cout << "队首:" << q.front() << ",队尾:" << q.back() << std::endl;
    std::cout << "叫号:" << q.front() << std::endl;
    q.pop();
    std::cout << "叫号:" << q.front() << std::endl;
    q.pop();
    std::cout << "还剩 " << q.size() << " 人" << std::endl;
    return 0;
}
```

运行结果:

```text
队首:Tom,队尾:Bob
叫号:Tom
叫号:Ann
还剩 1 人
```

和栈一样:`pop()` 不返回值;对空队列调用 `front()`、`pop()` 是危险的,要先检查 `empty()`。

**队列用在:** 任务排队、消息传递、打印队列、**广度优先搜索**(第 35 课)。

### 一个小例子:约瑟夫问题

n 个人围成一圈,从 1 开始报数,报到 k 的人出局,然后从下一个人重新从 1 数起,直到剩最后一个。问最后留下的是谁?

用队列模拟:每个人依次出队,报数;没报到 k 的,重新排到队尾;报到 k 的,不再入队。

```cpp
// file: josephus.cpp
#include <iostream>
#include <queue>

int josephus(int n, int k) {
    std::queue<int> q;
    for (int i = 1; i <= n; i++) {
        q.push(i);
    }
    while (q.size() > 1) {
        for (int step = 1; step < k; step++) {
            q.push(q.front());
            q.pop();
        }
        q.pop();
    }
    return q.front();
}

int main() {
    std::cout << josephus(7, 3) << std::endl;
    std::cout << josephus(1, 5) << std::endl;
    std::cout << josephus(10, 2) << std::endl;
    return 0;
}
```

运行结果:

```text
4
1
5
```

7 个人,报到 3 出局:出局顺序是 3、6、2、7、5、1,最后剩 4。

## 9. 双端队列 deque

如果要在**两头**都能高效地加入、删除,用 `std::deque`(读作"deck",头文件 `<deque>`)。它既有 `push_front`、`pop_front`,也有 `push_back`、`pop_back`,还支持下标访问 `d[i]`。

```cpp
// file: std_deque.cpp
#include <deque>
#include <iostream>

int main() {
    std::deque<int> d;
    d.push_back(2);
    d.push_back(3);
    d.push_front(1);
    d.push_front(0);

    for (int x : d) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    d.pop_front();
    d.pop_back();
    std::cout << "d[0] = " << d[0] << ",大小 " << d.size() << std::endl;
    return 0;
}
```

运行结果:

```text
0 1 2 3 
d[0] = 1,大小 2
```

`std::stack` 和 `std::queue` 默认就是用 `deque` 在底层实现的。

## 10. 怎么选

| 需求 | 选 |
| --- | --- |
| 一排数据,按下标访问,主要在末尾操作 | `vector` |
| 后进先出 | `stack` |
| 先进先出 | `queue` |
| 两头都要增删 | `deque` |
| 在已知位置频繁插入删除,不要下标 | `list`(很少用) |

## 练习

1. 给 `LinkedList` 加一个 `reverse()` 成员函数,把链表反转(1→2→3 变成 3→2→1)。提示:用 `prev`、`cur`、`next` 三个普通指针,一个节点一个节点地改指向。
2. 给 `LinkedList` 加 `push_back`。先写一个每次从头走到尾的版本,再想想怎么优化(提示:记录尾部)。
3. 用栈写一个"后缀表达式求值"函数:比如 `"3 4 + 2 *"` 的结果是 `(3 + 4) * 2 = 14`。规则:遇到数压栈,遇到运算符弹出两个数计算,结果压回栈。
4. 用两个栈实现一个队列(只用 `push`、`pop`、`top`、`empty`)。提示:一个栈负责入,一个栈负责出。
5. 用 `std::stack<std::string>` 模拟浏览器的"后退"功能:访问网址时压栈,点后退时弹出,输出当前网址。
6. 对拍:随机生成操作序列(push、pop),分别用你写的链表栈和 `std::stack` 执行,比较结果。

## 小结

- 链表:节点分散存放,每个节点记着下一个的地址。已知位置插入删除 `O(1)`,按下标访问 `O(n)`。
- 工程里默认用 `vector`,链表很少用,但它是理解树、图的基础。
- 用 `unique_ptr` 写链表,**一定要写循环释放的析构函数**,否则链太长销毁时会栈溢出。实测 10 万节点就会崩。
- 栈:后进先出。`std::stack`。用于括号匹配、撤销、函数调用、深度优先搜索。
- 队列:先进先出。`std::queue`。用于任务排队、广度优先搜索。
- `deque`:两头都能高效增删。
- `pop()` 不返回值;对空的栈、队列取 `top`/`front`/`pop` 是危险的,先检查 `empty()`。
