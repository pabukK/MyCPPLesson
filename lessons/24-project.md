# 第 24 课:综合项目:成绩管理程序

## 这一课你会学到

- 把前 23 课的内容合在一起,做一个完整的小程序
- 怎么从需求出发,一步步设计
- 读懂并运行一个多文件、带测试的 CMake 项目

完整代码已经放在 `project/grade_manager/` 里,每一个文件都在 WSL 里编译、运行并检查过。本课带你读懂它,并动手扩展。

## 1. 需求

做一个命令行的成绩管理程序,能:

1. 添加学生(名字和分数)
2. 删除学生
3. 按名字查询
4. 按分数从高到低排名
5. 求平均分
6. 保存到文件、从文件读取

还要求:输入有误时不能崩溃,要给出提示。

## 2. 先设计,再写代码

动手前,先想清楚有哪些"东西",各自负责什么:

| 东西 | 职责 | 用到的知识 |
| --- | --- | --- |
| `Student` 类 | 一个学生:名字、分数。保证数据合法 | 第 14 课:类、构造函数、`private`;第 20 课:异常 |
| `Gradebook` 类 | 一堆学生:增删查、排名、平均分、存取文件 | 第 18 课:`vector`;第 19 课:算法与 lambda;第 20 课:文件 |
| `main.cpp` | 菜单,读用户输入,调用 `Gradebook` | 第 5 课:循环;第 4 课:`switch` |

这样分的好处:**每个类只管一件事**。`Student` 不知道文件和菜单,`Gradebook` 不知道菜单怎么显示。将来想把命令行换成图形界面,只需要改 `main.cpp`。

## 3. 项目结构

```text
project/grade_manager/
├── CMakeLists.txt
├── include/
│   ├── student.h
│   └── gradebook.h
├── src/
│   ├── student.cpp
│   ├── gradebook.cpp
│   └── main.cpp
└── tests/
    └── test_gradebook.cpp
```

这就是第 21 课、第 22 课讲的规范结构:头文件放 `include`,源文件放 `src`,测试放 `tests`。

## 4. Student 类

打开 `include/student.h` 和 `src/student.cpp`。重点看构造函数:

```cpp
Student::Student(std::string n, int s) : name(n), score(s) {
    if (name.empty()) {
        throw std::invalid_argument("名字不能为空");
    }
    if (score < 0 || score > 100) {
        throw std::invalid_argument("分数必须在 0 到 100 之间");
    }
}
```

**构造函数负责保证:只要对象创建成功,里面的数据一定是合法的。** 不合法就抛出异常,对象根本创建不出来。这样,程序里其他地方拿到一个 `Student`,就不用再检查名字和分数了。这是第 14 课"用 `private` 保护数据"的思路的延伸。

(第 20 课说过 `catch` 的异常要接住。调用者怎么处理,下一节看 `main.cpp`。)

## 5. Gradebook 类

打开 `include/gradebook.h`,先看它对外承诺了什么:

```cpp
class Gradebook {
public:
    void add(const Student& s);
    bool remove(const std::string& name);
    const Student* find(const std::string& name) const;

    std::size_t size() const;
    double average() const;
    std::vector<Student> sorted_by_score() const;

    void save(const std::string& path) const;
    void load(const std::string& path);

private:
    std::vector<Student> students;
};
```

读头文件,就像读说明书:不用看实现,就知道这个类能做什么。

值得注意的几个设计:

**成员是 `std::vector<Student>`。** 第 15 课的"零法则":成员都是会自己管理内存的类型,所以这个类没有写析构函数、拷贝构造函数,也没有任何 `new` 和 `delete`。整个项目里一个 `new` 都没有。

**参数用 `const Student&`、`const std::string&`。** 第 10 课:大对象只读使用,用只读引用,避免复制。

**`find` 返回 `const Student*`。** 找到了返回地址,找不到返回 `nullptr`(第 9 课)。这里用指针而不是引用,正是因为引用不能为空,而"找不到"是正常情况。调用者必须先检查它是不是 `nullptr`。返回的是 `const`,调用者只能读,不能通过它改动成绩册里的数据。

**`add` 检查重名,重名时抛异常。** `remove` 找不到时返回 `false` 而不抛异常:"删一个不存在的人"是用户可能犯的常见小错,不是严重错误,所以用返回值表示就够了。第 20 课的建议:异常用来处理不应该发生的情况。

**`sorted_by_score` 返回的是一个排好序的副本。** 先复制 `students`,再对副本排序,原数据不变。返回 `vector` 是按值返回,第 15 课说过:不用担心慢,编译器会搬而不是抄。

打开 `src/gradebook.cpp`,看 `remove` 里的写法:

```cpp
auto it = std::find_if(students.begin(), students.end(),
                       [&name](const Student& s) { return s.get_name() == name; });
```

这是第 19 课的 `find_if` 加 lambda。`[&name]` 捕获外面的 `name`。读起来就是:"在 `students` 里,找第一个名字等于 `name` 的"。

## 6. main.cpp

`main` 里是一个循环(第 5 课),每轮显示菜单,读入选项,用 `switch` 分派(第 4 课)。

注意整个 `switch` 都放在 `try` 里面:

```cpp
try {
    switch (choice) {
        case 1: do_add(book); break;
        // ......
    }
} catch (const std::exception& e) {
    std::cout << "出错:" << e.what() << std::endl;
}
```

不论哪个功能抛出什么标准异常(名字为空、分数越界、重名、文件打不开),都会在这里被接住,输出提示,然后回到菜单继续。程序不会崩溃。

`do_add` 里还有一处值得讲的细节。用户如果在应该输入数字的地方输入了字母,`std::cin` 会进入"出错状态",之后的读取全部失败。如果不处理,菜单循环里 `cin` 一直读不到东西,程序行为就乱了。所以需要:

```cpp
if (!(std::cin >> name >> score)) {
    std::cin.clear();
    std::cin.ignore(10000, '\n');
    throw std::invalid_argument("输入格式不对,分数必须是整数");
}
```

- `std::cin.clear()`:清除出错状态。
- `std::cin.ignore(10000, '\n')`:把这一行里剩下的内容丢掉。
- 然后抛出异常,让外面统一提示。

这个问题我在写项目时真的遇到了:第一版没处理,输入 `Tom abc` 之后,程序就把后面的输入全吃坏了。**写完代码一定要故意输入错误的内容试一试**,就是为了发现这类问题。

`main.cpp` 开头有一对 `namespace { ... }`,它的作用是:把里面的函数限定在这个文件内部使用。先照写,不用深究。

## 7. CMake 配置

打开 `CMakeLists.txt`,和第 22 课比,多了几样东西:

```cmake
add_library(grade_core
    src/student.cpp
    src/gradebook.cpp
)
target_include_directories(grade_core PUBLIC include)

add_executable(grade_app src/main.cpp)
target_link_libraries(grade_app PRIVATE grade_core)

add_executable(grade_tests tests/test_gradebook.cpp)
target_link_libraries(grade_tests PRIVATE grade_core)
```

- `add_library`:把 `Student` 和 `Gradebook` 的代码做成一个**库**,名字叫 `grade_core`。库就是"一包可以被别人使用的代码"。
- `grade_app`(正式程序)和 `grade_tests`(测试程序)**都使用这个库**。这样同一份代码不用编译两次,测试测的也正是正式程序用的那份代码。
- `target_link_libraries(... grade_core)`:告诉 CMake "这个程序要用 `grade_core`"。
- `PUBLIC include` 让用到这个库的程序,自动也能找到 `include` 里的头文件。

## 8. 构建、测试、运行

在 WSL 里,进入项目文件夹:

```bash
cd /mnt/d/.Project/other/Homework/project/grade_manager
cmake -S . -B build
cmake --build build
```

先运行测试:

```bash
./build/grade_tests
```

输出:

```text
全部测试通过
```

也可以用 CMake 自带的测试命令:

```bash
cd build && ctest
```

```text
100% tests passed, 0 tests failed out of 1
```

再运行程序:

```bash
./build/grade_app
```

试着输入:`1` 回车,`Tom 90` 回车,再添加几个人,然后选 `4` 看排名,选 `6` 保存。程序会在当前文件夹里生成 `grades.txt`。

## 9. 用内存检查再跑一遍

第 12 课、第 22 课学的工具,现在派上用场。用另一个构建文件夹,打开内存检查:

```bash
cmake -S . -B build_asan -DUSE_ASAN=ON
cmake --build build_asan
./build_asan/grade_tests
./build_asan/grade_app
```

我对这个项目做过这样的检查,没有报错。也用 valgrind 查过泄漏:

```bash
valgrind --leak-check=full ./build/grade_app
```

得到:

```text
All heap blocks were freed -- no leaks are possible
```

整个项目没有一个 `new`,内存的事都交给了 `vector`、`string` 这样的标准库类型,所以没有泄漏。**这就是前 13 课反复讲的"能用标准库,就不要自己管内存"的结果。**

## 10. 测试是怎么写的

打开 `tests/test_gradebook.cpp`。每个功能一个测试函数:

- `test_student`:名字、分数、及格线(59 不及格,60 及格)、非法参数抛异常。
- `test_add_find_remove`:增删查,重名抛异常。
- `test_average_and_sort`:平均分(包括空成绩册)、排序。
- `test_save_load`:保存后再读取,内容一致。测完把临时文件删掉。

这些测试覆盖了第 23 课说的"边界":空成绩册、刚好 59/60、分数 -1/101、空名字。

文件里还有一个小的函数模板 `throws_invalid_argument`(第 17 课),用来检查"某段代码是否抛出了 `invalid_argument`",避免每次都重复写 `try/catch`。

## 11. 动手扩展

把项目当作你自己的,试着加功能。每加一个,先写测试,再写实现。

1. **统计及格人数**:在 `Gradebook` 里加 `int passed_count() const`,用 `std::count_if` 和 `Student::passed()`。先在测试里写好预期结果。
2. **修改分数**:加 `bool update_score(const std::string& name, int new_score)`。思考:`Student` 现在没有修改分数的函数,你要怎么加,才能继续保证分数合法?
3. **最高分和最低分**:返回的类型应该是什么?成绩册为空时怎么办?(提示:想想 `find` 为什么返回指针。)
4. **读取时的容错**:现在 `load` 遇到文件里有非法数据会抛异常,而且之前的数据不会被破坏(因为是先读到临时的 `loaded` 里,成功了才替换)。想一想为什么这样设计更安全。
5. **用 `std::map` 重写**:把 `vector<Student>` 换成 `map<string, Student>`,查找会更快。注意 `Student` 没有默认构造函数,用方括号放进 `map` 会有问题,查资料或自己试试怎么处理。
6. **用 AddressSanitizer 找 bug**:故意在 `Gradebook` 里写一个越界访问,确认工具能抓到。

## 12. 课程回顾

你已经走完了这一整套课程。回头看看走过的路:

| 部分 | 你学会了 |
| --- | --- |
| 语法入门(1–7 课) | 变量、运算、条件、循环、函数、数组、字符串、`vector` |
| 内存管理(8–13 课) | 地址、指针、引用、栈与堆、`new/delete`、常见错误与工具、智能指针与 RAII |
| 面向对象(14–17 课) | 类、构造与析构、拷贝与移动、继承与多态、模板 |
| 标准库(18–20 课) | 容器、算法、lambda、异常、文件 |
| 工程实践(21–24 课) | 多文件、CMake、调试、测试、完整项目 |

关于内存,请记住这几条:

1. **能不用 `new` 和 `delete`,就不用。** 用 `vector`、`string`、`unique_ptr`。
2. **谁负责归还,必须清楚。** `unique_ptr` 把这件事写进了代码里。
3. **用工具检查。** AddressSanitizer 和 valgrind 比眼睛可靠。
4. **指针和引用只是"借用"。** 只要对象还活着,借用才有效。别返回局部变量的引用,别在 `delete` 之后继续用。

## 接下来学什么

- **更多标准库:** `std::optional`、`std::variant`、`std::string_view`、`<chrono>`(时间)、`<thread>`(多线程)。
- **更现代的写法:** 结构化绑定、`std::filesystem`、范围库。
- **测试框架:** GoogleTest 或 Catch2。
- **做点真东西:** 小游戏、命令行工具、解析配置文件、简单的网络程序。
- **读别人的代码:** 找一个小型的开源 C++ 项目,试着读懂它的结构。

遇到问题时,一个好用的参考网站是 cppreference.com,里面是所有标准库功能的权威说明。

## 小结

- 先设计:每个类只管一件事。
- 构造函数保证对象一创建出来数据就合法。
- 用异常处理不应该发生的情况,在合适的地方统一接住。
- 项目里没有 `new` 和 `delete`,内存全部交给标准库管理。
- 写完代码要测试:正常输入、边界输入、错误输入都要试。
- 测试代码和正式代码共用一个库,保证测的就是用的。

