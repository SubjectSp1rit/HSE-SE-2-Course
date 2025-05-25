#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <string_view>
#include <vector>

namespace utils {
    using Clock = std::chrono::steady_clock;
    using Milli = std::chrono::duration<double, std::milli>;
}

static std::size_t charCompCnt = 0;
inline void resetCharCounter() noexcept { charCompCnt = 0; }
inline std::size_t charComparisons()    noexcept { return charCompCnt; }

class StringGenerator {
public:
    explicit StringGenerator(std::uint32_t seed = std::random_device{}())
        : rng(seed), lenDist(10, 200), charDist(0, alphabet.size() - 1) {
        fillBases();
    }

    enum class Kind {Random, Reverse, AlmostSorted};

    std::vector<std::string> makeSample(std::size_t n, Kind kind) const {
        if (n > 3000 || n == 0) throw std::invalid_argument("размер данных должен быть 1..3000");
        const auto& src = base(kind);
        return {src.begin(), src.begin() + static_cast<long>(n)};
    }

private:
    static constexpr std::string_view alphabet =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "!@#%:;^&*()-.";

    mutable std::mt19937 rng;
    std::uniform_int_distribution<int> lenDist;
    std::uniform_int_distribution<int> charDist;

    std::vector<std::string> baseRandom, baseReverse, baseAlmost;

    std::string randomString() {
        std::string s(lenDist(rng), '\0');
        for (char& c : s) c = alphabet[charDist(rng)];
        return s;
    }

    void fillBases() {
        baseRandom.reserve(3000);
        for (int i = 0; i < 3000; ++i) baseRandom.push_back(randomString());

        baseReverse = baseRandom;
        std::sort(baseReverse.begin(), baseReverse.end(), std::greater<>());
        baseAlmost = baseReverse;
        std::uniform_int_distribution<int> idx(0, 2999);
        for (int k = 0; k < 100; ++k)
            std::swap(baseAlmost[idx(rng)], baseAlmost[idx(rng)]);
    }

    const std::vector<std::string>& base(Kind k) const {
        switch (k) {
            case Kind::Random:       return baseRandom;
            case Kind::Reverse:      return baseReverse;
            case Kind::AlmostSorted: return baseAlmost;
        }
        throw std::logic_error("Неизвестный тип генерации случайных массивов данных");
    }
};

[[nodiscard]] inline int charAt(std::string_view s, std::size_t d) {
    return d < s.size() ? static_cast<unsigned char>(s[d]) : -1;
}

[[nodiscard]] inline bool strLess(const std::string& a, const std::string& b) {
    std::size_t i = 0, lim = std::min(a.size(), b.size());
    while (i < lim) {
        ++charCompCnt;
        if (a[i] < b[i]) return true;
        if (a[i] > b[i]) return false;
        ++i;
    }
    ++charCompCnt;
    return a.size() < b.size();
}

// STRING MERGE SORT
namespace lcp_merge {
    inline int lcpPrefix(std::string_view a, std::string_view b, int start = 0) {
        int i = start, lim = static_cast<int>(std::min(a.size(), b.size()));
        while (i < lim && a[i] == b[i]) { ++charCompCnt; ++i; }
        if (i < lim) ++charCompCnt;
        return i;
    }
    struct Node { std::string s; int lcp; };

    std::pair<bool,int> lcpCompare(const std::string& a, const std::string& b, int offset) {
        int h = lcpPrefix(a, b, offset);
        if (h == a.size() && h == b.size()) return {false, h};
        if (h == a.size())                  return {true,  h};
        if (h == b.size())                  return {false, h};
        return {a[h] < b[h], h};
    }

    std::vector<Node> merge(const std::vector<Node>& p, const std::vector<Node>& q) {
        std::vector<Node> r; r.reserve(p.size() + q.size());
        auto pp = p, qq = q;
        std::size_t i = 0, j = 0;
        while (i < pp.size() && j < qq.size()) {
            int ki = pp[i].lcp, pj = qq[j].lcp;
            if (ki > pj) {
                r.push_back(pp[i++]);
            } else if (ki < pj) {
                r.push_back(qq[j++]);
            } else {
                auto [less, h] = lcpCompare(pp[i].s, qq[j].s, ki);
                if (less) { r.push_back(pp[i++]); qq[j].lcp = h; }
                else      { r.push_back(qq[j++]); pp[i].lcp = h; }
            }
        }
        while (i < pp.size()) r.push_back(pp[i++]);
        while (j < qq.size()) r.push_back(qq[j++]);
        return r;
    }

    std::vector<Node> sortRec(const std::vector<std::string>& a, std::size_t l, std::size_t r) {
        if (r - l == 1) return {Node{a[l], 0}};
        std::size_t m = l + (r - l)/2;
        auto left  = sortRec(a, l, m);
        auto right = sortRec(a, m, r);
        return merge(left, right);
    }

    inline void sort(std::vector<std::string>& arr) {
        auto nodes = sortRec(arr, 0, arr.size());
        for (std::size_t i = 0; i < arr.size(); ++i) arr[i] = std::move(nodes[i].s);
    }
}

// Стандартный MERGE SORT
namespace std_merge {
    void merge(std::vector<std::string>& a, std::vector<std::string>& aux,
               int lo, int mid, int hi) {
        int i = lo, j = mid + 1;
        for (int k = lo; k <= hi; ++k) aux[k] = a[k];

        for (int k = lo; k <= hi; ++k) {
            if (i > mid)                     a[k] = std::move(aux[j++]);
            else if (j > hi)                a[k] = std::move(aux[i++]);
            else if (strLess(aux[j], aux[i])) a[k] = std::move(aux[j++]);
            else                             a[k] = std::move(aux[i++]);
        }
    }

    void sortRec(std::vector<std::string>& a, std::vector<std::string>& aux,
                 int lo, int hi) {
        if (hi <= lo) return;
        int mid = lo + (hi - lo) / 2;
        sortRec(a, aux, lo, mid);
        sortRec(a, aux, mid + 1, hi);
        if (!strLess(a[mid + 1], a[mid])) return;
        merge(a, aux, lo, mid, hi);
    }

    inline void sort(std::vector<std::string>& a) {
        if (a.empty()) return;
        std::vector<std::string> aux(a.size());
        sortRec(a, aux, 0, static_cast<int>(a.size()) - 1);
    }
}

// Стандартный QUICK SORT
namespace std_quick {
    void quick(std::vector<std::string>& a, int lo, int hi) {
        if (lo >= hi) return;
        int i = lo + 1, j = hi;
        const std::string pivot = a[lo];

        while (true) {
            while (i <= hi && strLess(a[i], pivot)) ++i;
            while (strLess(pivot, a[j]))           --j;
            if (i >= j) break;
            std::swap(a[i++], a[j--]);
        }
        std::swap(a[lo], a[j]);
        quick(a, lo, j - 1);
        quick(a, j + 1, hi);
    }

    inline void sort(std::vector<std::string>& a) {
        if (!a.empty()) quick(a, 0, static_cast<int>(a.size()) - 1);
    }
}

// STRING QUICK SORT
namespace str_quick {
    void qs(std::vector<std::string>& a, int lo, int hi, std::size_t d) {
        if (hi <= lo) return;
        int lt = lo, gt = hi;
        int v = charAt(a[lo], d);
        for (int i = lo + 1; i <= gt;) {
            int t = charAt(a[i], d);
            ++charCompCnt;
            if (t < v)       std::swap(a[lt++], a[i++]);
            else if (t > v)  std::swap(a[i], a[gt--]);
            else             ++i;
        }
        qs(a, lo, lt - 1, d);
        if (v >= 0) qs(a, lt, gt, d + 1);
        qs(a, gt + 1, hi, d);
    }
    inline void sort(std::vector<std::string>& a) {
        if (!a.empty()) qs(a, 0, static_cast<int>(a.size()) - 1, 0);
    }
}

// MSD RADIX
namespace msd_radix {
    constexpr int R = 256;
    [[nodiscard]] inline int bucket(std::string_view s, std::size_t d) {
        return d < s.size() ? static_cast<unsigned char>(s[d]) + 1 : 0;
    }
    void sortRec(std::vector<std::string>& a, std::vector<std::string>& aux,
                 int lo, int hi, std::size_t d) {
        if (hi <= lo) return;
        std::array<int,R+2> cnt{};
        for (int i = lo; i <= hi; ++i) ++cnt[bucket(a[i], d)+1];
        for (int r = 0; r < R+1; ++r) cnt[r+1] += cnt[r];
        for (int i = lo; i <= hi; ++i) aux[cnt[bucket(a[i], d)]++] = std::move(a[i]);
        for (int i = lo; i <= hi; ++i) a[i] = std::move(aux[i - lo]);
        for (int r = 0; r < R+1; ++r)
            sortRec(a, aux, lo + cnt[r], lo + cnt[r+1] - 1, d+1);
    }
    inline void sort(std::vector<std::string>& a) {
        std::vector<std::string> aux(a.size());
        sortRec(a, aux, 0, static_cast<int>(a.size()) - 1, 0);
    }
}

// MSD RADIX QUICK SORT
namespace msd_radix_q {
    constexpr int R = 256;
    [[nodiscard]] inline int bucket(std::string_view s, std::size_t d) {
        return d < s.size() ? static_cast<unsigned char>(s[d]) + 1 : 0;
    }
    void sortRec(std::vector<std::string>& a, std::vector<std::string>& aux,
                 int lo, int hi, std::size_t d) {
        if (hi - lo + 1 < 74) { str_quick::qs(a, lo, hi, d); return; }
        std::array<int,R+2> cnt{};
        for (int i = lo; i <= hi; ++i) ++cnt[bucket(a[i], d)+1];
        for (int r = 0; r < R+1; ++r) cnt[r+1] += cnt[r];
        for (int i = lo; i <= hi; ++i) aux[cnt[bucket(a[i], d)]++] = std::move(a[i]);
        for (int i = lo; i <= hi; ++i) a[i] = std::move(aux[i - lo]);
        for (int r = 0; r < R+1; ++r)
            sortRec(a, aux, lo + cnt[r], lo + cnt[r+1] - 1, d+1);
    }
    inline void sort(std::vector<std::string>& a) {
        std::vector<std::string> aux(a.size());
        sortRec(a, aux, 0, static_cast<int>(a.size()) - 1, 0);
    }
}

class StringSortTester {
public:
    enum class Algo {
        LcpMerge, TernaryQuick, MsdRadix, MsdRadixQuick, StdQuick, StdMerge
    };

    struct Result { std::chrono::duration<double, std::milli> time; std::size_t comps; };

    [[nodiscard]] Result run(Algo alg, std::vector<std::string> data) const {
        resetCharCounter();
        auto t0 = std::chrono::steady_clock::now();

        switch (alg) {
            case Algo::LcpMerge:      lcp_merge::sort(data);   break;
            case Algo::TernaryQuick:  str_quick::sort(data);   break;
            case Algo::MsdRadix:      msd_radix::sort(data);   break;
            case Algo::MsdRadixQuick: msd_radix_q::sort(data); break;
            case Algo::StdQuick:      std_quick::sort(data);   break;
            case Algo::StdMerge:      std_merge::sort(data);   break;
        }

        auto dt = std::chrono::steady_clock::now() - t0;
        return {std::chrono::duration<double, std::milli>(dt), charComparisons()};
    }
};

int main() {
    StringGenerator gen(42);
    StringSortTester tester;

    for (std::size_t n = 100; n <= 3000; n += 100) {
        auto sample = gen.makeSample(n, StringGenerator::Kind::Reverse); /* Random
                                                                                       AlmostSorted
                                                                                       Reverse */
        auto res = tester.run(StringSortTester::Algo::MsdRadixQuick, sample); /* LcpMerge (STRING MERGE SORT)
                                                                                           TernaryQuick (STRING QUICK SORT)
                                                                                           MsdRadix
                                                                                           MsdRadixQuick (с переключением)
                                                                                           StdQuick (стандартный QUICK SORT)
                                                                                           StdMerge (стандартный MERGE SORT)*/
        // Красивый вывод для консоли
        std::cout << "n=" << n << "  time=" << res.time.count() << " ms  charCmp=" << res.comps << '\n';

        // Удобный вывод для анализа в excel
        //std::cout << n << " " << res.time.count() << " " << res.comps << "\n";
    }
}
