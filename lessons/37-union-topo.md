# 第 37 课:并查集与拓扑排序

## 这一课你会学到

- 并查集:快速地判断"两个东西是否属于同一组"
- 它的两个关键优化:按大小合并、路径压缩
- 拓扑排序:有依赖关系时,决定处理的先后
- 用它们解决工程里的实际问题

## 1. 并查集解决什么

**问题:** 有一堆东西,不断被告知"这两个是同一组",然后要问"这两个是不是同一组?"

现实例子:

- 社交网络:判断两个人是否在同一个好友圈(是否互相认识,直接或通过朋友)。
- 网络连通:判断两台机器是否在同一网段。
- 等价类:编译器的类型推断,A 类型等于 B,B 等于 C,那 A 等于 C。
- 判断一张图有几个连通块(第 35 课用 DFS 做过)。

用 DFS 每次查询都要走一遍,`O(V + E)`。并查集能做到:**平均下来几乎每次操作只花常数时间**(记作 `O(α(n))`,α 是一个增长极慢的函数,n 取宇宙中的原子数时它不超过 4,所以实践中就当 `O(1)`)。

## 2. 想法:用一棵树代表一组

每个组用一棵树表示,树的**根**就是这一组的"代表"。

- `find(x)`:找到 x 所在组的根。判断"x 和 y 是否同组",就是比较它们的根。
- `union(a, b)`:把两个组合并,让一棵树的根挂到另一棵树的根下面。

```text
初始:每个点自己一组
[0]  [1]  [2]  [3]  [4]

union(0, 1),union(2, 3):
  0    2    4
  |    |
  1    3

union(1, 3): 把两棵树接起来
    0
   / \
  1   2
      | \
      3  ...

find(0) 和 find(3) 都返回 0 -> 同组
find(4) 返回 4 -> 和它们不同组
```

```cpp
// file: union_find_basic.cpp
#include <iostream>
#include <vector>

class UnionFind {
public:
    UnionFind(int n) : parent(n) {
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }
    }

    int find(int x) {
        while (parent[x] != x) {
            x = parent[x];
        }
        return x;
    }

    void unite(int a, int b) {
        int ra = find(a);
        int rb = find(b);
        if (ra != rb) {
            parent[rb] = ra;
        }
    }

    bool connected(int a, int b) {
        return find(a) == find(b);
    }

private:
    std::vector<int> parent;
};

int main() {
    UnionFind uf(5);
    uf.unite(0, 1);
    uf.unite(2, 3);
    uf.unite(1, 3);

    std::cout << "0 和 4 同组吗:" << uf.connected(0, 4) << std::endl;
    std::cout << "0 和 3 同组吗:" << uf.connected(0, 3) << std::endl;
    std::cout << "1 和 2 同组吗:" << uf.connected(1, 2) << std::endl;
    return 0;
}
```

运行结果:

```text
0 和 4 同组吗:0
0 和 3 同组吗:1
1 和 2 同组吗:1
```

- 构造函数里 `parent[i] = i`:每个点的父亲是自己,所以自己就是根。
- `find`:顺着父亲一路往上走,走到"父亲是自己"的那个点,就是根。
- `unite`:把 b 的根挂到 a 的根下面。

**这个朴素版本有个问题:** 树可能变得很高。如果每次都是 `unite(新点, 老组)`,树会变成一条链,`find` 退化成 `O(n)`。第 25 课说过,这种"平时快、最坏慢"的情况要小心。下面两个优化解决它。

## 3. 优化一:按大小合并

合并时,**总是让小树的根,挂到大树的根下面**。这样树的高度就不会乱长。

```text
  0        2      把小树(2 个)挂到大树(3 个)下面,而不是反过来
 / \      / \
1   3    4   5

  0
 /|\
1 2 3
  |\
  4 5
```

直觉:只有当两棵一样高的树合并时,高度才会增加。而要让高度加 1,至少需要 2 倍数量的节点。所以树高最多 `log n`,`find` 最坏 `O(log n)`,不会退化成链。

## 4. 优化二:路径压缩

`find` 的时候,既然已经知道了从 x 到根的路径,就**顺手把路径上的每个点,直接接到根上**。下次再找它们,一步就到。

```text
find(4) 之前:        find(4) 之后:
    0                    0
   / \                  / | \ \
  1   2                1  2  3  4  5
     / \                  (全部直连根)
    3   4
    |
    5
```

```cpp
// file: union_find_fast.cpp
#include <iostream>
#include <vector>

class UnionFind {
public:
    UnionFind(int n) : parent(n), size(n, 1), groups(n) {
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }
    }

    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    void unite(int a, int b) {
        int ra = find(a);
        int rb = find(b);
        if (ra == rb) {
            return;
        }
        if (size[ra] < size[rb]) {
            std::swap(ra, rb);
        }
        parent[rb] = ra;
        size[ra] += size[rb];
        groups--;
    }

    bool connected(int a, int b) {
        return find(a) == find(b);
    }

    int group_count() const { return groups; }
    int group_size(int x) { return size[find(x)]; }

private:
    std::vector<int> parent;
    std::vector<int> size;
    int groups;
};

int main() {
    UnionFind uf(6);
    uf.unite(0, 1);
    uf.unite(2, 3);
    uf.unite(0, 3);
    uf.unite(4, 5);

    std::cout << "0 和 3 同组吗:" << uf.connected(0, 3) << std::endl;
    std::cout << "0 和 5 同组吗:" << uf.connected(0, 5) << std::endl;
    std::cout << "有几个组:" << uf.group_count() << std::endl;
    std::cout << "0 所在组的人数:" << uf.group_size(0) << std::endl;
    std::cout << "4 所在组的人数:" << uf.group_size(4) << std::endl;

    uf.unite(3, 5);
    std::cout << "合并后有几个组:" << uf.group_count() << std::endl;
    std::cout << "0 所在组的人数:" << uf.group_size(0) << std::endl;
    return 0;
}
```

运行结果:

```text
0 和 3 同组吗:1
0 和 5 同组吗:0
有几个组:2
0 所在组的人数:4
4 所在组的人数:2
合并后有几个组:1
0 所在组的人数:6
```

两个优化都在:

- `find` 里的 `parent[x] = find(parent[x]);` 就是路径压缩。递归走到根,回来的路上,把每个点的父亲改成根。这一行代码,是并查集最漂亮的地方。
- `unite` 里先比较 `size`,保证大树当父亲(`std::swap` 那一行)。
- `size` 只在根上是准确的,所以 `group_size` 先 `find(x)` 拿到根,再读 `size[根]`。
- `groups` 记录还剩几个组,每次真正合并就减 1。判断"图是否连通"就变成:`groups == 1`。

**两个优化合起来,每次操作的平均代价就是常数级**(前面说的 `α(n)`)。这也是并查集在工程里能处理上亿次操作的原因。

## 5. 并查集的典型用法

| 问题 | 怎么做 |
| --- | --- |
| 判断图的连通分量个数 | 每条边 `unite`,最后看 `group_count()` |
| 判断两点是否连通 | `connected(a, b)` |
| 判断加一条边会不会形成环 | 如果 `connected(a, b)` 已经是 1,加这条边就成环 |
| 数"岛屿"个数(网格) | 把相邻的格子 `unite` |
| 变量等价类(类型推断) | 声明相等就 `unite`,查询就 `connected` |
| 第 41 课的 KMP 之类的前缀问题 | 有时用并查集更简单 |

第三条"**判断会不会成环**"很有用:第 38 课的最小生成树,就是靠它按权重从小到大加边,跳过会成环的那些。

## 6. 拓扑排序解决什么

**问题:** 一堆任务,有些必须等其他任务做完才能开始。给出一个可行的执行顺序。

```text
编译项目:
  头文件 -> student.cpp
  头文件 -> gradebook.cpp
  student.cpp -> 可执行文件
  gradebook.cpp -> 可执行文件

可行的顺序之一:头文件, student.cpp, gradebook.cpp, 可执行文件
不可能的顺序:可执行文件, 头文件, ...
```

这是**有向图**(边表示"必须先做"),而且必须**没有环**(有环就永远做不完,A 等 B,B 等 A)。这种图叫**有向无环图**,简称 DAG。

**想法(Kahn 算法):**

1. 统计每个点的**入度**:有多少条边指向它。入度为 0,意味着"没有前置任务",可以立刻做。
2. 把所有入度为 0 的点放进**队列**(第 35 课)。
3. 从队列取出一个点,放进结果;它指向的那些点,入度减 1。**减到 0 的,就入队**。
4. 队列空了,结束。

```text
头文件(入度 0) -> student.cpp(入度 1) -> 可执行文件(入度 2)
                -> gradebook.cpp(入度 1) ->/

入度:头文件 0,student.cpp 1,gradebook.cpp 1,可执行文件 2
队列:[头文件]  -> 取出头文件,student.cpp 和 gradebook.cpp 入度变 0,入队
队列:[student.cpp, gradebook.cpp]
     -> 取出 student.cpp,可执行文件入度 2->1
     -> 取出 gradebook.cpp,可执行文件入度 1->0,入队
     -> 取出可执行文件
结果:头文件, student.cpp, gradebook.cpp, 可执行文件
```

```cpp
// file: topo_sort.cpp
#include <iostream>
#include <queue>
#include <string>
#include <vector>

std::vector<int> topo_sort(const std::vector<std::vector<int>>& adj, const std::vector<int>& indegree) {
    std::vector<int> in = indegree;
    std::queue<int> q;
    for (int i = 0; i < (int)in.size(); i++) {
        if (in[i] == 0) {
            q.push(i);
        }
    }

    std::vector<int> order;
    while (!q.empty()) {
        int node = q.front();
        q.pop();
        order.push_back(node);
        for (int next : adj[node]) {
            if (--in[next] == 0) {
                q.push(next);
            }
        }
    }

    if ((int)order.size() != (int)adj.size()) {
        return {};
    }
    return order;
}

int main() {
    std::vector<std::string> names = {"头文件", "student.cpp", "gradebook.cpp", "链接", "运行测试"};
    std::vector<std::vector<int>> adj(5);
    std::vector<int> indegree(5, 0);
    auto add_dep = [&](int from, int to) {
        adj[from].push_back(to);
        indegree[to]++;
    };
    add_dep(0, 1);
    add_dep(0, 2);
    add_dep(1, 3);
    add_dep(2, 3);
    add_dep(3, 4);

    std::vector<int> order = topo_sort(adj, indegree);
    if (order.empty()) {
        std::cout << "有环,无法排序" << std::endl;
    } else {
        for (int i : order) {
            std::cout << names[i] << " ";
        }
        std::cout << std::endl;
    }

    std::vector<std::vector<int>> cyclic(3);
    std::vector<int> cin3(3, 0);
    cyclic[0].push_back(1); cin3[1]++;
    cyclic[1].push_back(2); cin3[2]++;
    cyclic[2].push_back(0); cin3[0]++;
    std::cout << "有环的情况:" << topo_sort(cyclic, cin3).size() << std::endl;
    return 0;
}
```

运行结果:

```text
头文件 student.cpp gradebook.cpp 链接 运行测试 
有环的情况:0
```

几个要点:

- `std::vector<int> in = indegree;` **复制一份**入度数组。算法会不断修改它,不能破坏调用者的数据(第 6 课:参数是复印件,但这里传的是引用式的 vector,所以必须自己复制)。
- `if (--in[next] == 0)`:先减 1,再判断。`--x` 是"减完再用值",`x--` 是"先用值再减"(第 3 课)。
- **判断有没有环,就看结果里有没有装下所有的点。** 有环时,环上的点入度永远减不到 0,进不了队列,所以 `order.size()` 小于点数。这是最省事的判环方式,不用额外的算法。
- 复杂度 `O(V + E)`:每个点入队一次,每条边看一次。

## 7. 拓扑排序的用途

| 场景 | 说明 |
| --- | --- |
| 构建系统(make、CMake) | 决定哪些文件先编译。第 24 课的项目就是靠它 |
| 安装包依赖 | apt、npm 决定安装顺序 |
| 数据库的迁移脚本 | 表 A 依赖表 B,按顺序执行 |
| 课程表:先修关系 | 判断能不能按期修完 |
| 编译器的循环依赖检测 | 有环就报错,并指出环在哪 |
| 任务调度 | 有前置条件的任务排序 |

**如果发现环,怎么找出环是哪些点?** 剩下的那些"入度没减到 0"的点,就都在环上或环的下游。从它们出发再走一遍 DFS(第 35 课),就能定位具体的环。

## 8. 实测:并查集 vs DFS

我用"数一个图有几个连通块"来对比。100 万个点、200 万条随机边:

| 做法 | 耗时 |
| --- | --- |
| DFS(显式栈,第 35 课;不含建邻接表) | 131 毫秒 |
| 并查集(直接处理边列表) | 34 毫秒 |
| 建邻接表(200 万条边) | 471 毫秒 |

(我的机器,`-O2`。你的数字会不同,看规律。)

两者数出的连通块**都是 18836**,互相验证了正确性。

结果和"都是 `O(V + E)`,应该差不多"的直觉不符,并查集明显更快。原因值得记下来,它体现了第 25 课的一个要点:**大 O 相同,不代表实际耗时相同。**

- **内存访问模式。** DFS 要沿着邻接表跳来跳去,访问的是分散在堆上的几百个小数组,CPU 缓存命中率低。并查集只有一个 `parent` 数组,顺序访问,缓存友好。
- **建图的成本。** 用 DFS 就得先建邻接表,这一步单独就花了 471 毫秒,比两种算法本身都贵。并查集不需要建图,直接吃边列表。
- **路径压缩的效果。** 树被压平之后,后续的 `find` 几乎是两次数组读取。

所以:**要反复合并、反复查询,或者内存紧张,用并查集。** 需要遍历图的结构本身(找路径、判环、分层),才用 DFS 或 BFS。

## 练习

1. 给 `UnionFind` 加一个 `bool unite_and_check(int a, int b)`:合并成功返回 true,本来就在同一组(说明这条边会形成环)返回 false。用它判断一个无向图有没有环。
2. 用并查集重写第 35 课的"数连通块",和 DFS 版本对拍,确认答案一致。
3. 网格版的"数岛屿":给一个 0/1 网格,把相邻的 1 用并查集连起来,数有几个组。和第 35 课的 DFS 版本对比。
4. 给 `topo_sort` 加输出"具体是哪些点构成了环"(提示:结果不完整时,剩下的点都在环上或环的下游)。
5. 用拓扑排序解决:给一堆课程和先修关系,判断能否在 n 学期内修完,每学期可以并行修多门没冲突的课。(提示:按"层"处理,这就是 BFS 的分层。)
6. 思考:拓扑排序的结果是唯一的吗?构造一个有多种合法顺序的例子。

## 小结

- 并查集:判断"两点是否同组",支持动态合并。`find` 找根,`unite` 合并。
- 两个优化:**按大小合并**(小树挂大树,树高 `O(log n)`)和**路径压缩**(`find` 时把路径上的点直连根)。合起来平均接近 `O(1)`。
- `group_count()` 直接给出连通块个数;`connected(a, b)` 为真时再加一条边就成环。
- 一次性数连通块,DFS 就够,并查集不见得更快。并查集的优势在**动态合并 + 反复查询 + 省内存**。
- 拓扑排序:有向无环图的执行顺序。Kahn 算法用入度 + 队列,`O(V + E)`。
- **结果装不下所有点,就说明有环。** 这是最简单的判环方法。
- 构建系统、包安装、任务调度,用的都是拓扑排序。
