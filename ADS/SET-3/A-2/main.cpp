#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <random>

class ArrayGenerator {
public:
    [[nodiscard]] int getMaxSize() const {
        return maxSize;
    }

    explicit ArrayGenerator(int maxSize, int rangeMin, int rangeMax)
        : maxSize(maxSize), rangeMin(rangeMin), rangeMax(rangeMax), seed(std::random_device{}()) {
        generateRandomArray();
        generateReversedArray();
        generateAlmostSortedArray();
    }

    std::vector<int> getRandomArray(int size) const {
        return std::vector<int>(randomArray.begin(), randomArray.begin() + size);
    }

    std::vector<int> getReversedArray(int size) const {
        return std::vector<int>(reversedArray.begin(), reversedArray.begin() + size);
    }

    std::vector<int> getAlmostSortedArray(int size) const {
        return std::vector<int>(semisortedArray.begin(), semisortedArray.begin() + size);
    }

private:
    int maxSize;
    int rangeMin, rangeMax;
    std::vector<int> randomArray;
    std::vector<int> reversedArray;
    std::vector<int> semisortedArray;
    std::mt19937 seed;

    void generateRandomArray() {
        std::uniform_int_distribution<int> dist(rangeMin, rangeMax);
        randomArray.resize(maxSize);
        for (int& val : randomArray) {
            val = dist(seed);
        }
    }

    void generateReversedArray() {
        reversedArray = randomArray;
        std::sort(reversedArray.rbegin(), reversedArray.rend());
    }

    void generateAlmostSortedArray() {
        semisortedArray = randomArray;
        std::sort(semisortedArray.begin(), semisortedArray.end());
        std::uniform_int_distribution<int> dist(0, maxSize - 1);
        for (int i = 0; i < maxSize / 100; ++i) {
            int idx1 = dist(seed);
            int idx2 = dist(seed);
            std::swap(semisortedArray[idx1], semisortedArray[idx2]);
        }
    }
};

class SortTester {
public:
    SortTester() = default;

    void testSorts(const ArrayGenerator& generator, const std::string& outputFile) {
        std::ofstream out(outputFile);

        out << "n\tRandom_Merge\tReversed_Merge\tAlmostSorted_Merge\t"
                "Random_Hybrid\tReversed_Hybrid\tAlmostSorted_Hybrid\n";

        int maxSize = generator.getMaxSize();
        for (int size = 500; size <= maxSize; size += 100) {
            std::vector<int> randomArray = generator.getRandomArray(size);
            std::vector<int> reversedArray = generator.getReversedArray(size);
            std::vector<int> almostSortedArray = generator.getAlmostSortedArray(size);

            out << size << "\t";
            out << measureTime(randomArray, mergeSort) << "\t";
            out << measureTime(reversedArray, mergeSort) << "\t";
            out << measureTime(almostSortedArray, mergeSort) << "\t";
            out << measureTime(randomArray, hybridMergeSort) << "\t";
            out << measureTime(reversedArray, hybridMergeSort) << "\t";
            out << measureTime(almostSortedArray, hybridMergeSort) << "\n";
        }

        out.close();
        std::cout << "Программа завершила свое выполнение успешно!" << '\n';
    }

    inline static int THRESHOLD;
private:
    static void mergeSort(std::vector<int>& arr, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }

    static void hybridMergeSort(std::vector<int>& arr, int left, int right) {
        if (right - left + 1 <= THRESHOLD) {
            insertionSort(arr, left, right);
            return;
        }
        int mid = left + (right - left) / 2;
        hybridMergeSort(arr, left, mid);
        hybridMergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }

    static void merge(std::vector<int>& arr, int left, int mid, int right) {
        int n1 = mid - left + 1, n2 = right - mid;
        std::vector<int> L(n1), R(n2);
        for (int i = 0; i < n1; ++i) L[i] = arr[left + i];
        for (int i = 0; i < n2; ++i) R[i] = arr[mid + 1 + i];
        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
    }

    static void insertionSort(std::vector<int>& arr, int left, int right) {
        for (int i = left + 1; i <= right; ++i) {
            int key = arr[i];
            int j = i - 1;
            while (j >= left && arr[j] > key) {
                arr[j + 1] = arr[j];
                --j;
            }
            arr[j + 1] = key;
        }
    }

    template <typename SortFunc>
    long long measureTime(std::vector<int> arr, SortFunc sortFunc) {
        auto start = std::chrono::high_resolution_clock::now();
        sortFunc(arr, 0, arr.size() - 1);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int maxSize = 10000;
    const int THRESHOLD = 15;
    SortTester::THRESHOLD = THRESHOLD;

    ArrayGenerator generator(maxSize, 0, 6000);
    SortTester tester;
    tester.testSorts(generator, "output.txt");

    return 0;
}