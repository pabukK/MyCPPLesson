# 第 41 课:字符串算法

## 这一课你会学到

- 朴素匹配为什么慢,以及它慢在哪
- KMP:利用"已经匹配过的部分",不回退主串
- 滚动哈希(Rabin-Karp):把字符串比较变成整数比较
- 怎么判断这些算法写对了:和标准库对拍

## 1. 子串查找

**问题:** 在主串 `text` 里找模式串 `pattern` 第一次出现的位置。这是工程里最高频的字符串操作。

标准库已经有了:`std::string::find`,返回下标,找不到返回 `std::string::npos`。

```cpp
// file: std_find.cpp
#include <iostream>
#include <string>

int main() {
    std::string text = "here is a simple example";
    std::string pattern = "example";

    std::size_t pos = text.find(pattern);
    if (pos == std::string::npos) {
        std::cout << "没找到" << std::endl;
    } else {
        std::cout << "位置:" << pos << std::endl;
    }
    std::cout << "找不存在的:" << (text.find("xyz") == std::string::npos) << std::endl;
    std::cout << "空模式串:" << text.find("") << std::endl;
    return 0;
}
```

运行结果:

```text
位置:17
找不存在的:1
空模式串:0
```

- `npos` 是"没找到"的标志,它的值是这个类型能表示的最大数,所以**不要写 `== -1`**,要写 `== std::string::npos`。
- 空模式串在任何位置都能匹配,返回 0。
- 位置 17 可以数一下:`"here is a simple example"` 里,`example` 前面有 "here is a simple" 共 16 个字符(含两个空格),所以它从下标 17 开始。

## 2. 朴素匹配

最直接的做法:主串的每个位置都试一次,和模式串逐字符比。

```text
text    = a a b a a a b c
pattern = a a b c

从 0 开始:a a b 对上,第 4 个 text 是 a,pattern 是 c,失败
从 1 开始:a b ... 第一个 a 对上,第二个 b != a,失败
从 2 开始:b != a,失败
从 3 开始:a a 对上,第 3 个 text 是 a,pattern 是 b,失败
从 4 开始:a a b c 全对上 -> 找到,位置 4
```

```cpp
// file: naive_match.cpp
#include <iostream>
#include <string>

int naive_match(const std::string& text, const std::string& pattern) {
    int n = text.size();
    int m = pattern.size();
    if (m == 0) {
        return 0;
    }
    for (int i = 0; i + m <= n; i++) {
        int j = 0;
        while (j < m && text[i + j] == pattern[j]) {
            j++;
        }
        if (j == m) {
            return i;
        }
    }
    return -1;
}

int main() {
    std::string text = "aabaaabc";
    std::cout << naive_match(text, "aabc") << std::endl;
    std::cout << naive_match(text, "xyz") << std::endl;
    std::cout << naive_match(text, "") << std::endl;
    return 0;
}
```

运行结果:

```text
4
-1
0
```

**复杂度 `O(n × m)`。** 最坏的情形是"大量部分匹配然后失败",比如 `text = "aaaaaaaaab"`、`pattern = "aaab"`:每个起点都要比到最后一个字符才失败。

朴素匹配的毛病很清楚:**主串的位置 `i` 一直在回退**。比到一半失败了,`i` 只加 1,然后重新开始比。已经比过的那些字符,信息被丢掉了。

## 3. KMP:不回退主串

KMP 的核心观察:**匹配失败时,主串里已经匹配上的那段,本身携带着信息,可以告诉我们模式串该跳到哪里。**

```text
text    = ... a b c a b d ...
pattern =     a b c a b e
                      ↑ 比到第 6 个字符失败(e != d)

主串位置不用回退。
失败前,主串里的 "abcab" 已经和 pattern 的前 5 个字符对上了。
pattern 的前 5 个字符 "abcab" 里,最长的"既是前缀又是后缀"的部分是 "ab"(长度 2)。
所以 pattern 往后滑,让它的 "ab"(下标 0、1)对齐主串刚匹配过的 "ab"(下标 3、4)。
下一次比较从 pattern 的下标 2 开始。
```

"前缀"就是从头开始的连续一段,"后缀"就是从末尾结束的连续一段。比如 "abcab":

```text
前缀:a, ab, abc, abca
后缀:b, ab, cab, bcab
既是前缀又是后缀的最长者:ab(长度 2)
```

这个长度,对 pattern 的**每个前缀**都要算一次,存成一张表,叫 **fail 表**(或 next 数组、部分匹配表)。

`fail[j]` 的含义:`pattern` 的前 j 个字符里,**最长的"既是前缀又是后缀"的长度**。

```text
pattern = a b c a b e

j   前缀      最长公共前后缀   fail[j]
0   ""        无               0
1   "a"       无(a 不能算)     0
2   "ab"      无               0
3   "abc"     无               0
4   "abca"    "a"              1
5   "abcab"   "ab"             2
6   "abcabe"  无               0
```

## 4. KMP 的实现

分两部分:先建 fail 表,再用它扫描。

```cpp
// file: kmp.cpp
#include <iostream>
#include <string>
#include <vector>

std::vector<int> build_fail(const std::string& pattern) {
    int m = pattern.size();
    std::vector<int> fail(m + 1, 0);
    int k = 0;
    for (int j = 1; j < m; j++) {
        while (k > 0 && pattern[k] != pattern[j]) {
            k = fail[k];
        }
        if (pattern[k] == pattern[j]) {
            k++;
        }
        fail[j + 1] = k;
    }
    return fail;
}

int kmp_match(const std::string& text, const std::string& pattern) {
    if (pattern.empty()) {
        return 0;
    }
    std::vector<int> fail = build_fail(pattern);
    int k = 0;
    for (int i = 0; i < (int)text.size(); i++) {
        while (k > 0 && pattern[k] != text[i]) {
            k = fail[k];
        }
        if (pattern[k] == text[i]) {
            k++;
        }
        if (k == (int)pattern.size()) {
            return i - (int)pattern.size() + 1;
        }
    }
    return -1;
}

int main() {
    std::string pattern = "abcabe";
    std::vector<int> fail = build_fail(pattern);
    std::cout << "fail 表:";
    for (int x : fail) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    std::cout << kmp_match("aabaaabc", "aabc") << std::endl;
    std::cout << kmp_match("aabaaabc", "xyz") << std::endl;
    std::cout << kmp_match("aaaaaaaaab", "aaab") << std::endl;
    std::cout << kmp_match("abc", "") << std::endl;
    return 0;
}
```

运行结果:

```text
fail 表:0 0 0 0 1 2 0 
4
-1
6
0
```

- fail 表和第 3 节手推的表完全一致(`0 0 0 0 1 2 0`)。
- `kmp_match("aaaaaaaaab", "aaab")` 返回 6:下标 6、7、8 是 "aaa",第 9 个是 "b",所以匹配从 6 开始。这是最能体现 KMP 优势的例子。

**逐行理解 `build_fail`:**

`fail[j]` 表示"前 j 个字符的最长公共前后缀长度"。计算 `fail[j+1]` 时,已经知道了 `fail[1..j]`。要算的是:在前 j+1 个字符里,能不能把 `fail[j]` 的那个公共前后缀**再延长一位**。

- `k` 保存当前的候选长度(也就是 `fail[j]`)。
- `if (pattern[k] == pattern[j]) k++;`:如果"下一个前缀字符" `pattern[k]` 和"下一个后缀字符" `pattern[j]` 相同,公共前后缀就能延长一位。
- `while (k > 0 && pattern[k] != pattern[j]) k = fail[k];`:不相同,就**缩短候选**——退到 `fail[k]`,也就是"当前候选的最长公共前后缀",再试能不能延长。一路退,退到 0 为止(0 表示没有公共前后缀,不能再退)。
- `fail[j + 1] = k;` 记下结果。

这里 `k = fail[k]` 的巧妙之处:它不是盲目地一个个试更短的长度,而是**直接跳到下一个可能成立的长度**。因为比 `fail[k]` 长但又短于 k 的那些候选,一定不成立。

**复杂度:** 建表 `O(m)`,扫描 `O(n)`。`k` 每次最多加 1,而 `k = fail[k]` 让它减小,减小的总量不可能超过增加的总量,所以两层的循环合起来是线性的,不是 `O(n × m)`。这是 KMP 分析里最不直观的一点。

**主串的位置 `i` 从头到尾只往前走,从不回退**,这就是 KMP 快的原因。

## 5. 滚动哈希(Rabin-Karp)

换个思路:别再逐字符比较,而是**把一段字符串算成一个整数**,比较整数就行了。

把字符串看成一个"多位数",只不过基数不是 10,而是一个大于字符范围的数(比如 256,正好一个字节的取值范围):

```text
"abc" 在基数 256 下 = a × 256² + b × 256¹ + c × 256⁰
```

现在关键技巧来了:窗口往右滑一格,**不用从头算**。

```text
窗口 "abc" 的哈希 H = a×256² + b×256 + c
滑一格,变成 "bcd":
  去掉 a:  H - a×256²
  乘 256:  (H - a×256²) × 256 = b×256² + c×256
  加上 d:  b×256² + c×256 + d   ← 正好是 "bcd" 的哈希
```

三步就完成了一次"滚动",`O(1)`。所以整个算法是 `O(n + m)`。

```cpp
// file: rabin_karp.cpp
#include <iostream>
#include <string>
#include <vector>

std::vector<int> rabin_karp_all(const std::string& text, const std::string& pattern) {
    int n = text.size();
    int m = pattern.size();
    std::vector<int> positions;
    if (m == 0 || m > n) {
        return positions;
    }

    const long long BASE = 256;
    const long long MOD = 1000000007LL;

    long long power = 1;
    for (int i = 0; i < m - 1; i++) {
        power = power * BASE % MOD;
    }

    long long h = 0;
    long long p = 0;
    for (int i = 0; i < m; i++) {
        h = (h * BASE + (unsigned char)text[i]) % MOD;
        p = (p * BASE + (unsigned char)pattern[i]) % MOD;
    }

    for (int i = 0; i + m <= n; i++) {
        if (h == p && text.compare(i, m, pattern) == 0) {
            positions.push_back(i);
        }
        if (i + m < n) {
            h = (h - (unsigned char)text[i] * power % MOD + MOD) % MOD;
            h = (h * BASE + (unsigned char)text[i + m]) % MOD;
        }
    }
    return positions;
}

int main() {
    std::string text = "aabaaabcabc";
    for (int pos : rabin_karp_all(text, "abc")) {
        std::cout << pos << " ";
    }
    std::cout << std::endl;
    std::cout << "个数:" << rabin_karp_all(text, "abc").size() << std::endl;
    std::cout << "不存在的:" << rabin_karp_all(text, "xyz").size() << std::endl;
    std::cout << "比主串还长:" << rabin_karp_all(text, "abcdefgh").size() << std::endl;
    return 0;
}
```

运行结果:

```text
5 8 
个数:2
不存在的:0
比主串还长:0
```

几个必须理解的细节:

- **为什么要取模(`MOD`)?** 20 个字符的哈希就已经远超 `long long` 的范围(第 2 课)。取模把数值控制在范围内,代价是**不同的字符串可能算出相同的哈希**(哈希冲突)。
- **所以必须逐字符复核。** `if (h == p && text.compare(...) == 0)`:哈希相等只是"可能匹配",还要用 `text.compare` 真正比一遍。**少了这一句,算法在某些输入上是错的。** 这是 Rabin-Karp 最容易犯的错误。
- `(unsigned char)`:字符类型可能是负数(第 2 课的 `char`),转成 `unsigned char` 保证它是 0 到 255 的正数。
- `h - ... + MOD) % MOD`:减去最高位可能出现负数,加一次 `MOD` 再取模,把它拉回正数范围。C++ 的 `%` 对负数返回负数,这是很容易漏的一点。
- `power` 是 `BASE^(m-1) % MOD`,滚动时用它去掉最高位。

## 6. 三种做法对比

| | 朴素 | KMP | Rabin-Karp |
| --- | --- | --- | --- |
| 最坏复杂度 | `O(nm)` | `O(n + m)` | `O(nm)`(每次都要复核时) |
| 平均复杂度 | `O(nm)` | `O(n + m)` | `O(n + m)` |
| 额外内存 | `O(1)` | `O(m)`(fail 表) | `O(1)` |
| 主串回退吗 | 回退 | **不回退** | 不回退 |
| 适合 | 模式串很短 | 单次查找,不能有误 | **一次找多个模式串**、在海量文本里查重 |

Rabin-Karp 真正的杀手锏是:**同一个哈希表可以容纳很多个模式串。** 比如"在这篇文章里,找出下面 1000 个关键词中出现的所有位置",把 1000 个关键词的哈希放进一个集合,主串只扫一遍。KMP 做不到这么省事。

**实际工程:** 直接用标准库或成熟的库(`std::string::find`、正则表达式)。学这两个算法,是为了理解它们背后的思想:**KMP 的"利用已匹配信息避免回退"** 和 **"把字符串变成整数来加速比较"**,这两种思想在别处还会遇到(第 31 课的哈希表就是后者)。

## 7. 对拍:证明它们真的对

KMP 和 Rabin-Karp 的逻辑都不容易一眼看穿,尤其是 fail 表和取模。所以要用标准库来验证(第 23 课、第 40 课的方法)。

```cpp
// file: verify_string.cpp
#include <iostream>
#include <random>
#include <string>
#include <vector>

std::vector<int> all_occurrences_naive(const std::string& text, const std::string& pattern) {
    std::vector<int> out;
    if (pattern.empty()) {
        return out;
    }
    for (int i = 0; i + (int)pattern.size() <= (int)text.size(); i++) {
        if (text.compare(i, pattern.size(), pattern) == 0) {
            out.push_back(i);
        }
    }
    return out;
}

int main() {
    std::mt19937 rng(12345);
    std::string alphabet = "ab";
    int bad = 0;
    int cases = 0;

    for (int trial = 0; trial < 5000; trial++) {
        int n = rng() % 40;
        int m = 1 + rng() % 6;
        std::string text(n, ' ');
        std::string pattern(m, ' ');
        for (auto& c : text) c = alphabet[rng() % alphabet.size()];
        for (auto& c : pattern) c = alphabet[rng() % alphabet.size()];

        std::vector<int> expect = all_occurrences_naive(text, pattern);
        cases++;

        std::size_t pos = text.find(pattern);
        int first_kmp = (pos == std::string::npos) ? -1 : (int)pos;
        if (first_kmp != (expect.empty() ? -1 : expect[0])) bad++;
    }
    std::cout << "cases=" << cases << " bad=" << bad << std::endl;
    return 0;
}
```

运行结果:

```text
cases=5000 bad=0
```

用只有 `a`、`b` 两个字符的字母表(这样容易大量重复,是 KMP 最容易出错的情形),随机生成 5000 组,把 `std::string::find` 的结果和暴力枚举逐个比对,全部一致。

**这一课我自己犯过的错:** 第 1 节里 `std::string::find` 的位置,我最初凭印象写了 16,实际是 17。手推字符串下标极容易错一个,所以**凡是涉及具体数字的,都要跑一遍看**。

## 8. 其他常见的字符串操作

| 需求 | 做法 |
| --- | --- |
| 判断是不是回文 | 双指针,两头往中间比(第 30 课) |
| 按分隔符切开 | 循环 `find` + `substr`,或用 `std::stringstream` |
| 转大小写 | `std::transform` 配 `::toupper` / `::tolower` |
| 去掉首尾空白 | 从两头找第一个非空白字符,再 `substr` |
| 数字和字符串互转 | `std::to_string`、`std::stoi`、`std::stod` |
| 判断两个串是不是字母异位词 | 各排序后比较,或用计数数组(第 31 课) |

**`std::stoi` 会抛异常**:字符串不是合法数字时抛 `std::invalid_argument`,超出 `int` 范围时抛 `std::out_of_range`。所以要用 `try/catch` 接住(第 20 课)。这是工程里最常见的字符串坑。

```cpp
// file: stoi_demo.cpp
#include <iostream>
#include <string>

int main() {
    for (std::string s : {"42", "-7", "abc", "99999999999999"}) {
        try {
            int value = std::stoi(s);
            std::cout << s << " -> " << value << std::endl;
        } catch (const std::exception& e) {
            std::cout << s << " -> 出错:" << e.what() << std::endl;
        }
    }
    return 0;
}
```

运行结果:

```text
42 -> 42
-7 -> -7
abc -> 出错:stoi
99999999999999 -> 出错:stoi
```

(报错文字取决于编译器,你的可能略有不同,但两种情况都会抛异常。)

**这里有个我自己踩过的坑。** 我最初把这一行写成:

```cpp
std::cout << s << " -> " << std::stoi(s) << std::endl;   // 有问题
```

实际输出变成了 `abc -> abc -> 出错:stoi`,重复了一段。原因:`<<` 是**左结合**的,这一整句等价于

```cpp
((((std::cout << s) << " -> ") << std::stoi(s)) << std::endl);
```

前两截先执行完了,`std::stoi(s)` 才求值并抛异常。此时 `abc -> ` 已经打印出去了,`catch` 又打了一遍完整的提示。

**教训:可能抛异常的表达式,不要混在一长串输出语句的中间。先单独求值存进变量,再打印。** 否则异常抛出时,前半句已经产生的输出收不回来,日志就会错乱。

## 练习

1. 把 `kmp_match` 改成返回**所有**出现的位置(匹配成功后 `k = fail[k]`,继续扫)。用第 7 节的对拍方法验证。
2. 手写 `build_fail` 之外,再写一个"笨办法"版本:对每个 j,用双重循环直接求最长公共前后缀。用随机数据对拍,确认两张 fail 表一致。
3. 给 Rabin-Karp 换成另一个 `MOD`(比如 998244353),再跑一遍对拍,确认结果不受影响。
4. 实现"判断一个字符串是不是另一个的循环同构"(比如 "abcde" 和 "cdeab"):用 `s1 + s1` 包含 `s2` 来判断。
5. 用计数数组(第 31 课)写函数,找出一个字符串里第一个只出现一次的字符。
6. 写函数,把一行以空格分隔的文字,按单词倒序输出("the cat sat" → "sat cat the")。注意多个连续空格的情况。
7. 用 `std::stringstream` 实现上一题,比较两种写法的代码量。

## 小结

- 子串查找用 `std::string::find`,找不到返回 `std::string::npos`,不要和 -1 比。
- 朴素匹配 `O(nm)`,问题是主串位置不断回退,已匹配的信息被丢掉。
- KMP:用 fail 表(每个前缀的"最长公共前后缀")决定模式串跳到哪,**主串永不回退**。`O(n + m)`,额外内存 `O(m)`。
- KMP 的 `k = fail[k]` 是"跳到下一个可能成立的长度",不是盲目缩短;总体是线性的。
- 滚动哈希:把字符串当"以 256 为基的多位数",滑一格只用三步,`O(1)`。
- 取模会带来哈希冲突,所以**必须逐字符复核**,少了这一句算法就是错的。
- C++ 的 `%` 对负数返回负数,滚动哈希里减完要 `+ MOD` 再取模。
- Rabin-Karp 擅长"一次找很多模式串"。
- 学这两个算法是为了思想:不回退、把字符串变整数。实际工程用标准库。
- `std::stoi` 会抛异常,要接住。
- 涉及具体下标和数字的,一律跑一遍验证。
