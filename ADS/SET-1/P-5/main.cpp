#include <iostream>
#include <deque>

void Fight(int n) {
    std::deque<int> dq;
    for (int i = 0; i < n; ++ i) {
        char action;
        std::cin >> action;
        if (action == '+') {
            int num;
            std::cin >> num;

            dq.push_back(num);
        } else if (action == '*') {
            int num;
            std::cin >> num;

            if (dq.size() % 2 == 0) {
                dq.insert(dq.begin() + dq.size() / 2, num);
            } else {
                dq.insert(dq.begin() + dq.size() / 2 + 1, num);
            }
        } else if (action == '-') {
            std::cout << dq.front() << '\n';
            dq.pop_front();
        }
    }
}

int main() {
    std::ios::sync_with_stdio(false);  // Отключить синхронизацию между iostream и stdio.
    std::cin.tie(nullptr);        // Отключить синхронизацию между std::cin и std::cout.
    int n;
    std::cin >> n;

    Fight(n);

    return 0;
}