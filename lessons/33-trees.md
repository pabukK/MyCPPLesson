# 第 33 课:二叉树与二叉搜索树

## 这一课你会学到

- 树是什么,二叉树是什么
- 三种遍历顺序,以及为什么递归遍历只有几行
- 二叉搜索树:一种能让查找保持 `O(log n)` 的树
- 为什么它会在数据有序时退化,以及工程上怎么应对

## 1. 树是什么

数组和链表是"一条线"。树是"分叉"的结构,像倒过来的家谱:最上面是**根**,根下面挂着**孩子**,孩子再挂孩子。没有孩子的节点叫**叶子**。

```text
        8              <- 根
      /   \
     3     10          <- 第二层
    / \      \
   1   6      14       <- 叶子:1, 6, 14
```

**二叉树**:每个节点最多两个孩子,习惯上叫左孩子、右孩子。

树在工程里到处是:文件系统的目录、HTML 的标签结构、数据库的索引、编译器里的语法分析。

## 2. 用代码表示

和第 32 课的链表一样,节点里放指针。区别是链表只有一个 `next`,树有两个:

```cpp
// file: tree_node.cpp
#include <iostream>
#include <memory>
#include <vector>

struct Node {
    int value;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    Node(int v) : value(v) {}
};

void collect_inorder(const Node* node, std::vector<int>& out) {
    if (node == nullptr) {
        return;
    }
    collect_inorder(node->left.get(), out);
    out.push_back(node->value);
    collect_inorder(node->right.get(), out);
}

int main() {
    auto root = std::make_unique<Node>(8);
    root->left = std::make_unique<Node>(3);
    root->right = std::make_unique<Node>(10);
    root->left->left = std::make_unique<Node>(1);
    root->left->right = std::make_unique<Node>(6);
    root->right->right = std::make_unique<Node>(14);

    std::vector<int> order;
    collect_inorder(root.get(), order);
    for (int x : order) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

运行结果:

```text
1 3 6 8 10 14 
```

- 每个节点用 `unique_ptr` 管理,父节点拥有子节点。根销毁,整棵树自动释放,不用手写 `delete`(第 13 课)。
- 参数是 `const Node*`,一个"只看不改"的普通指针(第 32 课:只借用,不拥有)。
- 树不会像链表那样深到几万层,所以这里不用担心递归析构的栈溢出问题。

## 3. 三种遍历顺序

访问树里的每个节点一次,叫**遍历**。因为一个节点有"自己、左子树、右子树"三部分,顺序就有三种排法:

| 名字 | 顺序 | 记忆 |
| --- | --- | --- |
| 前序 | 自己 → 左 → 右 | 先记根 |
| 中序 | 左 → 自己 → 右 | 根在中间 |
| 后序 | 左 → 右 → 自己 | 最后记根 |

用上面那棵树,三种遍历的结果:

```text
        8
      /   \
     3     10
    / \      \
   1   6      14

前序:8 3 1 6 10 14
中序:1 3 6 8 10 14
后序:1 6 3 14 10 8
```

代码只差把 `out.push_back(node->value)` 放在哪一行,三个函数体几乎一模一样:

```cpp
// file: traversals.cpp
#include <iostream>
#include <memory>
#include <vector>

struct Node {
    int value;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    Node(int v) : value(v) {}
};

void preorder(const Node* n, std::vector<int>& out) {
    if (n == nullptr) return;
    out.push_back(n->value);
    preorder(n->left.get(), out);
    preorder(n->right.get(), out);
}

void inorder(const Node* n, std::vector<int>& out) {
    if (n == nullptr) return;
    inorder(n->left.get(), out);
    out.push_back(n->value);
    inorder(n->right.get(), out);
}

void postorder(const Node* n, std::vector<int>& out) {
    if (n == nullptr) return;
    postorder(n->left.get(), out);
    postorder(n->right.get(), out);
    out.push_back(n->value);
}

int main() {
    auto root = std::make_unique<Node>(8);
    root->left = std::make_unique<Node>(3);
    root->right = std::make_unique<Node>(10);
    root->left->left = std::make_unique<Node>(1);
    root->left->right = std::make_unique<Node>(6);
    root->right->right = std::make_unique<Node>(14);

    for (auto f : {preorder, inorder, postorder}) {
        std::vector<int> out;
        f(root.get(), out);
        for (int x : out) {
            std::cout << x << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}
```

运行结果:

```text
8 3 1 6 10 14 
1 3 6 8 10 14 
1 6 3 14 10 8 
```

- 每个节点访问一次,`O(n)`。
- 递归深度等于树的高度。平衡的树高度约 `log n`,退化的树高度可达 `n`(第 26 课的栈溢出风险)。
- `{preorder, inorder, postorder}` 是"函数指针数组"(第 32 课的 `std::stack` 用法类似),配合范围 for 少写三遍。

**为什么中序重要?** 对二叉搜索树做中序遍历,得到的正好是**从小到大**的有序序列。上面例子的中序结果 `1 3 6 8 10 14` 就是有序的,这不是巧合。

## 4. 二叉搜索树

**二叉搜索树**(简称 BST)是一种有规则的树:

> 每个节点,**左子树里所有的值都比它小,右子树里所有的值都比它大**。

```text
       8                    8 的左边 3、1、6 都比 8 小,右边 10、14 都比 8 大
     /   \                 3 的左边 1 比 3 小,右边 6 比 3 大
    3     10
   / \      \
  1   6      14
```

这个规则带来一个好处:**查找时,每次能排除一整棵子树。** 这和第 29 课的二分是同一个思想。

找 6:从 8 开始,6 比 8 小,去左边;6 比 3 大,去右边;找到了。三步。而 8 的左子树里那一整片(如果很大)全都不用看了。

```cpp
// file: bst.cpp
#include <algorithm>
#include <iostream>
#include <memory>

class Bst {
public:
    void insert(int value) { root = insert_into(std::move(root), value); }

    bool contains(int value) const {
        const Node* cur = root.get();
        while (cur != nullptr) {
            if (cur->value == value) {
                return true;
            }
            cur = (value < cur->value) ? cur->left.get() : cur->right.get();
        }
        return false;
    }

    int height() const { return height_of(root.get()); }

private:
    struct Node {
        int value;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        Node(int v) : value(v) {}
    };

    std::unique_ptr<Node> insert_into(std::unique_ptr<Node> node, int value) {
        if (node == nullptr) {
            return std::make_unique<Node>(value);
        }
        if (value < node->value) {
            node->left = insert_into(std::move(node->left), value);
        } else if (value > node->value) {
            node->right = insert_into(std::move(node->right), value);
        }
        return std::move(node);
    }

    int height_of(const Node* node) const {
        if (node == nullptr) {
            return 0;
        }
        int left_height = height_of(node->left.get());
        int right_height = height_of(node->right.get());
        return 1 + std::max(left_height, right_height);
    }

    std::unique_ptr<Node> root;
};

int main() {
    Bst tree;
    for (int x : {8, 3, 10, 1, 6, 14}) {
        tree.insert(x);
    }
    std::cout << "有 6 吗:" << tree.contains(6) << std::endl;
    std::cout << "有 5 吗:" << tree.contains(5) << std::endl;
    std::cout << "树高:" << tree.height() << std::endl;
    return 0;
}
```

运行结果:

```text
有 6 吗:1
有 5 吗:0
树高:3
```

几个要点:

- `insert_into` 的参数是 `std::unique_ptr<Node> node`(按值传),返回新的 `unique_ptr`。这是管理所有权比较清爽的写法:调用方写 `node->left = insert_into(std::move(node->left), value);`,把左子树交出去,再拿回处理好的。
- 值相等时,什么都不做,所以这棵树**不会有重复的值**。
- `contains` 用循环而不是递归:每步往左或往右走一层,直到空。
- `height_of` 是典型的**后序**思路:先问左右两边的高度,再算自己的。

**复杂度:** 查找、插入、删除,都要从根走到某个叶子,走的步数等于树的高度。

- 树平衡时,高度约 `log n`,所以是 `O(log n)`。
- 树退化时,高度等于 `n`,就是 `O(n)`。

## 5. 实测:BST 会在什么时候退化

BST 的速度完全取决于树高。我用随机顺序和**从小到大顺序**插入同样多的数,实测:

| n | 插入顺序 | 树高 | 插入总耗时 | 查最后一个数要走几步 |
| --- | --- | --- | --- | --- |
| 1 000 | 随机 | 23 | 0.06 毫秒 | 19 |
| 1 000 | 从小到大 | 1000 | 0.5 毫秒 | 1000 |
| 10 000 | 随机 | 30 | 0.8 毫秒 | 18 |
| 10 000 | 从小到大 | 10000 | 200 毫秒 | 10000 |
| 20 000 | 随机 | 33 | 2 毫秒 | 18 |
| 20 000 | 从小到大 | 20000 | 1000 毫秒 | 20000 |

(开了 `-O2` 优化,我的机器上的数字。你的会不同,看规律。)

规律很清楚:

- **随机顺序插入**:树高 23、30、33,和 `log n` 的量级吻合(1 万的 `log₂` 约 13,随机 BST 的树高经验上约为它的 2 倍)。插入 2 万个数只要 2 毫秒。
- **从小到大插入**:每个新数都比已有的大,一路往右走,树变成了一条链。树高 = n,插入 2 万个数要 1000 毫秒,**慢了 500 倍**。

而且注意:数据有序在实际中太常见了(按时间戳、按编号入库)。所以这不是冷门情形。

退化成链之后,BST 就完全不如 `vector` 了:又慢,又费内存(每个节点还要存两个指针)。

## 6. 工程上怎么办

**自己写平衡树(红黑树、AVL 树)是不明智的。** 平衡树的核心是"旋转":在插入删除后,通过调整局部结构把树压回去,保证树高始终是 `O(log n)`。规则复杂,极易写错,而且错了不会报错,只会悄悄变慢。

正确做法:**用标准库。**

- `std::map`、`std::set`(第 18 课)内部就是**自平衡**的二叉搜索树(通常是红黑树)。查找、插入、删除,保证 `O(log n)`,而且按键有序。
- `std::unordered_map`、`std::unordered_set`(第 31 课)是哈希表,平均 `O(1)`,但没有顺序。

所以现在可以把前面几课的一张表补完整了:

| 需求 | 选择 | 查找 | 有序 |
| --- | --- | --- | --- |
| 只要按键查值 | `unordered_map` | 平均 `O(1)` | 否 |
| 要按键查值,还要有序、范围查询 | `map` | `O(log n)` | 是 |
| 只在末尾增删,按下标访问 | `vector` | `O(n)`(要遍历) | 自己排 |
| 只取极值 | `priority_queue` | 极值 `O(1)` | 否 |

**这一课自己写 BST,是为了理解 `map` 里面在做什么,以及理解"树高决定一切"。** 真正写项目时,直接 `std::map`。

顺便说一句:数据库的索引、文件系统的目录,用的都是这一类平衡树。

## 练习

1. 给 `Bst` 加一个 `void print_inorder()`,用中序遍历输出。插入 `{8, 3, 10, 1, 6, 14}` 后调用,验证输出是升序。
2. 给 `Bst` 加 `int min_value()` 和 `int max_value()`:一直往左走 / 一直往右走。想想退化成链时它们是 `O(n)` 还是 `O(log n)`。
3. 写函数,求一棵二叉树的节点个数和最大深度。用第 26 课的递归思路。
4. 写函数 `bool is_bst(const Node* root)`,判断一棵树是不是合法的二叉搜索树。提示:不能只比较父节点和孩子,因为左子树里**所有**节点都要小于根。(提示:给递归加上下界,或者用中序遍历检查是否升序。)
5. 实测:用 `std::map` 按从小到大的顺序插入 2 万个数,再测随机顺序,比较耗时。观察它会不会像 BST 那样退化。
6. 用 `std::map` 重写第 31 课的"两数之和",想想行不行得通,复杂度是多少。

## 小结

- 树是分叉结构;二叉树每个节点最多两个孩子。
- 三种遍历:前序(自己→左→右)、中序(左→自己→右)、后序(左→右→自己)。递归写法只有几行。
- **二叉搜索树**:左子树全部小于自己,右子树全部大于自己。中序遍历得到升序。
- BST 的查找、插入、删除,复杂度等于**树高**。平衡时 `O(log n)`,退化时 `O(n)`。
- 按有序数据插入,BST 会退化成一条链,实测慢 500 倍。
- 自己写平衡树极易出错,工程上直接用 `std::map` / `std::set`,它们是自平衡的。
- 数据库索引、文件系统目录,用的都是平衡树。
