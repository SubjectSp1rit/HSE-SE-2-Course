#include <iostream>
#include <vector>
#include <bitset>
#include <string>

const int MAX_N = 4000;

using Bitset = std::bitset<MAX_N>;

std::vector<Bitset> readMatrix(int n) {
    std::vector<Bitset> matrix(n);
    int blocks = (n + 3) / 4;

    for (int i = 0; i < n; i++) {
        std::string line;
        std::cin >> line;

        int bitIndex = 0;
        for (int j = 0; j < blocks; j++) {
            char hexDigit = line[j];
            int value = (hexDigit >= '0' && hexDigit <= '9') ? hexDigit - '0' : hexDigit - 'A' + 10;

            for (int k = 3; k >= 0; k--) {
                if (bitIndex < n) {
                    matrix[i][bitIndex++] = (value >> k) & 1;
                }
            }
        }
    }
    return matrix;
}

std::vector<Bitset> multiply(const std::vector<Bitset>& A, const std::vector<Bitset>& B, int n) {
    std::vector<Bitset> result(n);

    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            if (A[i][k]) {
                result[i] ^= B[k];
            }
        }
    }

    return result;
}

bool compareMatrices(const std::vector<Bitset>& A, const std::vector<Bitset>& B, int n) {
    for (int i = 0; i < n; i++) {
        if (A[i] != B[i]) {
            return false;
        }
    }
    return true;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;

    auto A = readMatrix(n);
    auto B = readMatrix(n);
    auto C = readMatrix(n);

    auto AB = multiply(A, B, n);

    if (compareMatrices(AB, C, n)) {
        std::cout << "YES\n";
    } else {
        std::cout << "NO\n";
    }

    return 0;
}