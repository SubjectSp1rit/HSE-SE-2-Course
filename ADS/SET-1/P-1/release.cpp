#include <stdexcept>
#include <unordered_set>
#include "list.h"

List::List() : head(nullptr), tail(nullptr), _size(0) {}

List::List(const List& other) : List() {
    copy(other);
}

List::List(std::vector<int> array) : List() {
    for (const int& val : array) {
        push_back(val);
    }
}

List::~List() {
    clear();
}

int List::front() {
    return head->value;
}

int List::back() {
    return tail->value;
}

void List::push_back(int value) {
    Node* newNode = new Node(value);
    if (empty()) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
    ++_size;
}

void List::push_front(int value) {
    Node* newNode = new Node(value);
    if (empty()) {
        head = tail = newNode;
    } else {
        newNode->next = head;
        head->prev = newNode;
        head = newNode;
    }
    ++_size;
}

void List::insert(Node* pos, int value) {
    if (pos == nullptr) {
        throw std::runtime_error("Incorrect position!");
    }

    Node* newNode = new Node(value);
    if (pos == tail) {
        push_back(value);
    } else {
        newNode->next = pos->next;
        newNode->prev = pos;
        if (pos->next != nullptr) {
            pos->next->prev = newNode;
        }
        pos->next = newNode;
        ++_size;
    }
}

void List::pop_front() {
    if (empty()) {
        throw std::runtime_error("Deleting in empty list");
    }

    Node* toDelete = head;
    head = head->next;
    if (head != nullptr) {
        head->prev = nullptr;
    } else {
        tail = nullptr;
    }
    delete toDelete;
    --_size;
}

void List::pop_back() {
    if (empty()) {
        throw std::runtime_error("Deleting in empty list");
    }

    Node* toDelete = tail;
    tail = tail->prev;
    if (tail != nullptr) {
        tail->next = nullptr;
    } else {
        head = nullptr;
    }
    delete toDelete;
    --_size;
}

void List::erase(Node* pos) {
    if (pos == nullptr) {
        throw std::runtime_error("Incorrect position!");
    }

    if (pos == head) {
        pop_front();
    } else if (pos == tail) {
        pop_back();
    } else {
        pos->prev->next = pos->next;
        if (pos->next != nullptr) {
            pos->next->prev = pos->prev;
        }
        delete pos;
        --_size;
    }
}

void List::clear() {
    while (!empty()) {
        pop_front();
    }
}

void List::reverse() {
    if (empty()) return;

    Node* current = head;
    Node* temp = nullptr;

    while (current != nullptr) {
        temp = current->prev;
        current->prev = current->next;
        current->next = temp;
        current = current->prev;
    }

    temp = head;
    head = tail;
    tail = temp;
}

void List::remove_duplicates() {
    if (empty()) return;

    std::unordered_set<int> seen;
    Node* current = head;
    while (current != nullptr) {
        if (seen.find(current->value) != seen.end()) {
            Node* toDelete = current;
            current = current->next;
            erase(toDelete);
        } else {
            seen.insert(current->value);
            current = current->next;
        }
    }
}

void List::replace(int old_value, int new_value) {
    Node* current = head;
    while (current != nullptr) {
        if (current->value == old_value) {
            current->value = new_value;
        }
        current = current->next;
    }
}

void List::merge(const List& other) {
    Node* current = other.head;
    while (current != nullptr) {
        push_back(current->value);
        current = current->next;
    }
}

bool List::check_cycle() const {
    if (empty()) return false;

    Node* slow = head;
    Node* fast = head;
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return true;
    }
    return false;
}

size_t List::size() const {
    return _size;
}

bool List::empty() const {
    return _size == 0;
}

void List::copy(const List& other) {
    clear();
    Node* current = other.head;
    while (current != nullptr) {
        push_back(current->value);
        current = current->next;
    }
}
