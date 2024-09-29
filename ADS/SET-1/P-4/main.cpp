#include <iostream>
#include <stack>
#include <queue>
#include <vector>

std::vector<std::pair<int, int>> VagonSort(std::queue<int> q, int n) {
    // Тупик (стек)
    auto deadlock = std::stack<int>();
    // Сборщик всех совершенных действий
    auto answer = std::vector<std::pair<int, int>>();
    // Счетчик номера выгона, который должен выехать следующим
    int counter = 1;
    while (!q.empty()) {
        // Первый элемент из очереди
        int curVal = q.front();
        // Удаление элемента из очереди
        q.pop();

        // Если номер поезда в очереди не равен текущему - отправляем его в тупик
        if (curVal != counter) {
            answer.emplace_back(1, 1);
            deadlock.push(curVal);
        // Если номер поезда равен текущему - сразу отправляем его на выезд
        } else if (curVal == counter) {
            answer.emplace_back(1, 1);
            answer.emplace_back(2, 1);
            // Инкрементируем номер поезда, который будем искать
            ++counter;
            while(!deadlock.empty() && deadlock.top() == counter) {
                answer.emplace_back(2, 1);
                deadlock.pop();
                ++counter;
            }
        }
    }

    while(!deadlock.empty()) {
        if (deadlock.top() == counter) {
            answer.emplace_back(2, 1);
            deadlock.pop();
            ++counter;
        } else {
            return {};
        }
    }

    return answer;
}

int main() {
    int n;
    std::cin >> n;
    auto q = std::queue<int>{};
    for (int i = 0; i < n; ++i) {
        int val;
        std::cin >> val;
        q.emplace(val);
    }

    auto answ = VagonSort(q, n);
    if (answ.size() == 0) {
        std::cout << 0;
    } else {
        for (auto elem : answ) {
            std::cout << elem.first << " " << elem.second << '\n';
        }
    }

    return 0;
}