cmake_minimum_required(VERSION 3.10)
project(GradeManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_ASAN "开启内存检查" OFF)

add_library(grade_core
    src/student.cpp
    src/gradebook.cpp
)
target_include_directories(grade_core PUBLIC include)
target_compile_options(grade_core PRIVATE -Wall -Wextra)

add_executable(grade_app src/main.cpp)
target_link_libraries(grade_app PRIVATE grade_core)
target_compile_options(grade_app PRIVATE -Wall -Wextra)

add_executable(grade_tests tests/test_gradebook.cpp)
target_link_libraries(grade_tests PRIVATE grade_core)
target_compile_options(grade_tests PRIVATE -Wall -Wextra)

if(USE_ASAN)
    foreach(t grade_core grade_app grade_tests)
        target_compile_options(${t} PRIVATE -fsanitize=address -g)
        target_link_options(${t} PRIVATE -fsanitize=address)
    endforeach()
endif()

enable_testing()
add_test(NAME gradebook COMMAND grade_tests)
