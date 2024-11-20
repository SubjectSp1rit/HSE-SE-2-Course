#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

void radixSort(std::vector<int>& A) {
    const int base = 256;
    const int bytes = sizeof(int);
    std::vector<int> buffer(A.size());

    const int bias = std::numeric_limits<int>::min();

    for (int byte = 0; byte < bytes; byte++) {
        std::vector<int> count(base, 0);

        for (int value : A) {
            int adjusted_value = value - bias;
            int byte_value = (adjusted_value >> (8 * byte)) & 0xFF;
            count[byte_value]++;
        }

        for (int i = 1; i < base; i++) {
            count[i] += count[i - 1];
        }

        for (int i = A.size() - 1; i >= 0; i--) {
            int adjusted_value = A[i] - bias;
            int byte_value = (adjusted_value >> (8 * byte)) & 0xFF;
            buffer[--count[byte_value]] = A[i];
        }

        std::swap(A, buffer);
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;

    if (n == 0) {
        std::cout << '\n';
        return 0;
    }

    std::vector<int> A(n);
    for (int i = 0; i < n; i++) {
        std::cin >> A[i];
    }

    radixSort(A);

    for (int i = 0; i < n; i++) {
        std::cout << A[i] << (i == n - 1 ? '\n' : ' ');
    }

    return 0;
}