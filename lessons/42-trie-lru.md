# 第 42 课:前缀树与 LRU 缓存

## 这一课你会学到

- 前缀树(字典树):按字符一层一层组织的树,用来做"以某串开头"的查找
- LRU 缓存:淘汰"最久没用过"的那一项
- 它是怎么把哈希表和链表合在一起,做到两个操作都 `O(1)` 的
- 这两个结构在真实项目里的位置

## 1. 前缀树解决什么

第 31 课的哈希表,回答"这个词**在不在**集合里",`O(1)`。

但有些事哈希表做不了:

- 以 `"app"` **开头**的词有哪些?(自动补全)
- 集合里有没有以任何一个前缀匹配的词?(拼写检查)
- 按字典序,所有词是怎么排列的?

哈希表把整个词压成一个数,前缀的信息就丢了。**前缀树(trie,读作"try")保留了这个信息**:它按字符一层一层分叉。

```text
存入:app, apply, bat

        (根)
       /    \
      a      b
      |      |
      p      a
      |      |
      p*     t*
     /
    l
   /
  y*

带 * 的节点表示"到这里是一个完整的词"
```

- 从根往下走,每走一格消费一个字符。走到的路径拼起来,就是某个前缀。
- `"app"` 这个节点带 `*`,说明 app 是一个词;它下面还有 `ly`,说明 apply 也是。
- 以 `"app"` 开头的词,就是 `app` 这个节点**为根的整棵子树**里带 `*` 的节点。

## 2. 实现

每个节点存:26 个孩子(小写字母)和一个"这里是不是完整的词"的标记。

```cpp
// file: trie.cpp
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Trie {
public:
    void insert(const std::string& word) {
        Node* cur = root.get();
        for (char c : word) {
            int idx = c - 'a';
            if (cur->children[idx] == nullptr) {
                cur->children[idx] = std::make_unique<Node>();
            }
            cur = cur->children[idx].get();
        }
        cur->is_word = true;
    }

    bool search(const std::string& word) const {
        const Node* cur = find_node(word);
        return cur != nullptr && cur->is_word;
    }

    bool starts_with(const std::string& prefix) const {
        return find_node(prefix) != nullptr;
    }

    void collect(const std::string& prefix, std::vector<std::string>& out) const {
        const Node* cur = find_node(prefix);
        if (cur == nullptr) {
            return;
        }
        dfs(cur, prefix, out);
    }

private:
    struct Node {
        std::unique_ptr<Node> children[26];
        bool is_word = false;
    };

    const Node* find_node(const std::string& s) const {
        const Node* cur = root.get();
        for (char c : s) {
            int idx = c - 'a';
            if (idx < 0 || idx >= 26 || cur->children[idx] == nullptr) {
                return nullptr;
            }
            cur = cur->children[idx].get();
        }
        return cur;
    }

    void dfs(const Node* node, const std::string& path, std::vector<std::string>& out) const {
        if (node->is_word) {
            out.push_back(path);
        }
        for (int i = 0; i < 26; i++) {
            if (node->children[i] != nullptr) {
                dfs(node->children[i].get(), path + (char)('a' + i), out);
            }
        }
    }

    std::unique_ptr<Node> root = std::make_unique<Node>();
};

int main() {
    Trie trie;
    trie.insert("app");
    trie.insert("apply");
    trie.insert("bat");

    std::cout << "有 app 吗:" << trie.search("app") << std::endl;
    std::cout << "有 ap 吗:" << trie.search("ap") << std::endl;
    std::cout << "以 ap 开头:" << trie.starts_with("ap") << std::endl;
    std::cout << "以 xyz 开头:" << trie.starts_with("xyz") << std::endl;

    std::vector<std::string> words;
    trie.collect("app", words);
    std::cout << "以 app 开头的词:";
    for (const auto& w : words) {
        std::cout << w << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

运行结果:

```text
有 app 吗:1
有 ap 吗:0
以 ap 开头:1
以 xyz 开头:0
以 app 开头的词:app apply 
```

- `search("ap")` 是 0,但 `starts_with("ap")` 是 1。这正是前缀树和哈希表的区别:ap 不是词,但它是 app、apply 的前缀。
- `collect("app", ...)` 走的是 app 节点为根的整棵子树,按 `i` 从 0 到 25 的顺序递归,所以结果自然**按字典序**排列。

**逐处看几个要点:**

- `int idx = c - 'a';`:字符相减得到它在字母表里的序号。`'a'` 减 `'a'` 是 0,`'b'` 减 `'a'` 是 1。这是把字符变成数组下标的标准做法(第 31 课说字符可以当键)。
- `find_node` 里检查 `idx < 0 || idx >= 26`:输入含有非小写字母的字符时,不检查就会**越界访问数组**(第 7 课、第 12 课)。这是这类代码最容易漏的安全检查。
- 节点用 `unique_ptr` 持有孩子,根 `root` 销毁时整棵树自动释放,不用手写 `delete`(第 13 课)。而且树的深度等于词长,不会像链表那样(第 32 课)深到栈溢出。
- `std::unique_ptr<Node> children[26];` 是"26 个智能指针的数组",这是结构体里放定长数组的写法(第 7 课)。

## 3. 前缀树的代价

| | 哈希表 | 前缀树 |
| --- | --- | --- |
| 查找一个词 | 平均 `O(词长)` | `O(词长)` |
| 前缀查询、自动补全 | 做不到 | **可以** |
| 按字典序遍历 | 要另外排序 | **天然有序** |
| 内存 | 只存实际出现的词 | 每个节点 26 个指针,**很费内存** |

内存是前缀树最大的问题。每个节点 26 个指针,在 64 位系统上每个指针 8 字节,一个节点就是 208 字节,再加上标记和内存对齐,约 216 字节。存 10 万个词、平均 8 个字符,节点数可能有几十万,内存就要上百 MB。

省内存的办法(了解即可):

- 孩子用 `map` 或 `unordered_map` 代替 26 个槽,只存实际用到的分支。
- **压缩前缀树**:把只有一条孩子的连续路径合并成一个节点。
- 用数组而不是指针,下标表示父子关系。

## 4. 前缀树的用途

| 场景 | 说明 |
| --- | --- |
| 搜索引擎、输入法的自动补全 | 输入 "pyt",提示 "python" |
| 拼写检查 | 找前缀最接近的词 |
| IP 路由表 | 最长前缀匹配:哪个网段的前缀和这个 IP 重合得最长 |
| 敏感词过滤 | 大量模式串同时匹配 |
| 单词游戏(Scrabble) | 判断手上的字母能不能拼出词典里的词 |

**第 3 条是前缀树在工业界最重要的一处应用。** 网络设备的转发表,本质就是一棵前缀树。

## 5. LRU 缓存

**问题:** 缓存只能放 k 项。要放新项时,如果满了,**淘汰最久没有被使用的那一项**(LRU = Least Recently Used,最近最少使用)。

```text
容量 2,依次访问 A B A C
链表头放"最近用过"的,链表尾放"最久没用"的

放 A:      [A]
放 B:      [B A]
访问 A:    [A B]        A 变成最近用过的,挪到链表头
放 C:      满了,淘汰最久没用的 B(在尾部) -> [C A]
```

为什么是常见面试题?因为它要求两个操作都 `O(1)`:

- **查找**:某个键在不在缓存里?→ 需要哈希表(第 31 课)。
- **维护"使用顺序"**:淘汰最久没用的,并且每次访问都要把它标记为"最新"。→ 需要能在任意位置 `O(1)` 插入删除的结构 → **双向链表**(第 32 课)。

单靠一个不行:哈希表没有顺序;链表查找要 `O(n)`。**两个拼起来,各补对方的短。**

```text
哈希表:key -> 链表节点的位置
双向链表:头端是"最近用过",尾端是"最久没用"

  哈希表                 双向链表
  A -> 节点A  --+   头 [B] <-> [A] <-> [C] 尾
  B -> 节点B    |
  C -> 节点C  --+
                 淘汰时:取尾节点,从哈希表里删掉它的键
```

## 6. 实现 LRU

标准库的 `std::list`(双向链表,第 32 课)和 `std::unordered_map`(哈希表,第 31 课)拼起来:

```cpp
// file: lru_cache.cpp
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <utility>

class LruCache {
public:
    explicit LruCache(int capacity) : capacity(capacity) {}

    int get(int key) {
        auto it = index.find(key);
        if (it == index.end()) {
            return -1;
        }
        items.splice(items.begin(), items, it->second);
        return it->second->second;
    }

    void put(int key, int value) {
        auto it = index.find(key);
        if (it != index.end()) {
            it->second->second = value;
            items.splice(items.begin(), items, it->second);
            return;
        }
        if ((int)index.size() == capacity) {
            index.erase(items.back().first);
            items.pop_back();
        }
        items.emplace_front(key, value);
        index[key] = items.begin();
    }

    int size() const { return (int)items.size(); }
    const std::list<std::pair<int, int>>& order() const { return items; }

private:
    int capacity;
    std::list<std::pair<int, int>> items;
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> index;
};

int main() {
    LruCache cache(2);
    cache.put(1, 100);
    cache.put(2, 200);
    std::cout << cache.get(1) << std::endl;
    cache.put(3, 300);
    std::cout << "键 2 还在吗:" << cache.get(2) << std::endl;
    std::cout << "键 1:" << cache.get(1) << std::endl;
    std::cout << "键 3:" << cache.get(3) << std::endl;

    cache.put(1, 999);
    std::cout << "更新键 1:" << cache.get(1) << std::endl;
    std::cout << "个数:" << cache.size() << std::endl;

    std::cout << "顺序(新在前):";
    for (const auto& p : cache.order()) {
        std::cout << p.first << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

运行结果:

```text
100
键 2 还在吗:-1
键 1:100
键 3:300
更新键 1:999
个数:2
顺序(新在前):1 3 
```

对照第 5 节的流程走一遍:

- `put(1,100)`、`put(2,200)`:链表 `[2, 1]`,2 最新(新的总是放在链表头)。
- `get(1)`:命中,把 1 挪到最前,链表 `[1, 2]`。返回 100。
- `put(3,300)`:满了,淘汰最久没用的 2(在链表尾部),链表 `[3, 1]`。
- `get(2)`:已经不在了,返回 -1。
- `put(1,999)`:键 1 已存在,**只更新值**,不新增,所以个数还是 2。

**`splice` 是这里的关键。** `items.splice(目标位置, 另一个链表, 节点位置)` 把一个节点**从原处摘下来,接到目标处**,不复制元素、不分配内存,`O(1)`。用它把"被访问的节点"挪到链表头部,正好表达"它变成最近用过的"。

(第三个参数是同一个链表里的节点时,意思是"在本链表内移动"。这是标准库允许的用法。)

**为什么哈希表的值是指针(迭代器)?** `index` 的类型是 `unordered_map<int, list<...>::iterator>`:键 → 它在链表里的**位置**。查到位置后,直接 `splice` 移动,`O(1)`。如果哈希表只存值,就还得遍历链表找节点,退化成 `O(n)`。

**一个必须知道的坑:** `std::list` 的迭代器指向的节点,只要那个节点不被删除,就**一直有效**(第 32 课说过链表删除只影响被删的那个节点)。所以把迭代器存进哈希表是安全的。

**换成 `std::vector` 就不行了**:`vector` 插入删除会让后面的元素整体挪动,所有迭代器、指针全部失效(第 12 课的悬空指针)。这也是为什么 LRU 必须用链表,不能用 vector。

## 7. 工程里的 LRU

真实项目里不要自己写,用现成的:

- 数据库的缓冲池(MySQL InnoDB、PostgreSQL)用的就是 LRU 的变种。
- Redis 的 `maxmemory-policy allkeys-lru`:内存满了就淘汰最久没用的键。
- 浏览器的图片缓存、操作系统的页面置换、CPU 的缓存,都是同一类思路。
- Web 服务里给接口结果加缓存,常用 `LRU-Cache` 这类库,或 Redis。

**要注意的点(工程角度):**

- 多线程下,`get` 也要改链表顺序,这是**写操作**,必须加锁。所以很多实现用"近似 LRU"(分段 LRU):把缓存切成若干段,各段独立维护,减少锁的竞争。Redis 就是这么做的。
- 严格的 LRU 对**偶发的批量扫描**很敏感:一次扫过 100 万个键,会把缓存里真正热的数据全部挤出去。所以实际系统常用 LRU-K、2Q、ARC 这些改良版。

## 8. 这一课把前面的知识串起来了

| 用到的内容 | 来自 |
| --- | --- |
| 双向链表 | 第 32 课 |
| 哈希表 | 第 31 课 |
| 智能指针管理树 | 第 13 课 |
| 递归深度和栈 | 第 26、32 课 |
| 越界检查 | 第 7、12 课 |
| 迭代器失效 | 第 12 课 |

**这就是为什么课程把内存部分放在前面。** 看不懂 `unique_ptr` 和迭代器为什么会失效,这两课的结构就写不对。

## 练习

1. 给 `Trie` 加 `bool erase(const std::string& word)`:删掉一个词。注意只有当某个节点"不再是词,也没有孩子"时才能真的删掉它,否则会把别的词切断。
2. 用前缀树解决:给一个词表,判断一个字符串能不能拆成词表里的若干个词(单词拆分)。
3. 把 `LruCache` 的容量改成 1,验证 `put` 两个不同的键之后,第一个立刻被淘汰。
4. 给 `LruCache` 加一个 `bool erase(int key)`,并从 `index` 和 `items` 两边同时删掉。想想只删一边会怎样。
5. 实现 MRU(最常使用先淘汰)缓存,和 LRU 比较代码的差别在哪一行。
6. 思考:如果只用 `unordered_map` 存 `(值, 上次访问的时间戳)`,淘汰时扫一遍找时间最早的,能行吗?复杂度是多少?和双向链表方案比,什么时候这个更合适?

## 小结

- 前缀树:按字符分叉的树,节点标记"这里是不是完整的词"。查找 `O(词长)`,能做**前缀查询、自动补全、按字典序遍历**,这是哈希表做不到的。
- 代价是每个节点 26 个指针,**很费内存**。
- 工业上最重要的用途之一:IP 路由表的**最长前缀匹配**。
- 字符转下标:`c - 'a'`,但要检查是不是在 0 到 25 之内,否则会越界。
- LRU 缓存:淘汰最久没用的。要求"查找"和"调整顺序"都 `O(1)`。
- 实现:**哈希表(键 → 链表节点位置)+ 双向链表(新在前,旧在后)**,用 `splice` 移动节点。
- 用链表是因为它的迭代器不会因别的节点增删而失效;`vector` 不行。
- 工程上用现成的库;多线程下 `get` 也要加锁,所以常用分段或近似 LRU。
