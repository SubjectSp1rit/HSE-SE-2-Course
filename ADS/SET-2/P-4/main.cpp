#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>

const double EPS = 1e-8;

struct Segment {
    double time;
    double speed;
};

struct Event {
    double time;
    int train;
    int segmentIndex;
};

// Чтение входных данных
void readInput(double &L, int &n, std::vector<Segment> &segments, std::vector<double> &cumT, std::vector<double> &cumS) {
    std::cin >> L >> n;
    segments.resize(n);
    cumT.resize(n + 1, 0.0);
    cumS.resize(n + 1, 0.0);
    for (int i = 0; i < n; ++i) {
        std::cin >> segments[i].time >> segments[i].speed;
        cumT[i + 1] = cumT[i] + segments[i].time;
        cumS[i + 1] = cumS[i] + segments[i].time * segments[i].speed;
    }
}

// Проверка расстояния между поездами
bool isSafe(double D, double L, int n, const std::vector<Segment> &segments, const std::vector<double> &cumT, const std::vector<double> &cumS) {
    double T_total = cumT[n];
    std::vector<double> times;

    for (int i = 0; i <= n; ++i) {
        double t = cumT[i];
        if (t >= D && t <= T_total + EPS)
            times.push_back(t);
        double t2 = D + cumT[i];
        if (t2 >= D - EPS && t2 <= T_total + EPS)
            times.push_back(t2);
    }

    sort(times.begin(), times.end());
    times.erase(unique(times.begin(), times.end()), times.end());

    for (size_t k = 0; k + 1 < times.size(); ++k) {
        double t0 = times[k];
        double t1 = times[k + 1];

        int seg1 = std::lower_bound(cumT.begin(), cumT.end(), t0 + EPS) - cumT.begin() - 1;
        double a1 = segments[seg1].speed;
        double s1_t0 = cumS[seg1] + (t0 - cumT[seg1]) * a1;

        double t0_rel = t0 - D;
        int seg2 = std::lower_bound(cumT.begin(), cumT.end(), t0_rel + EPS) - cumT.begin() - 1;
        double a2 = 0.0, s2_t0 = 0.0;
        if (t0_rel >= 0 && seg2 >= 0 && seg2 < n) {
            a2 = segments[seg2].speed;
            s2_t0 = cumS[seg2] + (t0_rel - cumT[seg2]) * a2;
        }

        double delta_s = s2_t0 - s1_t0;
        double A = a2 - a1;

        double dist_min = fabs(delta_s);
        if (fabs(A) > EPS) {
            double t_candidate = t0 - delta_s / A;
            if (t_candidate >= t0 - EPS && t_candidate <= t1 + EPS) {
                double s1_candidate = s1_t0 + a1 * (t_candidate - t0);
                double s2_candidate = s2_t0 + a2 * (t_candidate - t0);
                double dist_candidate = fabs(s2_candidate - s1_candidate);
                dist_min = std::min(dist_min, dist_candidate);
            }
        }

        double s1_t1 = s1_t0 + a1 * (t1 - t0);
        double s2_t1 = s2_t0 + a2 * (t1 - t0);
        dist_min = std::min(dist_min, fabs(s2_t1 - s1_t1));

        if (dist_min < L - EPS) {
            return false;
        }
    }
    return true;
}

double findMinimumInterval(double L, int n, const std::vector<Segment> &segments, const std::vector<double> &cum_t, const std::vector<double> &cum_s) {
    double T_total = cum_t[n];
    double left = 0.0, right = T_total;

    for (int i = 0; i < 100; ++i) {
        double D = (left + right) / 2.0;
        if (isSafe(D, L, n, segments, cum_t, cum_s))
            right = D;
        else
            left = D;
    }
    return right;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    double L;
    int n;
    std::vector<Segment> segments;
    std::vector<double> cumT, cumS;

    readInput(L, n, segments, cumT, cumS);

    double minInterval = findMinimumInterval(L, n, segments, cumT, cumS);

    std::cout << std::fixed << std::setprecision(3) << minInterval << '\n';

    return 0;
}