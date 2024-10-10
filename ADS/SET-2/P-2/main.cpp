#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdio>

// Убирает ведущие нули
void normalize(std::vector<int>& a) {
    while (a.size() > 1 && a.back() == 0)
        a.pop_back();
}

// Переводит строку в вектор значений
std::vector<int> stringToBigInt(const std::string& s) {
    std::vector<int> a;
    int n = s.size();
    for (int i = n; i > 0; i -= 9) {
        int start = std::max(0, i - 9);
        int len = i - start;
        int num = stoi(s.substr(start, len));
        a.push_back(num);
    }
    return a;
}

// Суммирует два больших числа (в виде векторов)
std::vector<int> add(const std::vector<int>& a, const std::vector<int>& b) {
    std::vector<int> res;
    int carry = 0;
    size_t n = std::max(a.size(), b.size());
    for (size_t i = 0; i < n || carry; ++i) {
        int x = carry;
        if (i < a.size())
            x += a[i];
        if (i < b.size())
            x += b[i];
        if (x >= 1000000000) {
            x -= 1000000000;
            carry = 1;
        } else {
            carry = 0;
        }
        res.push_back(x);
    }
    return res;
}

// Вычитает два больших числа (в виде векторов)
std::vector<int> subtract(const std::vector<int>& a, const std::vector<int>& b) {
    std::vector<int> res = a;
    int carry = 0;
    for (size_t i = 0; i < b.size() || carry; ++i) {
        res[i] -= carry + (i < b.size() ? b[i] : 0);
        if (res[i] < 0) {
            res[i] += 1000000000;
            carry = 1;
        } else {
            carry = 0;
        }
    }
    normalize(res);
    return res;
}

// Наивное умножение
std::vector<int> simpleMultiply(const std::vector<int>& a, const std::vector<int>& b) {
    std::vector<int> res(a.size() + b.size());
    for (size_t i = 0; i < a.size(); ++i) {
        int64_t carry = 0;
        for (size_t j = 0; j < b.size() || carry; ++j) {
            int64_t cur = res[i + j] + carry + int64_t(a[i]) * (j < b.size() ? b[j] : 0);
            res[i + j] = int(cur % 1000000000);
            carry = cur / 1000000000;
        }
    }
    normalize(res);
    return res;
}

// Сдвигает числа в векторе на k позиций влево
std::vector<int> shiftLeft(const std::vector<int>& a, size_t k) {
    std::vector<int> res(k, 0);
    res.insert(res.end(), a.begin(), a.end());
    return res;
}

// Алгоритм карацубы
std::vector<int> karatsubaMultiply(const std::vector<int>& a, const std::vector<int>& b) {
    size_t n = std::max(a.size(), b.size());

    if (n <= 32) {
        return simpleMultiply(a, b);
    }

    size_t k = n / 2;

    // Делит числа 
    std::vector<int> aLow(a.begin(), a.begin() + std::min(a.size(), k));
    std::vector<int> aHigh(a.begin() + std::min(a.size(), k), a.end());
    std::vector<int> bLow(b.begin(), b.begin() + std::min(b.size(), k));
    std::vector<int> bHigh(b.begin() + std::min(b.size(), k), b.end());

    std::vector<int> z0 = karatsubaMultiply(aLow, bLow);
    std::vector<int> z2 = karatsubaMultiply(aHigh, bHigh);

    std::vector<int> aLowHigh = add(aLow, aHigh);
    std::vector<int> bLowHigh = add(bLow, bHigh);

    std::vector<int> z1 = karatsubaMultiply(aLowHigh, bLowHigh);
    z1 = subtract(z1, z0);
    z1 = subtract(z1, z2);

    std::vector<int> res = add(z0, shiftLeft(z1, k));
    res = add(res, shiftLeft(z2, 2 * k));

    normalize(res);
    return res;
}

void printBigInt(const std::vector<int>& a) {
    if (a.empty()) {
        std::cout << 0 << std::endl;
        return;
    }
    std::cout << a.back();
    char buf[10];
    for (int i = int(a.size()) - 2; i >= 0; --i) {
        sprintf(buf, "%0*d", 9, a[i]);
        std::cout << buf;
    }
    std::cout << '\n';
}

int main() {
    std::string x, y;
    std::cin >> x >> y;

    std::vector<int> a = stringToBigInt(x);
    std::vector<int> b = stringToBigInt(y);

    std::vector<int> res = karatsubaMultiply(a, b);

    printBigInt(res);

    return 0;
}

