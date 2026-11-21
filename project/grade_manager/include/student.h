#pragma once
#include <string>

class Student {
public:
    Student(std::string name, int score);

    const std::string& get_name() const;
    int get_score() const;
    bool passed() const;

private:
    std::string name;
    int score = 0;
};
