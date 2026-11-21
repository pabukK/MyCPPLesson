#include <cassert>
#include <cstdio>
#include <iostream>
#include <stdexcept>

#include "gradebook.h"

namespace {

template <typename Func>
bool throws_invalid_argument(Func f) {
    try {
        f();
    } catch (const std::invalid_argument&) {
        return true;
    }
    return false;
}

void test_student() {
    Student s("小明", 75);
    assert(s.get_name() == "小明");
    assert(s.passed());
    assert(!Student("小红", 59).passed());
    assert(Student("小刚", 60).passed());
    assert(throws_invalid_argument([] { Student("", 50); }));
    assert(throws_invalid_argument([] { Student("a", -1); }));
    assert(throws_invalid_argument([] { Student("a", 101); }));
}

void test_add_find_remove() {
    Gradebook book;
    assert(book.size() == 0);
    book.add(Student("小明", 90));
    book.add(Student("小红", 80));
    assert(book.size() == 2);
    assert(book.find("小明") != nullptr);
    assert(book.find("没有") == nullptr);
    assert(throws_invalid_argument([&book] { book.add(Student("小明", 1)); }));
    assert(book.remove("小明"));
    assert(!book.remove("小明"));
    assert(book.size() == 1);
}

void test_average_and_sort() {
    Gradebook book;
    assert(book.average() == 0.0);
    book.add(Student("a", 70));
    book.add(Student("b", 90));
    book.add(Student("c", 80));
    assert(book.average() == 80.0);
    auto sorted = book.sorted_by_score();
    assert(sorted[0].get_name() == "b");
    assert(sorted[2].get_name() == "a");
}

void test_save_load() {
    Gradebook book;
    book.add(Student("a", 70));
    book.add(Student("b", 90));
    book.save("test_grades.txt");

    Gradebook other;
    other.load("test_grades.txt");
    assert(other.size() == 2);
    assert(other.find("b")->get_score() == 90);
    std::remove("test_grades.txt");
}

}  // namespace

int main() {
    test_student();
    test_add_find_remove();
    test_average_and_sort();
    test_save_load();
    std::cout << "全部测试通过" << std::endl;
    return 0;
}
