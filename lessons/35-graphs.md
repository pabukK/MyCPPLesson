# 第 35 课:图:深度优先与广度优先

## 这一课你会学到

- 图是什么,怎么在代码里表示
- 深度优先搜索(DFS):一条路走到底,走不通再回头
- 广度优先搜索(BFS):一层一层往外扩散
- 用它们解决连通、找环、分层的问题

## 1. 图是什么

树(第 33 课)是"有层级、不分叉回去"的结构。图更一般:**一堆点,和一些连接这些点的边**。

```text
    0 --- 1
    |     |
    2 --- 3 --- 4
```

图分两类:

- **无向图**:边没有方向。0—1 表示 0 连着 1,1 也连着 0。像道路,双向通行。
- **有向图**:边有方向。0 → 1 只表示从 0 能到 1。像单行道、网页之间的链接。

树其实是图的一种特殊情况:没有环的连通图。

图在工程里到处都是:道路网络、社交关系、网页链接、依赖关系(哪个包依赖哪个包)、电路。

## 2. 怎么表示:邻接表

最常用的表示法是**邻接表**:对每个点,记下"和它直接相连的点有哪些"。

```text
0: [1, 2]
1: [0, 3]
2: [0, 3]
3: [1, 2, 4]
4: [3]
```

用 `vector<vector<int>>` 就行:

```cpp
// file: graph_build.cpp
#include <iostream>
#include <vector>

int main() {
    int n = 5;
    std::vector<std::vector<int>> adj(n);

    auto add_edge = [&](int a, int b) {
        adj[a].push_back(b);
        adj[b].push_back(a);
    };
    add_edge(0, 1);
    add_edge(0, 2);
    add_edge(1, 3);
    add_edge(2, 3);
    add_edge(3, 4);

    for (int i = 0; i < n; i++) {
        std::cout << i << ": ";
        for (int next : adj[i]) {
            std::cout << next << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}
```

运行结果:

```text
0: 1 2 
1: 0 3 
2: 0 3 
3: 1 2 4 
4: 3 
```

- `adj[i]` 是点 i 的邻居列表。
- 无向图的一条边,要**存两次**(`add_edge` 里两句)。有向图只存一次:`adj[a].push_back(b);`。
- 加一条边 `O(1)`。

另一种表示是**邻接矩阵**:`bool adj[n][n]`,`adj[a][b]` 表示 a 和 b 之间有没有边。它占 `O(n²)` 内存,但"判断两点是否相连"是 `O(1)`。

| | 邻接表 | 邻接矩阵 |
| --- | --- | --- |
| 内存 | `O(点数 + 边数)` | `O(点数²)` |
| 遍历一个点的所有邻居 | `O(邻居数)` | `O(点数)`,要扫一整行 |
| 判断两点相连 | `O(邻居数)` | `O(1)` |

**边少的图(稀疏图)用邻接表**,这是绝大多数情况。点数几百以内、又频繁判断"是否相连"时,才用矩阵。

## 3. 遍历图:必须记住"走过"

树只有一个父亲,顺着走不会绕回来。图有环:0 → 1 → 3 → 2 → 0 → ……会无限循环。

所以遍历图,**必须记录哪些点已经访问过**。这个记录通常叫 `visited`。

## 4. 深度优先搜索(DFS)

**想法:** 从一个点出发,**沿着一条路一直走到底**;走不通了,退回上一个岔路口,换另一条路。像走迷宫时贴着墙走,遇到死胡同就回头。

这正好是第 26 课递归做的事:递归的每一层,就是"当前在哪个点"。

```text
    0 --- 1
    |     |
    2 --- 3 --- 4

从 0 开始 DFS:
访问 0 -> 去邻居 1 -> 访问 1 -> 去邻居 0(已访问,跳过) -> 去邻居 3
-> 访问 3 -> 去邻居 1(跳过) -> 去邻居 2 -> 访问 2 -> 邻居 0、3 都跳过,回头
-> 3 的下一个邻居 4 -> 访问 4 -> 没有未访问的邻居,回头
顺序:0 1 3 2 4
```

```cpp
// file: dfs.cpp
#include <iostream>
#include <vector>

void dfs(int node, const std::vector<std::vector<int>>& adj, std::vector<bool>& visited, std::vector<int>& order) {
    visited[node] = true;
    order.push_back(node);
    for (int next : adj[node]) {
        if (!visited[next]) {
            dfs(next, adj, visited, order);
        }
    }
}

int main() {
    std::vector<std::vector<int>> adj(5);
    auto add_edge = [&](int a, int b) {
        adj[a].push_back(b);
        adj[b].push_back(a);
    };
    add_edge(0, 1);
    add_edge(0, 2);
    add_edge(1, 3);
    add_edge(2, 3);
    add_edge(3, 4);

    std::vector<bool> visited(adj.size(), false);
    std::vector<int> order;
    dfs(0, adj, visited, order);

    for (int x : order) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

运行结果:

```text
0 1 3 2 4 
```

- `std::vector<bool>` 是标准库提供的"一串真假值",专门用来做 `visited`。它占的内存比普通 `vector<char>` 更省,但用法有一点特殊(第 43 课会讲),用它做访问标记完全没问题。
- 参数 `const std::vector<std::vector<int>>&` 是只读引用(第 10 课),图很大,不要复制。
- 每个点访问一次,每条边看两次(无向图存了两份),所以是 `O(点数 + 边数)`。

**如果图不连通**(有几块彼此到不了),从 0 出发的 DFS 只能到达 0 所在的那一块。要遍历全部,得在外层再套一个循环:

```cpp
// file: dfs_components.cpp
#include <iostream>
#include <vector>

int dfs_count(int node, const std::vector<std::vector<int>>& adj, std::vector<bool>& visited) {
    visited[node] = true;
    int size = 1;
    for (int next : adj[node]) {
        if (!visited[next]) {
            size += dfs_count(next, adj, visited);
        }
    }
    return size;
}

int main() {
    std::vector<std::vector<int>> adj(7);
    auto add_edge = [&](int a, int b) {
        adj[a].push_back(b);
        adj[b].push_back(a);
    };
    add_edge(0, 1);
    add_edge(1, 2);
    add_edge(3, 4);
    add_edge(5, 6);

    std::vector<bool> visited(adj.size(), false);
    int components = 0;
    std::vector<int> sizes;
    for (int i = 0; i < (int)adj.size(); i++) {
        if (!visited[i]) {
            components++;
            sizes.push_back(dfs_count(i, adj, visited));
        }
    }

    std::cout << "有几块:" << components << std::endl;
    for (int s : sizes) {
        std::cout << s << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

运行结果:

```text
有几块:3
3 2 2 
```

这个"数有几块"的写法,是 DFS 最常见的模式:外层循环找未访问的点,内层 DFS 把整块扫掉。

## 5. 递归 DFS 太深会崩

第 26 课说过:递归太深,栈会溢出。图的 DFS 递归深度等于路径长度。**如果图是一条 10 万个点的长链,从链头开始 DFS,就会崩。**

这在处理网格、道路网这类大图时是真的会遇到的。解决办法:把递归改成**显式用栈**(第 32 课)模拟:

```cpp
// file: dfs_iterative.cpp
#include <iostream>
#include <stack>
#include <vector>

std::vector<int> dfs_iterative(int start, const std::vector<std::vector<int>>& adj) {
    std::vector<bool> visited(adj.size(), false);
    std::vector<int> order;
    std::stack<int> st;
    st.push(start);

    while (!st.empty()) {
        int node = st.top();
        st.pop();
        if (visited[node]) {
            continue;
        }
        visited[node] = true;
        order.push_back(node);
        for (int next : adj[node]) {
            if (!visited[next]) {
                st.push(next);
            }
        }
    }
    return order;
}

int main() {
    std::vector<std::vector<int>> adj(100000);
    for (int i = 0; i + 1 < 100000; i++) {
        adj[i].push_back(i + 1);
        adj[i + 1].push_back(i);
    }

    std::cout << "递归 DFS 深度:" << 100000 << ",改成栈模拟" << std::endl;
    std::vector<int> order = dfs_iterative(0, adj);
    std::cout << "访问到的点数:" << order.size() << std::endl;
    std::cout << "前几个:" << order[0] << " " << order[1] << " " << order[2] << std::endl;
    return 0;
}
```

运行结果:

```text
递归 DFS 深度:100000,改成栈模拟
访问到的点数:100000
前几个:0 1 2
```

- 用 `std::stack` 代替递归,栈在堆上(第 11 课),不受 8 MB 的栈大小限制。
- **注意 `if (visited[node]) continue;` 这一句。** 一个点可能被多次压进栈(从不同的邻居压入),所以取出时必须检查它是不是已经访问过。少了这一句,同一个点会被重复处理,结果就错了。这是非递归 DFS 最容易犯的错。

## 6. 广度优先搜索(BFS)

**想法:** 像往水里扔石头,波纹一圈圈扩散。先访问离起点 1 步的所有点,再访问 2 步的所有点,然后是 3 步……**一层一层往外**。

用**队列**(第 32 课)来实现:新发现的点排到队尾,处理时从队首取。这样先发现的先处理,正好保证一层一层。

```text
    0 --- 1
    |     |
    2 --- 3 --- 4

从 0 开始 BFS:
队列 [0]                     取出 0,访问,把邻居 1、2 入队
队列 [1, 2]                  取出 1,访问,邻居 3 入队(0 已访问)
队列 [2, 3]                  取出 2,访问,邻居 3 已在队里/已访问,不入
队列 [3]                     取出 3,访问,邻居 4 入队
队列 [4]                     取出 4,访问
顺序:0 1 2 3 4
```

```cpp
// file: bfs.cpp
#include <iostream>
#include <queue>
#include <vector>

std::vector<int> bfs(int start, const std::vector<std::vector<int>>& adj, std::vector<int>& distance) {
    std::vector<bool> visited(adj.size(), false);
    std::vector<int> order;
    std::queue<int> q;

    q.push(start);
    visited[start] = true;
    distance[start] = 0;

    while (!q.empty()) {
        int node = q.front();
        q.pop();
        order.push_back(node);

        for (int next : adj[node]) {
            if (!visited[next]) {
                visited[next] = true;
                distance[next] = distance[node] + 1;
                q.push(next);
            }
        }
    }
    return order;
}

int main() {
    std::vector<std::vector<int>> adj(5);
    auto add_edge = [&](int a, int b) {
        adj[a].push_back(b);
        adj[b].push_back(a);
    };
    add_edge(0, 1);
    add_edge(0, 2);
    add_edge(1, 3);
    add_edge(2, 3);
    add_edge(3, 4);

    std::vector<int> distance(adj.size(), -1);
    std::vector<int> order = bfs(0, adj, distance);

    for (int x : order) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    for (int i = 0; i < (int)adj.size(); i++) {
        std::cout << "0 到 " << i << " 的距离:" << distance[i] << std::endl;
    }
    return 0;
}
```

运行结果:

```text
0 1 2 3 4 
0 到 0 的距离:0
0 到 1 的距离:1
0 到 2 的距离:1
0 到 3 的距离:2
0 到 4 的距离:3
```

**关键点:`visited[next] = true` 要在入队的时候设,不是出队的时候。** 如果只在出队时标记,同一个点会被多个邻居重复入队,队列会膨胀,速度变慢(结果还是对的,但浪费)。这是 BFS 最常见的写法错误。

## 7. BFS 最重要的性质:最短路径

在**每条边代价都相同**的图里,BFS 第一次到达某个点时,走的一定是**边数最少**的路。

为什么:因为 BFS 是按"距离从小到大"一层层访问的。距离 1 的点全部处理完,才轮到距离 2 的。所以第一个到达某点的那条路,不可能比它更长的路晚被发现。

```text
    0 --- 1
    |     |
    2 --- 3 --- 4

从 0 到 3 有两条路:0-1-3(2 步)和 0-2-3(2 步)
BFS 先处理 1 和 2(距离 1),所以 3 第一次被发现时,距离是 2。
如果先走 0-1-3-2 那条更绕的路,2 会在距离 3 时才被发现,但 BFS 不会这样。
```

**DFS 没有这个性质。** 上面那个例子里,DFS 的顺序是 `0 1 3 2 4`,它先到了 3,但那是"碰巧"。DFS 可能先走一条很长的弯路才到达某个点。

所以:**要求"最少几步"、"最短路径"(边权相同),用 BFS。只要求"能不能到"、"把所有点走一遍",DFS 和 BFS 都行,DFS 代码更短。**

真实的**带权**最短路径(每条边的代价不同),BFS 和 DFS 都不够,要用第 36 课的 Dijkstra。

## 8. 在网格上使用

**网格图**是图最常见的形态:一个二维数组,每个格子是一个点,和上下左右四个格子相连。迷宫、地图、图像里的连通区域,都是网格图。

```text
网格 3x4:
(0,0) (0,1) (0,2) (0,3)
(1,0) (1,1) (1,2) (1,3)
(2,0) (2,1) (2,2) (2,3)

格子 (r, c) 的四个邻居:(r-1,c) (r+1,c) (r,c-1) (r,c+1)
```

不用真的建 `adj`,当场算邻居就行。判断"在不在网格里"用 `0 <= r && r < rows`。

**问题:** 一个 0/1 网格,1 是墙,0 是可走。求从左上角到右下角最少走几步。走不通返回 -1。

```cpp
// file: grid_bfs.cpp
#include <iostream>
#include <queue>
#include <tuple>
#include <vector>

int shortest_path(const std::vector<std::vector<int>>& grid) {
    int rows = grid.size();
    if (rows == 0 || grid[0].size() == 0) {
        return -1;
    }
    int cols = grid[0].size();
    if (grid[0][0] == 1 || grid[rows - 1][cols - 1] == 1) {
        return -1;
    }

    std::vector<std::vector<int>> dist(rows, std::vector<int>(cols, -1));
    std::queue<std::tuple<int, int>> q;
    q.push({0, 0});
    dist[0][0] = 0;

    const int dr[4] = {-1, 1, 0, 0};
    const int dc[4] = {0, 0, -1, 1};

    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();
        for (int d = 0; d < 4; d++) {
            int nr = r + dr[d];
            int nc = c + dc[d];
            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
                continue;
            }
            if (grid[nr][nc] == 1 || dist[nr][nc] != -1) {
                continue;
            }
            dist[nr][nc] = dist[r][c] + 1;
            q.push({nr, nc});
        }
    }
    return dist[rows - 1][cols - 1];
}

int main() {
    std::vector<std::vector<int>> grid = {
        {0, 0, 1, 0},
        {0, 0, 0, 0},
        {1, 1, 0, 1},
        {0, 0, 0, 0}
    };
    std::cout << "最少步数:" << shortest_path(grid) << std::endl;

    std::vector<std::vector<int>> blocked = {
        {0, 1},
        {1, 0}
    };
    std::cout << "被墙挡住:" << shortest_path(blocked) << std::endl;

    std::vector<std::vector<int>> single = {{0}};
    std::cout << "只有一个格子:" << shortest_path(single) << std::endl;
    return 0;
}
```

运行结果:

```text
最少步数:6
被墙挡住:-1
只有一个格子:0
```

几个要点:

- `dr` 和 `dc` 两个数组,把四个方向写成表。这是网格遍历的标准写法,比写四个 `if` 清爽得多,也少出错。
- `dist` 同时充当 `visited`:值不是 -1 就说明到过。省掉一个数组。
- 用 `std::tuple<int, int>` 存坐标,配合结构化绑定 `auto [r, c]`(第 30 课)。第 31 课说过,坐标也可以用 `Point` 结构体加自定义哈希当键。
- 三个测试:正常、被墙挡住、只有一个格子(起点就是终点,0 步)。**起点或终点本身就是墙**,要单独检查,这是最容易漏的边界。

## 9. DFS 还是 BFS?

| | DFS | BFS |
| --- | --- | --- |
| 用什么 | 递归(或栈) | 队列 |
| 走法 | 一条路走到底,再回头 | 一层一层扩散 |
| 能求最短路径(边权相同) | **不能** | **能** |
| 内存 | 递归深度,或栈的大小 | 队列的大小,可能很宽 |
| 适合 | 判断连通、找环、拓扑排序、枚举所有可能(第 39 课回溯) | 最短步数、分层、扩散类问题 |

我实测过本课开头说的情况:一条 10 万个点的链,用递归 DFS 从链头开始,**没有优化编译(`-O0`)时,程序栈溢出崩溃**(退出码 139,`Segmentation fault`);**开了优化(`-O2`)就不崩**。和第 26 课、第 32 课的情况一样:优化有时能把递归变成循环,但这个行为你不该依赖,它取决于编译器、编译选项和每个函数帧的大小。

换成第 4 节的显式栈版本,两种编译方式下 10 万个点都正常访问完,因为栈放在了堆上(第 11 课),不受 8 MB 的限制。

**经验:** 数据量可能很大(网格、地图、几十万节点)时,用显式栈或 BFS,不要用递归。

## 练习

1. 用 DFS 判断无向图里有没有环。(提示:如果从一个点出发,遇到的邻居已访问、而且它不是自己的父亲,说明有环。)
2. 给一个网格,数一数有几块连通的区域(第 18 课的"岛屿数量")。
3. 用 BFS 求"从起点到所有点的最短距离",并把距离输出成表格。
4. 双向 BFS:同时从起点和终点开始扩散,相遇时停止。想想为什么这样能省一半的时间。
5. 用第 31 课的 `Point` 当键、`unordered_set` 存 `visited`,重写 `grid_bfs`,比较两者的速度。
6. 迷宫问题:给定一个 20×20 的迷宫,输出从入口到出口的具体路径(不只是步数)。(提示:BFS 时记录每个点是从哪个点来的,最后从终点倒着走回起点。)

## 小结

- 图 = 点 + 边。无向图的边要存两份,有向图存一份。
- 邻接表 `vector<vector<int>>` 最常用,内存 `O(点 + 边)`;邻接矩阵适合稠密图或频繁判断两点相连。
- 遍历图**必须**记录 `visited`,否则会绕环无限循环。
- DFS:递归(或显式栈),"一条路走到底"。`O(点 + 边)`。适合连通性、找环、枚举。
- BFS:队列,"一层一层扩散"。`O(点 + 边)`。**在边权相同的图里,第一次到达就是最短路径。**
- BFS 要在**入队时**标记已访问;非递归 DFS 要在**出栈时**检查是否已访问。
- 网格图不用真的建图,用 `dr`、`dc` 两个数组算四个邻居,并检查是否越界。
- 图很大时别用递归 DFS,会栈溢出。
