# 第 22 课:CMake

## 这一课你会学到

- 为什么需要 CMake
- 写一个最小的 `CMakeLists.txt`
- 用 CMake 编译多文件项目
- 添加编译选项和内存检查

## 1. 为什么需要 CMake

上一课,多文件项目要手敲这样的命令:

```bash
g++ -std=c++17 -Wall -o app main.cpp student.cpp math_utils.cpp
```

文件有几十个的时候,这就不现实了。而且你改了一个文件,其实只需要重新编译那一个,但手敲命令会把所有的都重新编译一遍,很慢。

**CMake** 是一个管理编译的工具。你在一个配置文件里写清楚"项目有哪些文件、用什么选项",它会替你生成编译命令,并且聪明地只重新编译改动过的部分。

CMake 本身不编译代码,它是调用 g++ 来编译。你可以把它看成"项目经理",g++ 是"干活的工人"。

你的 WSL 里已经装好了 CMake(开始时一起装的),用 `cmake --version` 可以确认。

## 2. 最小的项目

先建一个文件夹,里面放源文件和一个叫 `CMakeLists.txt` 的配置文件(名字必须一模一样)。

```text
hello_cmake/
├── CMakeLists.txt
└── main.cpp
```

`main.cpp`:

```cpp
// file: main.cpp
#include <iostream>

int main() {
    std::cout << "来自 CMake 项目" << std::endl;
    return 0;
}
```

`CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.13)
project(HelloCmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app main.cpp)
```

逐行看 `CMakeLists.txt`:

| 行 | 意思 |
| --- | --- |
| `cmake_minimum_required(VERSION 3.13)` | 要求 CMake 至少是 3.13 版本 |
| `project(HelloCmake)` | 给项目起个名字 |
| `set(CMAKE_CXX_STANDARD 17)` | 使用 C++17,相当于命令行里的 `-std=c++17` |
| `set(CMAKE_CXX_STANDARD_REQUIRED ON)` | 编译器不支持 17 就报错,不要悄悄降级 |
| `add_executable(app main.cpp)` | 生成一个叫 `app` 的程序,源文件是 `main.cpp` |

## 3. 构建项目

在项目文件夹里执行两步:

```bash
cmake -S . -B build
cmake --build build
./build/app
```

- `cmake -S . -B build`:**配置**。`-S .` 表示源码在当前文件夹,`-B build` 表示把所有生成的文件放进 `build` 文件夹。
- `cmake --build build`:**编译**。它会调用 g++。
- `./build/app`:运行生成的程序。

屏幕上会出现:

```text
来自 CMake 项目
```

把所有生成的东西放进 `build` 文件夹,好处是:你的源码文件夹保持干净。想"清理重来",直接删掉 `build` 文件夹就行,不会伤到源码。

**以后改了代码,只需要再执行 `cmake --build build`**,不用重新配置。只有改了 `CMakeLists.txt`,才需要重新配置(但再执行一遍 `cmake --build build` 通常也会自动重新配置)。

## 4. 多个源文件

把上一课的项目用 CMake 管理,只需要把源文件列在 `add_executable` 里:

```text
student_project/
├── CMakeLists.txt
├── include/
│   └── student.h
└── src/
    ├── main.cpp
    └── student.cpp
```

`CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.13)
project(StudentProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app
    src/main.cpp
    src/student.cpp
)
target_include_directories(app PRIVATE include)
```

- `add_executable(app ...)`:把所有 `.cpp` 都列出来,一行一个,清楚。
- `target_include_directories(app PRIVATE include)`:告诉编译器"到 `include` 文件夹里找头文件"。这样 `.cpp` 里写 `#include "student.h"` 就能找到。`PRIVATE` 先照写,不用深究。

## 5. 只重新编译改动的文件

多文件项目构建后,如果只改了 `main.cpp`,再执行 `cmake --build build`:

```text
[ 33%] Building CXX object CMakeFiles/app.dir/src/main.cpp.o
[ 66%] Linking CXX executable app
[100%] Built target app
```

只重新编译了 `main.cpp`,`student.cpp` 没动。项目越大,这个优势越明显。

## 6. 加编译选项

第 1 课讲过,编译时要加 `-Wall`。在 CMake 里,用 `target_compile_options` 给程序加选项:

```cmake
add_executable(app main.cpp)
target_compile_options(app PRIVATE -Wall -Wextra)
```

`-Wextra` 是比 `-Wall` 更严格的一档提醒。

## 7. 一键开启内存检查

第 12 课学过,编译时加 `-fsanitize=address -g` 可以查内存错误。每次手改配置很麻烦,可以在 `CMakeLists.txt` 里加一个开关:

```cmake
cmake_minimum_required(VERSION 3.13)
project(HelloCmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app main.cpp)
target_compile_options(app PRIVATE -Wall -Wextra)

option(USE_ASAN "开启内存检查" OFF)
if(USE_ASAN)
    target_compile_options(app PRIVATE -fsanitize=address -g)
    target_link_options(app PRIVATE -fsanitize=address)
endif()
```

- `option(名字 "说明" 默认值)` 定义一个开关,默认关闭(`OFF`)。
- `if(USE_ASAN) ... endif()` 在开关打开时,才加上这些选项。
- 内存检查要在**编译和链接**两个阶段都加选项,所以有两行。
- `target_link_options` 是 CMake **3.13** 才有的命令,所以最低版本写的是 3.13。版本号写低了,在老版本 CMake 上会直接报 `Unknown CMake command`。

平时正常构建。想查内存问题时,用另一个文件夹,并打开开关:

```bash
cmake -S . -B build_asan -DUSE_ASAN=ON
cmake --build build_asan
./build_asan/app
```

如果程序里有越界之类的问题,运行时会得到和第 12 课一样的报告:

```text
ERROR: AddressSanitizer: heap-buffer-overflow ...
WRITE of size 4 ...
```

两个构建文件夹(`build` 和 `build_asan`)互不干扰,想用哪个就用哪个。

## 8. 常见问题

- **改了代码,运行结果没变?** 忘了重新执行 `cmake --build build`。
- **新增了 `.cpp` 文件,编译报找不到?** 要把它加进 `add_executable` 的列表,然后重新构建。
- **想从头来过?** 删掉 `build` 文件夹,重新执行两步。
- **`CMakeLists.txt` 里的名字大小写?** `CMakeLists.txt` 的文件名必须完全一致。里面的命令(如 `add_executable`)不分大小写,但惯例写小写。

## 练习

1. 按第 2 节搭一个最小项目,构建并运行。
2. 把第 14 课的 `Rectangle` 拆成 `include/rectangle.h`、`src/rectangle.cpp`、`src/main.cpp`,用 CMake 管理。
3. 打开 `USE_ASAN`,故意写一个越界访问,确认能被抓到。
4. 在 `main.cpp` 里故意写一个未使用的变量,看看 `-Wall -Wextra` 会给出什么提醒。

## 小结

- CMake 是管理编译的工具,调用 g++ 来干活。
- 配置文件叫 `CMakeLists.txt`,必须同名。
- `add_executable(程序名 源文件...)` 生成程序。
- 两步:`cmake -S . -B build` 配置,`cmake --build build` 编译。
- 所有生成的文件放进 `build`,想清理就删掉它。
- 只重新编译改动的文件。
- 用 `option` 做一个开关,一键打开内存检查。

