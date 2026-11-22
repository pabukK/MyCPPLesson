# 第 34 课:优先队列与堆

## 这一课你会学到

- 优先队列是什么,和普通队列有什么不同
- 堆:一种用数组表示的二叉树
- `std::priority_queue` 的用法
- 堆的几个经典用途:取前 k 大、合并有序序列、调度

## 1. 优先队列

第 32 课的队列,先来先服务。但现实里常常要**按重要性**服务:急诊室里重伤的先看,不管谁先到。

**优先队列**:每次取出**最大**(或最小)的那个元素,而不是最早放进去的。

| 操作 | 普通队列 | 优先队列 |
| --- | --- | --- |
| 放入 | `push` | `push` |
| 取出 | 最早放进去的 | **优先级最高的** |
| 复杂度 | `O(1)` | `O(log n)` |

标准库:`std::priority_queue`,头文件 `<queue>`。

```cpp
// file: priority_queue_basic.cpp
#include <iostream>
#include <queue>
#include <vector>

int main() {
    std::priority_queue<int> pq;
    for (int x : {3, 1, 9, 2, 7}) {
        pq.push(x);
    }

    std::cout << "最大:" << pq.top() << std::endl;
    while (!pq.empty()) {
        std::cout << pq.top() << " ";
        pq.pop();
    }
    std::cout << std::endl;

    std::priority_queue<int, std::vector<int>, std::greater<int>> min_pq;
    for (int x : {3, 1, 9, 2, 7}) {
        min_pq.push(x);
    }
    while (!min_pq.empty()) {
        std::cout << min_pq.top() << " ";
        min_pq.pop();
    }
    std::cout << std::endl;
    return 0;
}
```

运行结果:

```text
最大:9
9 7 3 2 1 
1 2 3 7 9 
```

- `std::priority_queue<int>` 默认取出**最大**的,叫**大顶堆**。
- 想要取出**最小**的,尖括号里要写三个类型:`std::priority_queue<int, std::vector<int>, std::greater<int>>`。中间的 `std::vector<int>` 是"用什么容器存",最后的 `std::greater<int>` 是"怎么比大小"。这个写法长,但它是固定搭配,记住就行。
- `top()` 只看,`pop()` 才移除。和第 32 课的栈、队列一样,`pop()` 不返回值。
- 空的时候调用 `top()`、`pop()` 是危险的,先检查 `empty()`。

`std::greater<int>` 是标准库提供的"比较函数对象",和第 31 课自己写的 `PointHash` 是同类东西。

## 2. 堆:优先队列是怎么做到的

**堆**是一棵满足两条规则的树:

1. **形状规则**:除了最后一层,每层都是满的;最后一层的节点尽量靠左。叫**完全二叉树**。
2. **大小规则**:每个节点的值,都**不小于**它的两个孩子(大顶堆)。所以**最大的值一定在根**。

```text
        9                大顶堆:每个节点 >= 两个孩子
      /   \
     7     2
    / \   /
   3   1 9
```

注意:堆**只保证"父比子大",不保证左右有序**。所以它不是二叉搜索树(第 33 课),中序遍历堆得不到有序序列。堆只关心一件事:**根是极值。**

### 用数组表示堆

因为堆的形状是完全的,可以不用指针,直接按层放进一个数组:

```text
        9                 数组:[9, 7, 2, 3, 1]
      /   \               下标: 0  1  2  3  4
     7     2
    / \
   3   1

下标 i 的节点:
  父节点     = (i - 1) / 2
  左孩子     = 2 * i + 1
  右孩子     = 2 * i + 2
```

用几个下标算一算就能找到父子关系,不需要指针。这样:内存连续(CPU 访问快,第 31 课提过)、没有指针开销、销毁时也不用担心递归(第 32 课的坑)。

**这就是 `std::priority_queue` 底层用 `std::vector` 存的原因。**

## 3. 插入和调整

往堆里加一个数:先放到数组末尾(树的最后一层最右边),然后**往上调整**:只要它比父亲大,就和父亲交换。一路往上,直到不比父亲大为止。这叫**上浮**。

从堆 `[9, 7, 2, 3, 1]` 开始,插入 8:

```text
下标:  0  1  2  3  4
      [9, 7, 2, 3, 1]

第一步,8 放到末尾 -> [9, 7, 2, 3, 1, 8]
  8 在下标 5,父亲是下标 (5-1)/2 = 2,值 2。8 > 2,交换
  -> [9, 7, 8, 3, 1, 2]

第二步,8 现在在下标 2,父亲是下标 (2-1)/2 = 0,值 9。8 < 9,停。

        9
      /   \
     7     8          插入完成
    / \   /
   3   1 2
```

每交换一次,往上走一层,最多走"树的高度"步。堆的高度是 `log n`,所以插入是 `O(log n)`。

## 4. 取出最大值和调整

取出根(最大值),然后把**数组最后一个元素**搬到根的位置,再**往下调整**:和两个孩子里**较大**的那个比,如果孩子更大就交换,一路往下。这叫**下沉**。

还是从 `[9, 7, 2, 3, 1]` 开始,取出最大值:

```text
      9                    取出根 9
    /   \
   7     2
  / \
 3   1

第一步,把末尾的 1 搬到根 -> [1, 7, 2, 3]
  1 的孩子是 7(下标1)和 2(下标2),较大的是 7。7 > 1,和 7 交换
  -> [7, 1, 2, 3]

第二步,1 现在在下标 1,左孩子是下标 3 的 3,右孩子不存在。3 > 1,交换
  -> [7, 3, 2, 1]

第三步,3 在下标 1,孩子下标 3、4,只有 3 存在,值 1。3 > 1,不用换,停。

        7
      /   \
     3     2           完成
    /
   1
```

注意第二步:要和**两个孩子中较大的**那个比。如果只和左孩子比,可能把大的孩子留在下面,违反堆的规则。

下沉同样是 `O(log n)`。

## 5. 自己写一个堆

用 `std::vector` 存,加两个调整函数:

```cpp
// file: my_heap.cpp
#include <iostream>
#include <queue>
#include <vector>

class MaxHeap {
public:
    void push(int value) {
        data.push_back(value);
        sift_up((int)data.size() - 1);
    }

    int top() const { return data.at(0); }

    void pop() {
        data.at(0) = data.back();
        data.pop_back();
        if (!data.empty()) {
            sift_down(0);
        }
    }

    bool empty() const { return data.empty(); }
    int size() const { return (int)data.size(); }

private:
    void sift_up(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (data[parent] >= data[i]) {
                return;
            }
            std::swap(data[parent], data[i]);
            i = parent;
        }
    }

    void sift_down(int i) {
        int n = (int)data.size();
        while (true) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int largest = i;
            if (left < n && data[left] > data[largest]) {
                largest = left;
            }
            if (right < n && data[right] > data[largest]) {
                largest = right;
            }
            if (largest == i) {
                return;
            }
            std::swap(data[i], data[largest]);
            i = largest;
        }
    }

    std::vector<int> data;
};

int main() {
    std::vector<int> values = {3, 9, 1, 7, 2, 8, 5, 5, 0, 10};

    MaxHeap mine;
    std::priority_queue<int> std_pq;
    for (int x : values) {
        mine.push(x);
        std_pq.push(x);
    }

    bool same = (mine.size() == (int)std_pq.size());
    while (!mine.empty() && !std_pq.empty()) {
        if (mine.top() != std_pq.top()) {
            same = false;
        }
        mine.pop();
        std_pq.pop();
    }
    std::cout << "和标准库结果一致:" << same << std::endl;
    return 0;
}
```

运行结果:

```text
和标准库结果一致:1
```

对照第 3、4 节的文字,看两个调整函数:

- `sift_up`:只要比父亲小就停(`data[parent] >= data[i]`)。用 `>=` 而不是 `>`,是为了不多做无谓的交换。
- `sift_down`:先假设自己最大,再依次和左孩子、右孩子比,找出三者中最大的位置 `largest`。如果最大的就是自己,停;否则和它交换,继续往下。这正是"和两个孩子中较大的比"。
- `pop` 里先检查 `!data.empty()`,因为搬完之后堆可能空了,这时不需要下沉。
- `data.at(0)`:用 `.at()`(第 7 课),万一在空堆上调用,会明确报错,而不是悄悄读坏数据。

## 6. 用途一:取前 k 大

**问题:** 从 100 万个数里,找出最大的 10 个。

三种做法,复杂度差别很大:

| 做法 | 复杂度 | 说明 |
| --- | --- | --- |
| 全部排序,取前 10 | `O(n log n)` | 做了很多不必要的工作 |
| 每次扫一遍找最大,重复 10 次 | `O(k × n)` | k 大时慢 |
| **维护一个大小为 k 的小顶堆** | `O(n log k)` | 最好 |

第三种的想法:手里只留 k 个候选,用**小顶堆**装着(这样能立刻知道这 k 个里**最小**的是谁)。扫过每个新数:

- 堆还没装满 k 个:直接放进去。
- 装满了,但新数比堆顶(候选里最小的)还小:不可能是前 k 大,跳过。
- 装满了,新数比堆顶大:淘汰堆顶,把新数放进去。

扫完,堆里就是前 k 大。因为 `log k` 里 k = 10,几乎是常数,所以接近 `O(n)`。

```cpp
// file: top_k.cpp
#include <algorithm>
#include <functional>
#include <iostream>
#include <queue>
#include <vector>

std::vector<int> top_k_largest(const std::vector<int>& v, int k) {
    std::priority_queue<int, std::vector<int>, std::greater<int>> smallest_on_top;
    for (int x : v) {
        if ((int)smallest_on_top.size() < k) {
            smallest_on_top.push(x);
        } else if (k > 0 && x > smallest_on_top.top()) {
            smallest_on_top.pop();
            smallest_on_top.push(x);
        }
    }

    std::vector<int> result;
    while (!smallest_on_top.empty()) {
        result.push_back(smallest_on_top.top());
        smallest_on_top.pop();
    }
    std::reverse(result.begin(), result.end());
    return result;
}

int main() {
    std::vector<int> data = {3, 90, 1, 70, 5, 80, 2, 60, 4, 100, 7};
    for (int x : top_k_largest(data, 3)) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    std::cout << "k 比总数还大,个数:" << top_k_largest(data, 100).size() << std::endl;
    std::cout << "k = 0,个数:" << top_k_largest(data, 0).size() << std::endl;
    std::cout << "空数组,个数:" << top_k_largest({}, 3).size() << std::endl;
    return 0;
}
```

运行结果:

```text
100 90 80 
k 比总数还大,个数:11
k = 0,个数:0
空数组,个数:0
```

### 这一版里三个容易出错的地方

**① 输出的顺序。** 从小顶堆里往外 `pop`,先出来的是**最小**的,所以得到的顺序是 `80 90 100`(从小到大)。想要"从大到小",得反过来。我在 `return` 前加了 `std::reverse`。

我自己第一次写这段时,就忘了这一层,以为出来就是最大的在前。**这是取前 k 大最常见的错误**,记住:小顶堆弹出是升序。

**② `k = 0` 时,`top()` 不能被调用。** 如果去掉 `k > 0` 这个条件,`else if (x > smallest_on_top.top())` 在堆为空时会调用 `top()`,这是危险操作(第 32 课)。加上 `k > 0 &&` 之后,`k = 0` 直接走不进这个分支。

注意 `&&` 的顺序:`k > 0` 必须写在左边。第 4 课说过,`&&` 左边为假就不再算右边,所以空的 `top()` 不会被执行。顺序写反,保护就失效了。

**③ `k` 比总数大。** 这种情况不需要特殊处理:装满 k 个的条件永远不成立,所有数都进了堆,返回全部,正是想要的。我把它列出来,是想说明:**边界情况要专门测一遍**(第 23 课),测了才知道它是自然成立的,还是需要额外代码。

## 7. 用途二:合并 k 个有序序列

**场景:** 日志系统里,有 100 个文件,每个文件里的记录**已经按时间排好序**。现在要输出一个总的、按时间排序的流。不可能把全部记录读进内存。

这是**归并排序**(第 28 课)"合并"那一步的现实版本。两个序列合并容易;k 个序列怎么合并?

**想法:** 每个序列各出一个"当前最小值"(因为它们各自有序,当前最前面的就是各自最小的)。把这 k 个候选放进**小顶堆**,每次取出全局最小的输出,然后从它所属的那个序列里补进下一个。

```cpp
// file: merge_sorted.cpp
#include <functional>
#include <iostream>
#include <queue>
#include <tuple>
#include <vector>

std::vector<int> merge_k(const std::vector<std::vector<int>>& lists) {
    using Item = std::tuple<int, int, int>;
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> pq;

    for (int i = 0; i < (int)lists.size(); i++) {
        if (!lists[i].empty()) {
            pq.push({lists[i][0], i, 0});
        }
    }

    std::vector<int> result;
    while (!pq.empty()) {
        auto [value, list_index, pos] = pq.top();
        pq.pop();
        result.push_back(value);

        if (pos + 1 < (int)lists[list_index].size()) {
            pq.push({lists[list_index][pos + 1], list_index, pos + 1});
        }
    }
    return result;
}

int main() {
    std::vector<std::vector<int>> lists = {
        {1, 5, 9},
        {2, 3, 9},
        {},
        {0, 7}
    };
    for (int x : merge_k(lists)) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    std::cout << "全是空:" << merge_k({{}, {}}).size() << std::endl;
    return 0;
}
```

运行结果:

```text
0 1 2 3 5 7 9 9 
全是空:0
```

- `std::tuple<int, int, int>` 是"装三个值的小盒子"(和第 32 课的 `std::pair` 同类,`pair` 装两个,`tuple` 可以装任意个)。这里存的是 `(值, 第几个序列, 序列里的下标)`。
- 堆里的排序,按 `tuple` 的**逐个成员**比较:先比值,值相同比序列编号,再相同比下标。这让相同大小的值有确定的先后,排序因此是**稳定**的(第 27 课)。
- `auto [value, list_index, pos] = pq.top();` 是结构化绑定(第 30 课)。
- 空的序列一开始就不进堆;某个序列用完了,就不再补。这两处检查,让函数能正确处理"序列长短不一"和"全是空"。
- 堆里始终只有 k 个元素,所以每次 `push`、`pop` 是 `O(log k)`。总共处理 n 个元素,合计 `O(n log k)`。

## 8. 从一堆数据一次性建堆

如果一个数一个数地 `push`,n 个数要 `O(n log n)`。但**一次性把整个数组变成堆,只要 `O(n)`**。

做法:从**最后一个非叶子节点**开始,倒着对每个节点做一次下沉。

为什么是 `O(n)`?因为大部分节点在树的底部,下沉的步数是 0 或 1;越往上节点越少。把所有节点的下沉步数加起来,结果是 `2n` 量级,所以是 `O(n)`(严格的推导要用到等比求和,本课不展开)。

标准库提供了:

```cpp
// file: heapify.cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {3, 9, 1, 7, 2, 8, 5};

    std::make_heap(v.begin(), v.end());
    std::cout << "堆的根:" << v.front() << std::endl;

    std::pop_heap(v.begin(), v.end());
    std::cout << "弹出最大后:" << v.front() << ",末尾是 " << v.back() << std::endl;

    std::sort_heap(v.begin(), v.end() - 1);
    for (int x : v) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

运行结果:

```text
堆的根:9
弹出最大后:8,末尾是 9
1 2 3 5 7 8 9 
```

- `std::make_heap`:原地把数组变成堆,`O(n)`。默认是**大顶堆**。
- `std::pop_heap`:把堆顶换到末尾,再把剩下的部分重新变成堆。**它不删除元素**,所以要配合 `v.pop_back()` 才真正少一个(这里为了输出清楚,直接展示了末尾)。
- `std::push_heap`、`std::sort_heap` 是配套的另外两个。`sort_heap` 反复弹出最大值,得到有序数组,这就是**堆排序**,`O(n log n)`。

实际写程序,一般直接用 `std::priority_queue` 或这几个函数,不用自己实现堆。学它的价值在于:**理解为什么这些操作是 `O(log n)`,以及什么时候该想到用堆。**

## 9. 堆的适用场合

| 需求 | 为什么用堆 |
| --- | --- |
| 只要极值,不需要全部有序 | 取最大最小 `O(1)`,插入 `O(log n)` |
| 取前 k 大 / 小 | `O(n log k)`,比全排序省 |
| 合并 k 个有序序列 | 堆里只放 k 个候选 |
| 任务调度:每次做优先级最高的事 | 天生匹配 |
| 数据流:实时维护"当前最大的 k 个" | 不需要保存全部历史 |

堆的局限:**只能取极值,不能按键查找,不能遍历成有序序列。** 需要那些,用第 33 课的二叉搜索树或第 18、31 课的 `map`、`unordered_map`。

## 练习

1. 把 `MaxHeap` 改成 `MinHeap`(改两处比较符号),和标准库的小顶堆对拍。
2. 给 `MaxHeap` 加一个 `static void build_from(std::vector<int> data)`,用第 8 节的"从最后一个非叶子节点倒着下沉"实现 `O(n)` 建堆。和逐个 `push` 的结果对比。
3. 用**大顶堆**实现取前 k **小**(提示:反过来想,维护 k 个候选,淘汰最大的)。
4. 有 100 个文件,每个文件里是一行一行的整数,已经排好序。写程序把它们合并输出成一个有序流,一次只在内存里保留 100 个数。用本课的 `merge_k` 思路。
5. 用 `std::make_heap` 和 `std::pop_heap` 手写一个堆排序,和第 28 课的归并排序对拍。
6. 思考:为什么"取前 k 大"用**小顶堆**,而不是大顶堆?

## 小结

- 优先队列:每次取出优先级最高(最大或最小)的元素。标准库 `std::priority_queue`,插入取出都是 `O(log n)`。
- 堆:完全二叉树 + "父不小于子"。最大的在根。
- 堆用**数组**表示,靠下标算父子关系:`父 = (i-1)/2`,`左 = 2i+1`,`右 = 2i+2`。不需要指针。
- 插入:放到末尾,往上**上浮**。取出:末尾搬到根,往下**下沉**,要和**两个孩子中较大的**比。
- 小顶堆弹出是**升序**,想要降序得反转。
- 一次性建堆是 `O(n)`,逐个插入是 `O(n log n)`。
- 典型用途:前 k 大、合并 k 个有序序列、任务调度。
- 堆只能取极值,不能查找、不能有序遍历。
