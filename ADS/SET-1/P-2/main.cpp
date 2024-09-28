#include <iostream>
#include <unordered_set>
#include <string>
#include <sstream>

int maxHammingDistance(int* arr, int* moving_arr, int n) {
    int max_diffs = 0;
    std::unordered_set<std::string> diffs_set = {};

    // Рассмотрим крайние случаи
    int first_elem = arr[0];
    int cntr = 0;
    for (int i = 1; i < n; i++) {
        if (arr[i] == first_elem) {
            ++cntr;
        }
    }
    if (cntr == n - 1) {
        return 1;
    }

    for (int i = 0; i < (n - 1); ++i) {
        int cur_diffs = 0;

        // Крутим массив вправо
        int last = moving_arr[n - 1];
        for (int j = (n - 1); j > 0; --j) {
            moving_arr[j] = moving_arr[j - 1];
        }
        moving_arr[0] = last;

        // Считаем количество различий
        for (int j = 0; j < n; ++j) {
            if (arr[j] != moving_arr[j]) {
                ++cur_diffs;
            }
        }

        if (cur_diffs > max_diffs) {
            max_diffs = cur_diffs;

            std::string res;
            for (int k = 0; k < n; ++k) {
                res += std::to_string(moving_arr[k]);
            }
            diffs_set = {res};
        } else if (cur_diffs == max_diffs) {
            std::string res;
            for (int k = 0; k < n; ++k) {
                res += std::to_string(moving_arr[k]);
            }
            diffs_set.emplace(res);
        }
    }
    return diffs_set.size();
}

int main() {
    int n;
    std::cin >> n;
    auto arr = new int[n];
    auto moving_arr = new int[n];

    int val;
    for (int i = 0; i < n; ++i) {
        std::cin >> val;
        arr[i] = val;
        moving_arr[i] = val;
    }

    std::cout << maxHammingDistance(arr, moving_arr, n);

    // Удаляем массив из динамической памяти
    delete[] arr;

    return 0;
}