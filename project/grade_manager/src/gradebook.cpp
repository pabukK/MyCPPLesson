#include "gradebook.h"

#include <algorithm>
#include <fstream>
#include <numeric>
#include <stdexcept>

void Gradebook::add(const Student& s) {
    if (find(s.get_name()) != nullptr) {
        throw std::invalid_argument("已经有同名学生:" + s.get_name());
    }
    students.push_back(s);
}

bool Gradebook::remove(const std::string& name) {
    auto it = std::find_if(students.begin(), students.end(),
                           [&name](const Student& s) { return s.get_name() == name; });
    if (it == students.end()) {
        return false;
    }
    students.erase(it);
    return true;
}

const Student* Gradebook::find(const std::string& name) const {
    for (const auto& s : students) {
        if (s.get_name() == name) {
            return &s;
        }
    }
    return nullptr;
}

std::size_t Gradebook::size() const {
    return students.size();
}

double Gradebook::average() const {
    if (students.empty()) {
        return 0.0;
    }
    int total = std::accumulate(students.begin(), students.end(), 0,
                                [](int sum, const Student& s) { return sum + s.get_score(); });
    return static_cast<double>(total) / students.size();
}

std::vector<Student> Gradebook::sorted_by_score() const {
    std::vector<Student> copy = students;
    std::sort(copy.begin(), copy.end(), [](const Student& a, const Student& b) {
        if (a.get_score() != b.get_score()) {
            return a.get_score() > b.get_score();
        }
        return a.get_name() < b.get_name();
    });
    return copy;
}

void Gradebook::save(const std::string& path) const {
    std::ofstream out(path);
    if (!out) {
        throw std::runtime_error("无法写入文件:" + path);
    }
    for (const auto& s : students) {
        out << s.get_name() << " " << s.get_score() << "\n";
    }
}

void Gradebook::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("无法打开文件:" + path);
    }
    std::vector<Student> loaded;
    std::string name;
    int score = 0;
    while (in >> name >> score) {
        loaded.push_back(Student(name, score));
    }
    students = std::move(loaded);
}
