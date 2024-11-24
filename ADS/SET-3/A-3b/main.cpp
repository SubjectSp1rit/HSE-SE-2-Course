#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <chrono>
#include <random>
#include <algorithm>

class ArrayGenerator {
public:
    ArrayGenerator(int maxSize, int rangeMin, int rangeMax)
        : maxSize(maxSize), rangeMin(rangeMin), rangeMax(rangeMax), rng(std::random_device{}()) {
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
        return std::vector<int>(almostSortedArray.begin(), almostSortedArray.begin() + size);
    }

private:
    int maxSize;
    int rangeMin, rangeMax;
    std::vector<int> randomArray;
    std::vector<int> reversedArray;
    std::vector<int> almostSortedArray;
    std::mt19937 rng;

    void generateRandomArray() {
        std::uniform_int_distribution<int> dist(rangeMin, rangeMax);
        randomArray.resize(maxSize);
        for (int& val : randomArray) {
            val = dist(rng);
        }
    }

    void generateReversedArray() {
        std::uniform_int_distribution<int> dist(rangeMin, rangeMax);
        reversedArray.resize(maxSize);
        for (int& val : reversedArray) {
            val = dist(rng);
        }
        std::ranges::sort(reversedArray, std::ranges::greater{});
        // for (int i = 0; i < maxSize; ++i) {
        //     std::cout << reversedArray[i] << " ";
        // }
        //std::sort(reversedArray.rbegin(), reversedArray.rend());
    }

    void generateAlmostSortedArray() {
        almostSortedArray.resize(maxSize);
        std::iota(almostSortedArray.begin(), almostSortedArray.end(), 1); // Полностью отсортированный массив
        std::shuffle(almostSortedArray.begin(), almostSortedArray.end(), rng); // Перемешиваем

        // Переставляем 20% элементов
        int swaps = maxSize * 0.2; // Количество перестановок
        std::uniform_int_distribution<int> dist(0, maxSize - 1);

        for (int i = 0; i < swaps; ++i) {
            int idx1 = dist(rng);
            int idx2 = dist(rng);
            std::swap(almostSortedArray[idx1], almostSortedArray[idx2]);
        }
    }
};

class SortTester {
public:
    SortTester() = default;

    void testIntrosort(const ArrayGenerator& generator, const std::string& outputFile) {
        std::ofstream out(outputFile);

        out << "Size\tRandom_quicksort\tReversed_quicksort\tSemisorted_quicksort\n";

        for (int size = 10000; size <= 100000; size += 100) {
            std::vector<int> randomArray = generator.getRandomArray(size);
            std::vector<int> reversedArray = generator.getReversedArray(size);
            std::vector<int> almostSortedArray = generator.getAlmostSortedArray(size);

            out << size << "\t";
            out << measureTime(randomArray, quickSort) << "\t";
            out << measureTime(reversedArray, quickSort) << "\t";
            out << measureTime(almostSortedArray, quickSort) << "\n";
        }

        out.close();
        std::cout << "Done" << '\n';
    }

private:
    static void introsort(std::vector<int>& arr, int begin, int end, int depth_limit) {
        int size = end - begin + 1;
        if (size < 16) {
            insertionSort(arr, begin, end);
            return;
        }
        if (depth_limit == 0) {
            heapSort(arr, begin, end);
            return;
        }
        int pivot = partition(arr, begin, end);
        introsort(arr, begin, pivot - 1, depth_limit - 1);
        introsort(arr, pivot + 1, end, depth_limit - 1);
    }

    static void quickSort(std::vector<int>& arr, int begin, int end, int depth_limit) {
        if (begin < end) {
            int pivot = partition(arr, begin, end);
            quickSort(arr, begin, pivot - 1, depth_limit);
            quickSort(arr, pivot + 1, end, depth_limit);
        }
    }

    static void insertionSort(std::vector<int>& arr, int begin, int end) {
        for (int i = begin + 1; i <= end; ++i) {
            int key = arr[i];
            int j = i - 1;
            while (j >= begin && arr[j] > key) {
                arr[j + 1] = arr[j];
                --j;
            }
            arr[j + 1] = key;
        }
    }

    static void heapify(std::vector<int>& arr, int begin, int end, int i) {
        int largest = i;
        int left = 2 * (i - begin) + 1 + begin;
        int right = 2 * (i - begin) + 2 + begin;
        if (left <= end && arr[left] > arr[largest])
            largest = left;
        if (right <= end && arr[right] > arr[largest])
            largest = right;
        if (largest != i) {
            std::swap(arr[i], arr[largest]);
            heapify(arr, begin, end, largest);
        }
    }

    static void heapSort(std::vector<int>& arr, int begin, int end) {
        for (int i = begin + (end - begin) / 2; i >= begin; --i)
            heapify(arr, begin, end, i);
        for (int i = end; i > begin; --i) {
            std::swap(arr[begin], arr[i]);
            heapify(arr, begin, i - 1, begin);
        }
    }

    static int medianOfThree(std::vector<int>& arr, int a, int b, int c) {
        if (arr[a] < arr[b]) {
            if (arr[b] < arr[c])
                return b;
            else if (arr[a] < arr[c])
                return c;
            else
                return a;
        } else {
            if (arr[a] < arr[c])
                return a;
            else if (arr[b] < arr[c])
                return c;
            else
                return b;
        }
    }

    static int partition(std::vector<int>& arr, int begin, int end) {
        int mid = begin + (end - begin) / 2;
        int pivotIndex = medianOfThree(arr, begin, mid, end);
        int pivotValue = arr[pivotIndex];
        std::swap(arr[pivotIndex], arr[end]);
        int storeIndex = begin;
        for (int i = begin; i < end; ++i) {
            if (arr[i] < pivotValue) {
                std::swap(arr[i], arr[storeIndex]);
                ++storeIndex;
            }
        }
        std::swap(arr[storeIndex], arr[end]);
        return storeIndex;
    }

    template <typename SortFunc>
    long long measureTime(std::vector<int>& arr, SortFunc sortFunc) {
        auto start = std::chrono::high_resolution_clock::now();
        //sortFunc(arr);
        sortFunc(arr, 0, arr.size() - 1, 2 * std::log2(arr.size()));
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    ArrayGenerator generator(100000, 0, 6000);
    SortTester tester;
    tester.testIntrosort(generator, "output.txt");
    return 0;
}