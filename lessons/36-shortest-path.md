# 第 36 课:最短路径

## 这一课你会学到

- 边有权重时,BFS 为什么不够
- Dijkstra 算法:每次确定"当前最近的那个点"
- 用优先队列实现它
- 什么时候不能用 Dijkstra
- 工程里实际怎么选

## 1. 边有权重

第 35 课的图,每条边都一样。现实里不是:两个城市之间的距离、两段路的行车时间、两次转账的手续费,都不同。

```text
    0 --4--> 1
    |        |
    1        2
    |        |
    2 --5--> 3
```

从 0 到 3,有两条路:

- 0 → 1 → 3:4 + 2 = 6
- 0 → 2 → 3:1 + 5 = 6

两条一样长。但如果把 0→2 的权重改成 10,第一条就更好了。

**BFS 只数"边数",不看权重。** 在有权重的图里,边数少的路,总权重不一定小。所以 BFS 不再适用。

## 2. Dijkstra 的想法

**只适用于权重非负**的图(为什么,第 4 节解释)。

维护一个 `dist` 数组:`dist[i]` 表示"目前已知从起点到 i 的最小权重和"。

想法很简单:

> 每次从"还没确定"的点里,挑出 `dist` 最小的那个。它的值**已经不可能更小了**,可以确定下来。然后用它去更新它的邻居。

为什么"已经不可能更小"?因为所有边的权重都是非负的。从任何其他尚未确定的点绕过去,只会更远或持平,不会更近。这是 Dijkstra 成立的关键。

```text
起点 0,dist = [0, 无穷, 无穷, 无穷]

确定 0(dist 0)。更新邻居:1 -> 4,2 -> 1
  dist = [0, 4, 1, 无穷]        未确定里最小的是 2
确定 2(dist 1)。更新邻居:3 -> 1+5 = 6
  dist = [0, 4, 1, 6]           未确定里最小的是 1
确定 1(dist 4)。更新邻居:3 -> min(6, 4+2) = 6
  dist = [0, 4, 1, 6]
确定 3(dist 6)。全部完成。
```

## 3. 用优先队列实现

朴素做法:每次从头扫一遍找最小的 `dist`,一次 `O(V)`,总共 `O(V²)`(V 是点数)。

用第 34 课的**小顶堆**来取最小值,每次 `O(log V)`,这是标准实现:

```cpp
// file: dijkstra.cpp
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <tuple>
#include <vector>

const long long INF = std::numeric_limits<long long>::max();

std::vector<long long> dijkstra(const std::vector<std::vector<std::pair<int, int>>>& adj, int start) {
    std::vector<long long> dist(adj.size(), INF);
    std::priority_queue<std::pair<long long, int>, std::vector<std::pair<long long, int>>, std::greater<>> pq;

    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [d, node] = pq.top();
        pq.pop();
        if (d > dist[node]) {
            continue;
        }
        for (auto [next, weight] : adj[node]) {
            if (dist[node] + weight < dist[next]) {
                dist[next] = dist[node] + weight;
                pq.push({dist[next], next});
            }
        }
    }
    return dist;
}

int main() {
    int n = 4;
    std::vector<std::vector<std::pair<int, int>>> adj(n);
    auto add_edge = [&](int a, int b, int w) {
        adj[a].push_back({b, w});
        adj[b].push_back({a, w});
    };
    add_edge(0, 1, 4);
    add_edge(0, 2, 1);
    add_edge(2, 3, 5);
    add_edge(1, 3, 2);

    std::vector<long long> dist = dijkstra(adj, 0);
    for (int i = 0; i < n; i++) {
        std::cout << "0 到 " << i << ":" << (dist[i] == INF ? -1 : dist[i]) << std::endl;
    }
    return 0;
}
```

运行结果:

```text
0 到 0:0
0 到 1:4
0 到 2:1
0 到 3:6
```

几个要点:

- **带权图的邻接表**:`vector<vector<pair<int, int>>>`,每个邻居存 `(点到谁的编号, 边的权重)`。
- `INF` 用 `std::numeric_limits<long long>::max()`,标准库提供的"这个类型能装的最大值",代表"还没到达过"。用 `long long` 是因为权重累加很容易超过 `int`(第 2 课)。
- 堆里存 `(距离, 点)`,`std::pair` 的比较是"先比第一个,再比第二个",所以堆按**距离**排序,正是我们要的。
- `std::greater<>` 尖括号里不写类型,让它自己推断,这样能少写一大串。
- **`if (d > dist[node]) continue;` 这一句是关键。** 同一个点可能被多次压进堆(每次发现更短的路都压一次)。堆里可能留着它的旧的、较大的距离值。取出来时,如果这个 `d` 已经比记录的 `dist[node]` 大,说明它是过期信息,直接跳过。少了这一句,算法还是对的,但会做大量无用功,慢很多。

## 4. 为什么不能有负权边

Dijkstra 的依据是:"权重非负 ⇒ 绕过尚未确定的点,不会更近"。有了负权边,这条就不成立了。

```text
    0 --2--> 1 --(-5)--> 3
    |_____________________|
              1

从 0 到 3:
直接走:1
走 0->1->3:2 + (-5) = -3   更近!
```

Dijkstra 会先确定 3(因为 `dist[3] = 1` 比 `dist[1] = 2` 小),之后就再也不看 3 了。但实际上经过 1 的那条路更近。所以 Dijkstra 给出错误答案。

**负权边的真实场景:** 优惠券、返利、坡度(下坡省油)。出现负权,就要换算法。

还有一种更糟的情况:**负权环**。一圈走下来总权重是负的,绕圈可以无限减小距离,"最短路径"根本没有意义。

```text
1 --(-3)--> 2 --1--> 1     绕一圈净 -2,可以无限绕
```

## 5. 有负权时怎么办:Bellman-Ford

**想法:** 不挑"最小的",而是**把所有边都放松一遍**,重复 V-1 轮(V 是点数)。"放松一条边"就是:检查 `dist[a] + w < dist[b]`,是就更新 `dist[b]`。

为什么 V-1 轮够:最短路径最多经过 V-1 条边(不然就有重复的点,即环;权重非负时去掉环不会变差)。每轮至少能让一条边的正确值传播出去,V-1 轮足够覆盖最长的情形。

再额外做一轮:如果还能更新,说明存在**负权环**。这是 Bellman-Ford 的一个附带好处——它能检测负权环。

```cpp
// file: bellman_ford.cpp
#include <iostream>
#include <limits>
#include <vector>

const long long INF = std::numeric_limits<long long>::max() / 4;

struct Edge {
    int from;
    int to;
    int weight;
};

bool has_negative_cycle(int n, const std::vector<Edge>& edges) {
    std::vector<long long> dist(n, INF);
    dist[0] = 0;

    for (int round = 0; round < n - 1; round++) {
        for (const auto& e : edges) {
            if (dist[e.from] != INF && dist[e.from] + e.weight < dist[e.to]) {
                dist[e.to] = dist[e.from] + e.weight;
            }
        }
    }
    for (const auto& e : edges) {
        if (dist[e.from] != INF && dist[e.from] + e.weight < dist[e.to]) {
            return true;
        }
    }
    return false;
}

int main() {
    std::vector<Edge> edges = {{0, 1, 2}, {1, 2, -5}, {0, 2, 1}};
    std::cout << "有负权环吗:" << has_negative_cycle(3, edges) << std::endl;

    std::vector<Edge> cycle{{0, 1, 1}, {1, 2, -3}, {2, 1, -1}};
    std::cout << "有负权环吗:" << has_negative_cycle(3, cycle) << std::endl;
    return 0;
}
```

运行结果:

```text
有负权环吗:0
有负权环吗:1
```

- `INF` 除以 4,是为了防止 `dist[e.from] + e.weight` 溢出(第 2 课)。这是用"无穷大"参与加法时的常见防护。
- 复杂度 `O(V × E)` 或写作 `O(VE)`,E 是边数。比 Dijkstra 慢,所以只在必须处理负权时用。

## 6. 其他情况

| 情况 | 用什么 |
| --- | --- |
| 权重非负,求单点到其他所有点 | **Dijkstra**(最常用) |
| 边权只有 0 和 1 | 0-1 BFS(用双端队列,`O(E)`,比 Dijkstra 快) |
| 有权重,有负权,需要检测负权环 | Bellman-Ford |
| 求**任意两点**之间的最短距离 | Floyd-Warshall,`O(V³)`,点少时(几百内)最简单 |
| 边权都相同(没权重,或都是 1) | **BFS**(第 35 课)就够了 |
| 树(无环连通图) | 不需要算法,两次 BFS/DFS 就行 |
| 有向无环图(DAG) | 按拓扑序(第 37 课)动态规划,`O(V + E)`,**允许负权** |

**Floyd-Warshall** 值得单独说一句:三重循环,五行代码,求出所有点对之间的距离。点数几百以内时,它比"跑 V 次 Dijkstra"更省事,而且常数小。

```cpp
// file: floyd.cpp
#include <iostream>
#include <limits>
#include <vector>

const long long INF = std::numeric_limits<long long>::max() / 4;

int main() {
    int n = 4;
    std::vector<std::vector<long long>> d(n, std::vector<long long>(n, INF));
    for (int i = 0; i < n; i++) {
        d[i][i] = 0;
    }
    auto set_edge = [&](int a, int b, long long w) {
        d[a][b] = w;
        d[b][a] = w;
    };
    set_edge(0, 1, 4);
    set_edge(0, 2, 1);
    set_edge(2, 3, 5);
    set_edge(1, 3, 2);

    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (d[i][k] != INF && d[k][j] != INF && d[i][k] + d[k][j] < d[i][j]) {
                    d[i][j] = d[i][k] + d[k][j];
                }
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << (d[i][j] == INF ? -1 : d[i][j]) << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}
```

运行结果:

运行结果:

```text
0 4 1 6 
4 0 5 2 
1 5 0 5 
6 2 5 0 
```

对照着验证几个值(边是 0—1 权重 4、0—2 权重 1、2—3 权重 5、1—3 权重 2):

- `d[0][3] = 6`:走 0→1→3 是 4+2 = 6,走 0→2→3 是 1+5 = 6,两条一样。
- `d[1][3] = 2`:有直连边,权重 2,不需要绕。
- `d[2][1] = 5`:走 2→0→1 是 1+4 = 5,比 2→3→1 的 5+2 = 7 近。

- 用**邻接矩阵**(第 35 课),因为要查任意两点。
- 三层循环,**最外层必须是 `k`**(中间点)。顺序写错,结果就错。这是 Floyd 最容易犯的错。
- `d[i][k] != INF` 的检查不能少,否则两个"无穷"相加会溢出。

## 7. 工程里怎么选

真实项目中,**很少从零写最短路**。更常见的情况:

- 路网导航:用专门的库(OSRM、Valhalla),它们在 Dijkstra 之上做了大量优化(A*、分层、预处理),因为纯 Dijkstra 在几百万节点的路网上还是太慢。
- 依赖解析、构建系统:用拓扑排序(第 37 课),不是最短路。
- 网络路由协议:距离向量协议(RIP)本质上就是 Bellman-Ford 的分布式版本。
- 游戏寻路:网格上用 A*(Dijkstra 加启发式,第 44 课会提)。

学这些算法的价值在于:**知道问题的难度边界,知道为什么导航软件不能秒答,知道库函数背后是什么。**

## 练习

1. 给 `dijkstra.cpp` 加上"输出具体路径":记录每个点是从哪个点更新来的,最后从终点倒着走回起点。
2. 用 Dijkstra 解第 35 课的网格迷宫,把每个格子的权重设成 1,和第 35 课的 BFS 结果对比。
3. 实现 0-1 BFS:边权只有 0 和 1,权重 0 的边从双端队列**前端**入队,权重 1 的从**后端**入队。证明它比 Dijkstra 快。
4. 实现 Floyd-Warshall,用它求"图中任意两点是否连通"(把权重当成 0/1)。
5. 把 `bellman_ford.cpp` 改成真的输出最短距离,而不只是检测负权环。
6. 思考:一个图有 100 万个点、300 万条边,Dijkstra 用优先队列实现,大约多少步?(提示:`O((V + E) log V)`。)

## 小结

- 边有权重时,BFS 不适用(它只数边数)。
- Dijkstra:每次确定 `dist` 最小的点,用它更新邻居。**要求权重非负**,因为"绕过未确定的点不会更近"这一步依赖非负。
- 用优先队列实现,`O((V + E) log V)`。堆里存 `(距离, 点)`,取出时要跳过过期的条目(`if (d > dist[node]) continue;`)。
- 负权边 → Bellman-Ford,`O(VE)`,还能检测负权环。
- 所有点对之间 → Floyd-Warshall,`O(V³)`,三层循环最外层是中间点 `k`。
- 权重全相同 → 还是用 BFS。
- 用"无穷大"参与加法时,记得防溢出(`INF / 4`,或先检查 `!= INF`)。
- 真实项目用库,但要知道背后的算法和它的限制。
