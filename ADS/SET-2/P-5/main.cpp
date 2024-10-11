#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <cmath>

using namespace std;

struct Point {
    int64_t x, y;
};

inline int64_t squaredDistance(const Point& a, const Point& b) {
    int64_t dx = a.x - b.x;
    int64_t dy = a.y - b.y;
    return dx * dx + dy * dy;
}

int64_t closestPairRec(vector<Point>& Px, int left, int right, vector<Point>& tempY) {
    int n = right - left;
    if (n <= 3) {
        int64_t minDist = INT64_MAX;
        for (int i = left; i < right; ++i) {
            for (int j = i + 1; j < right; ++j) {
                int64_t dist = squaredDistance(Px[i], Px[j]);
                minDist = min(minDist, dist);
            }
        }
        sort(Px.begin() + left, Px.begin() + right, [](const Point& a, const Point& b) {
            return a.y < b.y;
        });
        return minDist;
    }

    int mid = left + n / 2;
    int64_t midX = Px[mid].x;

    int64_t dl = closestPairRec(Px, left, mid, tempY);
    int64_t dr = closestPairRec(Px, mid, right, tempY);
    int64_t d = min(dl, dr);

    merge(Px.begin() + left, Px.begin() + mid, Px.begin() + mid, Px.begin() + right, tempY.begin() + left, [](const Point& a, const Point& b) {
        return a.y < b.y;
    });
    copy(tempY.begin() + left, tempY.begin() + right, Px.begin() + left);

    vector<Point> strip;
    for (int i = left; i < right; ++i) {
        if (abs(Px[i].x - midX) < d) {
            strip.push_back(Px[i]);
        }
    }

    int sz = strip.size();
    for (int i = 0; i < sz; ++i) {
        for (int j = i + 1; j < sz && j <= i + 7; ++j) {
            if ((strip[j].y - strip[i].y) >= d) {
                break;
            }
            int64_t dist = squaredDistance(strip[i], strip[j]);
            d = min(d, dist);
        }
    }

    return d;
}

int64_t closestPair(vector<Point>& points) {
    int n = points.size();
    vector<Point> tempY(n);

    return closestPairRec(points, 0, n, tempY);
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<Point> points;
    Point p;
    while (cin >> p.x >> p.y) {
        points.push_back(p);
    }

    int n = points.size();

    if (n < 2) {
        cout << 0 << '\n';
        return 0;
    }

    sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
        return a.x < b.x;
    });

    int64_t minDistSquared = closestPair(points);

    int64_t answer = static_cast<int64_t>(floor(sqrt((double)minDistSquared)));
    cout << answer << '\n';

    return 0;
}