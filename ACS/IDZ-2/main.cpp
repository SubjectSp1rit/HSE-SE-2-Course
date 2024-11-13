#include <iostream>
#include <cmath>
#include <iomanip>

// Эталонное значение числа π для сравнения
const double PI_REF = 3.141592653589793;

// Функция для вычисления π методом Виета
double compute_pi_vieta(int N) {
    double prod = 1.0;
    double current_value = std::sqrt(2.0);

    for (int i = 0; i < N; ++i) {
        prod *= 2.0 / current_value;
        current_value = std::sqrt(2.0 + current_value);
    }

    return 2.0 * prod;
}

// Функция для вычисления относительной ошибки в процентах
double relative_error(double computed_pi) {
    return std::abs((PI_REF - computed_pi) / PI_REF) * 100;
}

int main() {
    int test_values[] = {1, 10, 50, 100, 500, 700, 1000, 1500, 2000, 10000};
    double error;

    std::cout << std::fixed << std::setprecision(10);
    std::cout << "N\tComputed Pi\tResult\n";
    std::cout << "---------------------------------------------------------\n";

    for (int N : test_values) {
        double computed_pi = compute_pi_vieta(N);
        error = relative_error(computed_pi);

        std::string result = (error < 0.05) ? "Passed" : "Failed";

        std::cout << N << "\t" << computed_pi << "\t" << result << "\n";
    }

    return 0;
}