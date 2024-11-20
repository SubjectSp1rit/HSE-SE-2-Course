#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

struct Fraction {
    int numerator;
    int denominator;
    int index;

    bool operator<(const Fraction& other) const {
        return static_cast<long long>(numerator) * other.denominator <
               static_cast<long long>(other.numerator) * denominator;
    }
};

Fraction parseFraction(const std::string& fractionString, int index) {
    size_t slashPos = fractionString.find('/');
    int numerator = std::stoi(fractionString.substr(0, slashPos));
    int denominator = std::stoi(fractionString.substr(slashPos + 1));
    return {numerator, denominator, index};
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N;
    std::cin >> N;

    std::vector<Fraction> fractions(N);
    for (int i = 0; i < N; i++) {
        std::string fractionString;
        std::cin >> fractionString;
        fractions[i] = parseFraction(fractionString, i);
    }

    std::stable_sort(fractions.begin(), fractions.end());

    for (const Fraction& fraction : fractions) {
        std::cout << fraction.numerator << '/' << fraction.denominator << ' ';
    }
    std::cout << '\n';

    return 0;
}