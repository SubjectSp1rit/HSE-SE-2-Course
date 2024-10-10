#include <iostream>
#include <vector>
#include <algorithm>

struct Interval {
    int left;
    int right;

    Interval(int l = 0, int r = -1) {
        left = l;
        right = r;
    }

    size_t length() {
        return right - left + 1;
    }

    Interval overlap(const Interval& other) {
        int left = std::max(this->left, other.left);
        int right = std::min(this->right, other.right);
        if (left > right) return {0, -1};
        return {left, right};
    }

    bool operator<(const Interval& other) const {
        if (left != other.left) {
            return left < other.left;
        }
        return right < other.right;
    }
};

std::pair<Interval, int64_t> findOverlapBetweenSorted(std::vector<Interval>& leftIntervals, std::vector<Interval>& rightIntervals) {
    Interval maxOverlapBetween = Interval();
    int64_t maxOverlapLength = 0;

    int i = 0, j = 0;
    while (i < leftIntervals.size() && j < rightIntervals.size()) {
        Interval overlap = leftIntervals[i].overlap(rightIntervals[j]);
        int64_t overlap_len = overlap.length();
        if (overlap_len > maxOverlapLength) {
            maxOverlapBetween = overlap;
            maxOverlapLength = overlap_len;
        } else if (overlap_len == maxOverlapLength && overlap < maxOverlapBetween) {
            maxOverlapBetween = overlap;
        }

        if (leftIntervals[i].right < rightIntervals[j].right) {
            ++i;
        } else {
            ++j;
        }
    }

    return { maxOverlapBetween, maxOverlapLength };
}

std::pair<Interval, int> recursiveFindOverlap(std::vector<Interval>& intervals, int left, int right) {
    if (left == right) {
        return { Interval(), 0 };
    }

    int mid = left + (right - left) / 2;

    auto leftResult = recursiveFindOverlap(intervals, left, mid);
    auto rightResult = recursiveFindOverlap(intervals, mid + 1, right);

    std::vector<Interval> left_intervals(intervals.begin() + left, intervals.begin() + mid + 1);
    std::vector<Interval> right_intervals(intervals.begin() + mid + 1, intervals.begin() + right + 1);
    std::sort(left_intervals.begin(), left_intervals.end());
    std::sort(right_intervals.begin(), right_intervals.end());

    auto betweenResult = findOverlapBetweenSorted(left_intervals, right_intervals);

    if (leftResult.second > rightResult.second ||
        (leftResult.second == rightResult.second && leftResult.first < rightResult.first)) {
        if (leftResult.second > betweenResult.second ||
            (leftResult.second == betweenResult.second && leftResult.first < betweenResult.first)) {
            return leftResult;
        }
    } else {
        if (rightResult.second > betweenResult.second ||
            (rightResult.second == betweenResult.second && rightResult.first < betweenResult.first)) {
            return rightResult;
        }
    }

    return betweenResult;
}

std::pair<Interval, int> getLongestOverlap(std::vector<Interval>& intervals) {
    if (intervals.empty()) {
        return { Interval(), 0 };
    }
    intervals.erase(std::remove_if(intervals.begin(), intervals.end(),
                                   [](const Interval& i) { return i.left > i.right; }),
                    intervals.end());

    if (intervals.empty()) {
        return { Interval(), 0 };
    }

    return recursiveFindOverlap(intervals, 0, intervals.size() - 1);
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::vector<Interval> intervals;
    std::cin >> n;
    int x;
    int y;
    for (int i = 0; i < n; ++i) {
        std::cin >> x >> y;
        intervals.emplace_back(x, y);
    }

    auto [first, second] = getLongestOverlap(intervals);
    if (first.left == 0 && first.right == -1) {
        std::cout << 0 << '\n';
    } else {
        std::cout << second << '\n' << first.left << ' ' << first.right << '\n';
    }


    return 0;
}

