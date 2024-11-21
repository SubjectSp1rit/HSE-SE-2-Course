#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>

int evaluate_board(const std::vector<int> &board) {
    size_t n = board.size();
    std::vector<int> main_diag(2 * n, 0);
    std::vector<int> sub_diag(2 * n, 0);
    int valid_positions = 0;

    for (size_t col = 0; col < n; ++col) {
        int main_idx = n + col - board[col];
        int sub_idx = col + board[col];
        ++main_diag[main_idx];
        ++sub_diag[sub_idx];

        if (main_diag[main_idx] == 1 && sub_diag[sub_idx] == 1) {
            ++valid_positions;
        }
    }

    return valid_positions;
}

void generate_neighbor(std::vector<int> &board, std::mt19937 &rng) {
    std::uniform_int_distribution<> dist(0, board.size() - 1);
    int i = dist(rng);
    int j = dist(rng);
    while (i == j) {
        j = dist(rng);
    }
    std::swap(board[i], board[j]);
}

void solve_n_queens(int n) {
    std::vector<int> queens_positions(n);
    for (int i = 0; i < n; ++i) {
        queens_positions[i] = i;
    }

    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(queens_positions.begin(), queens_positions.end(), rng);

    int current_energy = evaluate_board(queens_positions);
    double temperature = 1.0;

    const int max_iterations = 50000;
    std::uniform_real_distribution<double> probability_dist(0, 1);

    for (int iteration = 0; iteration < max_iterations; ++iteration) {
        temperature *= 0.99;

        std::vector<int> new_positions = queens_positions;
        generate_neighbor(new_positions, rng);

        int new_energy = evaluate_board(new_positions);
        int delta_energy = new_energy - current_energy;

        if (exp(delta_energy / temperature) > probability_dist(rng) || delta_energy > 0) {
            queens_positions = new_positions;
            current_energy = new_energy;
        }

        if (current_energy == n) {
            break;
        }
    }

    for (int position : queens_positions) {
        std::cout << position + 1 << ' ';
    }
    std::cout << '\n';
}

int main() {
    int n;
    std::cin >> n;

    solve_n_queens(n);

    return 0;
}