#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <cmath>
#include <unordered_set>

using namespace std;

struct Point3D {
    int64_t x, y, z;
    int index; // исходный индекс точки во входных данных

    bool operator==(const Point3D& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

// Хеш-функция для ускорения работы
struct Point3DHash {
    size_t operator()(const Point3D& p) const {
        return hash<int64_t>()(p.x) ^ hash<int64_t>()(p.y) ^ hash<int64_t>()(p.z);
    }
};

// Вычисляет квадрат расстояния между двумя точками
inline int64_t squaredDistance(const Point3D& a, const Point3D& b) {
    int64_t dx = a.x - b.x;
    int64_t dy = a.y - b.y;
    int64_t dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

// Находит минимальное расстояние
int64_t closestPairRec(vector<Point3D>& Px, int left, int right, vector<Point3D>& tempY, pair<int, int>& result) {
    int n = right - left;
    // Базовый случай: если точек 3 или меньше => вычисляем расстояния напрямую
    if (n <= 3) {
        int64_t minDist = INT64_MAX;
        for (int i = left; i < right; ++i) {
            for (int j = i + 1; j < right; ++j) {
                int64_t dist = squaredDistance(Px[i], Px[j]);
                if (dist < minDist) {
                    minDist = dist;
                    result = {Px[i].index, Px[j].index};
                }
            }
        }
        // Сортировка по y для последующего слияния
        sort(Px.begin() + left, Px.begin() + right, [](const Point3D& a, const Point3D& b) {
            return a.y < b.y;
        });
        return minDist;
    }

    int mid = left + n / 2;
    int64_t midX = Px[mid].x;

    // Делим пополам
    pair<int, int> leftResult, rightResult;
    int64_t dl = closestPairRec(Px, left, mid, tempY, leftResult);
    int64_t dr = closestPairRec(Px, mid, right, tempY, rightResult);

    // Выбираем наименьшее расстояние
    int64_t d = dl;
    result = leftResult;
    if (dr < d) {
        d = dr;
        result = rightResult;
    }

    // Слияние отсортированных половин по y
    merge(Px.begin() + left, Px.begin() + mid, Px.begin() + mid, Px.begin() + right,
          tempY.begin() + left, [](const Point3D& a, const Point3D& b) {
        return a.y < b.y;
    });
    copy(tempY.begin() + left, tempY.begin() + right, Px.begin() + left);

    // Построение полосы шириной 2
    vector<Point3D> strip;
    strip.reserve(right - left);
    for (int i = left; i < right; ++i) {
        int64_t dx = Px[i].x - midX;
        if (dx * dx < d) {
            strip.push_back(Px[i]);
        }
    }

    // Поиск минимального расстояния в полосе
    int sz = strip.size();
    for (int i = 0; i < sz; ++i) {
        for (int j = i + 1; j < sz && (strip[j].y - strip[i].y) * (strip[j].y - strip[i].y) < d; ++j) {
            int64_t dy = strip[j].y - strip[i].y;
            if (dy * dy >= d)
                break;
            int64_t dz = strip[j].z - strip[i].z;
            if (dz * dz >= d)
                continue;
            int64_t dist = squaredDistance(strip[i], strip[j]);
            if (dist < d) {
                d = dist;
                result = {strip[i].index, strip[j].index};
            }
        }
    }

    return d;
}

// Находит минимальное расстояние между точками
double closestPair(vector<Point3D>& points, pair<int, int>& result) {
    int n = points.size();
    vector<Point3D> tempY(n);

    // Проверка на наличие одинаковых точек
    unordered_set<Point3D, Point3DHash> pointSet;
    for (const auto& point : points) {
        if (pointSet.find(point) != pointSet.end()) {
            result = {point.index, point.index};
            return 0.0;
        }
        pointSet.insert(point);
    }

    int64_t minDistSquared = closestPairRec(points, 0, n, tempY, result);
    return sqrt(static_cast<double>(minDistSquared));
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    vector<Point3D> points(n);

    bool sameX = true, sameY = true, sameZ = true;
    int64_t firstX, firstY, firstZ;

    // Считывание точек и проверка на совпадение координат
    for (int i = 0; i < n; ++i) {
        cin >> points[i].x >> points[i].y >> points[i].z;
        points[i].index = i + 1;

        if (i == 0) {
            firstX = points[i].x;
            firstY = points[i].y;
            firstZ = points[i].z;
        } else {
            if (points[i].x != firstX) sameX = false;
            if (points[i].y != firstY) sameY = false;
            if (points[i].z != firstZ) sameZ = false;
        }
    }

    if (n < 2) {
        cout << fixed << setprecision(5) << 0.0 << '\n';
        cout << "1 1\n";
        return 0;
    }

    // Если точки лежат на одной линии или плоскости => сводим задачу к Р-5
    if (sameX && sameY) {
        // Точки на оси z
        sort(points.begin(), points.end(), [](const Point3D& a, const Point3D& b) {
            return a.z < b.z;
        });
        double minDist = abs(points[1].z - points[0].z);
        pair<int, int> result = {points[0].index, points[1].index};
        for (int i = 1; i < n - 1; ++i) {
            double dist = abs(points[i + 1].z - points[i].z);
            if (dist < minDist) {
                minDist = dist;
                result = {points[i].index, points[i + 1].index};
            }
        }
        cout << fixed << setprecision(5) << minDist << '\n';
        cout << result.first << ' ' << result.second << '\n';
        return 0;
    } else if (sameX && sameZ) {
        // Точки на оси y
        sort(points.begin(), points.end(), [](const Point3D& a, const Point3D& b) {
            return a.y < b.y;
        });
        double minDist = abs(points[1].y - points[0].y);
        pair<int, int> result = {points[0].index, points[1].index};
        for (int i = 1; i < n - 1; ++i) {
            double dist = abs(points[i + 1].y - points[i].y);
            if (dist < minDist) {
                minDist = dist;
                result = {points[i].index, points[i + 1].index};
            }
        }
        cout << fixed << setprecision(5) << minDist << '\n';
        cout << result.first << ' ' << result.second << '\n';
        return 0;
    } else if (sameY && sameZ) {
        // Точки на оси x
        sort(points.begin(), points.end(), [](const Point3D& a, const Point3D& b) {
            return a.x < b.x;
        });
        double minDist = abs(points[1].x - points[0].x);
        pair<int, int> result = {points[0].index, points[1].index};
        for (int i = 1; i < n - 1; ++i) {
            double dist = abs(points[i + 1].x - points[i].x);
            if (dist < minDist) {
                minDist = dist;
                result = {points[i].index, points[i + 1].index};
            }
        }
        cout << fixed << setprecision(5) << minDist << '\n';
        cout << result.first << ' ' << result.second << '\n';
        return 0;
    }

    // Сортировка точек по x
    sort(points.begin(), points.end(), [](const Point3D& a, const Point3D& b) {
        return a.x < b.x;
    });

    pair<int, int> result;
    double minDist = closestPair(points, result);

    cout << fixed << setprecision(5) << minDist << '\n';
    cout << result.first << ' ' << result.second << '\n';

    return 0;
}