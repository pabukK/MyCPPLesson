#pragma once
#include <string>
#include <vector>

#include "student.h"

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
