#include <iostream>
#include <vector>
#include <algorithm>

// Складывает две матрицы
std::vector<std::vector<int64_t>> add(const std::vector<std::vector<int64_t>>& A, const std::vector<std::vector<int64_t>>& B, int size) {
    std::vector<std::vector<int64_t>> C(size, std::vector<int64_t>(size));
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            C[i][j] = A[i][j] + B[i][j];
    return C;
}

// Вычитает матрицы
std::vector<std::vector<int64_t>> subtract(const std::vector<std::vector<int64_t>>& A, const std::vector<std::vector<int64_t>>& B, int size) {
    std::vector<std::vector<int64_t>> C(size, std::vector<int64_t>(size));
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            C[i][j] = A[i][j] - B[i][j];
    return C;
}

// Умножение матриц методом разделяй-и-властвуй
std::vector<std::vector<int64_t>> DaCMatrixMultiply(const std::vector<std::vector<int64_t>>& A, const std::vector<std::vector<int64_t>>& B, int size) {
    std::vector<std::vector<int64_t>> C(size, std::vector<int64_t>(size, 0));

    if (size <= 64) {
        for (int i = 0; i < size; ++i)
            for (int k = 0; k < size; ++k)
                for (int j = 0; j < size; ++j)
                    C[i][j] += A[i][k] * B[k][j];
    } else {
        int newSize = size / 2;

        std::vector<std::vector<int64_t>> A11(newSize, std::vector<int64_t>(newSize));
        std::vector<std::vector<int64_t>> A12(newSize, std::vector<int64_t>(newSize));
        std::vector<std::vector<int64_t>> A21(newSize, std::vector<int64_t>(newSize));
        std::vector<std::vector<int64_t>> A22(newSize, std::vector<int64_t>(newSize));

        std::vector<std::vector<int64_t>> B11(newSize, std::vector<int64_t>(newSize));
        std::vector<std::vector<int64_t>> B12(newSize, std::vector<int64_t>(newSize));
        std::vector<std::vector<int64_t>> B21(newSize, std::vector<int64_t>(newSize));
        std::vector<std::vector<int64_t>> B22(newSize, std::vector<int64_t>(newSize));

        for (int i = 0; i < newSize; ++i)
            for (int j = 0; j < newSize; ++j) {
                A11[i][j] = A[i][j];
                A12[i][j] = A[i][j + newSize];
                A21[i][j] = A[i + newSize][j];
                A22[i][j] = A[i + newSize][j + newSize];

                B11[i][j] = B[i][j];
                B12[i][j] = B[i][j + newSize];
                B21[i][j] = B[i + newSize][j];
                B22[i][j] = B[i + newSize][j + newSize];
            }

        // Рекурсивные вычисления
        auto M1 = DaCMatrixMultiply(add(A11, A22, newSize), add(B11, B22, newSize), newSize);
        auto M2 = DaCMatrixMultiply(add(A21, A22, newSize), B11, newSize);
        auto M3 = DaCMatrixMultiply(A11, subtract(B12, B22, newSize), newSize);
        auto M4 = DaCMatrixMultiply(A22, subtract(B21, B11, newSize), newSize);
        auto M5 = DaCMatrixMultiply(add(A11, A12, newSize), B22, newSize);
        auto M6 = DaCMatrixMultiply(subtract(A21, A11, newSize), add(B11, B12, newSize), newSize);
        auto M7 = DaCMatrixMultiply(subtract(A12, A22, newSize), add(B21, B22, newSize), newSize);

        // Вычисление подматриц результата
        auto C11 = add(subtract(add(M1, M4, newSize), M5, newSize), M7, newSize);
        auto C12 = add(M3, M5, newSize);
        auto C21 = add(M2, M4, newSize);
        auto C22 = add(subtract(add(M1, M3, newSize), M2, newSize), M6, newSize);

        // Объединение подматриц в одну матрицу результата
        for (int i = 0; i < newSize; ++i)
            for (int j = 0; j < newSize; ++j) {
                C[i][j] = C11[i][j];
                C[i][j + newSize] = C12[i][j];
                C[i + newSize][j] = C21[i][j];
                C[i + newSize][j + newSize] = C22[i][j];
            }
    }

    return C;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;

    std::vector<std::vector<int64_t>> A(n, std::vector<int64_t>(n));
    std::vector<std::vector<int64_t>> B(n, std::vector<int64_t>(n));

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            std::cin >> A[i][j];

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            std::cin >> B[i][j];

    auto C = DaCMatrixMultiply(A, B, n);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            std::cout << C[i][j] << ' ';
        std::cout << '\n';
    }

    return 0;
}

