#include <iostream>
#include <vector>

bool MatrixSearch(const std::vector<std::vector<int>>& matrix, int key) {
    if (matrix.empty() || matrix[0].empty()) return false;

    int n = matrix.size();
    int row = 0;
    int col = 0;

    while (row < n && col < n) {
        int current = matrix[row][col];

        if (current == key) {
            return true;
        }
        if (current < key) {
            col++;
        } else {
            row++;
        }
    }

    return false;
}

int main() {
    std::vector<std::vector<int>> matrix = {
        {11, 12, 18},
        {8, 9, 10},
        {5, 6, 7}
    };
    int key;
    std::cin >> key;

    if (MatrixSearch(matrix, key)) {
        std::cout << "Значение " << key << " найдено в матрице" << '\n';
    } else {
        std::cout << "Значение " << key << " не найдено в матрице" << '\n';
    }

    return 0;
}

