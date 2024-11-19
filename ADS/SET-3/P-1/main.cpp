#include <iostream>
#include <vector>

void heapify(std::vector<int>& A, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && A[left] > A[largest]) {
        largest = left;
    }

    if (right < n && A[right] > A[largest]) {
        largest = right;
    }

    if (largest != i) {
        std::swap(A[i], A[largest]);

        heapify(A, n, largest);
    }
}

void buildMaxHeap(std::vector<int>& A) {
    int n = A.size();
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(A, n, i);
    }
}

void heapSort(std::vector<int>& A) {
    int n = A.size();

    buildMaxHeap(A);

    for (int i = n - 1; i >= 0; i--) {
        std::swap(A[0], A[i]);

        heapify(A, i, 0);
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;

    std::vector<int> A(n);
    for (int i = 0; i < n; i++) {
        std::cin >> A[i];
    }

    heapSort(A);

    for (int i = 0; i < n; i++) {
        std::cout << A[i] << (i == n - 1 ? '\n' : ' ');
    }

    return 0;
}