#include <iostream>
#include <cmath>
#include <random>

struct Circle {
    double x, y, r;

    Circle(double x_, double y_, double r_) : x(x_), y(y_), r(r_) {}
};

bool isInsideCircle(const Circle& circle, double px, double py) {
    double dx = px - circle.x;
    double dy = py - circle.y;
    return (dx * dx + dy * dy) <= (circle.r * circle.r);
}

bool isInsideIntersection(const Circle& c1, const Circle& c2, const Circle& c3, double px, double py) {
    return isInsideCircle(c1, px, py) && isInsideCircle(c2, px, py) && isInsideCircle(c3, px, py);
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    double x1, y1, r1, x2, y2, r2, x3, y3, r3;
    std::cin >> x1 >> y1 >> r1 >> x2 >> y2 >> r2 >> x3 >> y3 >> r3;

    Circle c1(x1, y1, r1), c2(x2, y2, r2), c3(x3, y3, r3);

    double minX = std::min({x1 - r1, x2 - r2, x3 - r3});
    double maxX = std::max({x1 + r1, x2 + r2, x3 + r3});
    double minY = std::min({y1 - r1, y2 - r2, y3 - r3});
    double maxY = std::max({y1 + r1, y2 + r2, y3 + r3});

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> disX(minX, maxX);
    std::uniform_real_distribution<> disY(minY, maxY);

    const int NUM_POINTS = 1000000;
    int insideCount = 0;

    for (int i = 0; i < NUM_POINTS; ++i) {
        double px = disX(gen);
        double py = disY(gen);

        if (isInsideIntersection(c1, c2, c3, px, py)) {
            ++insideCount;
        }
    }

    double rectArea = (maxX - minX) * (maxY - minY);

    double intersectionArea = (static_cast<double>(insideCount) / NUM_POINTS) * rectArea;

    std::cout << intersectionArea << '\n';

    return 0;
}