#pragma once
#include <stack>

class MyQueue {
public:
    void push(int elem);
    int pop();
    int& front();
    int& back();
    int currentMin();
private:
    std::stack<std::pair<int, int>> pushStack_;
    std::stack<std::pair<int, int>> popStack_;
};