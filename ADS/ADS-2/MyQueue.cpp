#include "MyQueue.h"

void MyQueue::push(int elem) {
    int currentMin = pushStack_.empty() ? elem : std::min(elem, pushStack_.top().second);
    pushStack_.emplace(elem, currentMin);
}

int MyQueue::pop() {
    if (popStack_.empty()) {
        while (!pushStack_.empty()) {
            int elem = pushStack_.top().first;
            pushStack_.pop();
            int currentMin = popStack_.empty() ? elem : std::min(elem, popStack_.top().second);
            popStack_.emplace(elem, currentMin);
        }
    }

    int elem = popStack_.top().first;
    popStack_.pop();
    return elem;
}

int& MyQueue::front() {
    if (popStack_.empty()) {
        while (!pushStack_.empty()) {
            int elem = pushStack_.top().first;
            pushStack_.pop();
            int currentMin = popStack_.empty() ? elem : std::min(elem, popStack_.top().second);
            popStack_.emplace(elem, currentMin);
        }
    }
    return popStack_.top().first;
}

int& MyQueue::back() {
    if (!pushStack_.empty()) {
        return pushStack_.top().first;
    }
    if (!popStack_.empty()) {
        return popStack_.top().first;
    }
    throw std::out_of_range("Очередь пуста");
}

int MyQueue::currentMin() {
    if (pushStack_.empty() && popStack_.empty()) {
        throw std::out_of_range("Очередь пуста");
    }

    if (pushStack_.empty()) {
        return popStack_.top().second;
    }
    if (popStack_.empty()) {
        return pushStack_.top().second;
    }

    return std::min(pushStack_.top().second, popStack_.top().second);
}