#include <iostream>
#include <vector>
#include <limits>

void countingSort(std::vector<int>& A) {
    int max_value = std::numeric_limits<int>::min();
    int min_value = std::numeric_limits<int>::max();

    for (int value : A) {
        if (value > max_value) max_value = value;
        if (value < min_value) min_value = value;
    }

    int range = max_value - min_value + 1;
    std::vector<int> count(range, 0);

    for (int value : A) {
        count[value - min_value]++;
    }

    int index = 0;
    for (int i = 0; i < range; i++) {
        while (count[i] > 0) {
            A[index++] = i + min_value;
            count[i]--;
        }
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;

    std::vector<int> A(n);
    for (int i = 0; i < n; i++) {
        std::cin >> A[i];
    }

    countingSort(A);

    for (int i = 0; i < n; i++) {
        std::cout << A[i] << (i == n - 1 ? '\n' : ' ');
    }

    return 0;
}