#include "student.h"

#include <stdexcept>

Student::Student(std::string n, int s) : name(n), score(s) {
    if (name.empty()) {
        throw std::invalid_argument("名字不能为空");
    }
    if (score < 0 || score > 100) {
        throw std::invalid_argument("分数必须在 0 到 100 之间");
    }
}

const std::string& Student::get_name() const {
    return name;
}

int Student::get_score() const {
    return score;
}

bool Student::passed() const {
    return score >= 60;
}
