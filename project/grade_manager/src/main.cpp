#include <iostream>
#include <stdexcept>
#include <string>

#include "gradebook.h"

namespace {

void print_menu() {
    std::cout << "\n1 添加  2 删除  3 查询  4 排名  5 平均分  6 保存  7 读取  0 退出\n> ";
}

void do_add(Gradebook& book) {
    std::string name;
    int score = 0;
    std::cout << "名字 分数:";
    if (!(std::cin >> name >> score)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        throw std::invalid_argument("输入格式不对,分数必须是整数");
    }
    book.add(Student(name, score));
    std::cout << "已添加" << std::endl;
}

void do_remove(Gradebook& book) {
    std::string name;
    std::cout << "要删除的名字:";
    std::cin >> name;
    std::cout << (book.remove(name) ? "已删除" : "没有这个人") << std::endl;
}

void do_find(const Gradebook& book) {
    std::string name;
    std::cout << "要查询的名字:";
    std::cin >> name;
    const Student* s = book.find(name);
    if (s == nullptr) {
        std::cout << "没有这个人" << std::endl;
    } else {
        std::cout << s->get_name() << " " << s->get_score() << " 分" << std::endl;
    }
}

void do_rank(const Gradebook& book) {
    int rank = 1;
    for (const auto& s : book.sorted_by_score()) {
        std::cout << rank++ << ". " << s.get_name() << " " << s.get_score() << std::endl;
    }
}

}  // namespace

int main() {
    Gradebook book;
    int choice = -1;

    while (choice != 0) {
        print_menu();
        if (!(std::cin >> choice)) {
            break;
        }
        try {
            switch (choice) {
                case 1: do_add(book); break;
                case 2: do_remove(book); break;
                case 3: do_find(book); break;
                case 4: do_rank(book); break;
                case 5: std::cout << "平均分:" << book.average() << std::endl; break;
                case 6: book.save("grades.txt"); std::cout << "已保存" << std::endl; break;
                case 7: book.load("grades.txt"); std::cout << "已读取" << std::endl; break;
                case 0: break;
                default: std::cout << "无效选项" << std::endl; break;
            }
        } catch (const std::exception& e) {
            std::cout << "出错:" << e.what() << std::endl;
        }
    }
    return 0;
}
