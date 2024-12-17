#include <iostream>
#include <omp.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

// Глобальные переменные
std::vector<int> database;  // База данных
int active_readers = 0;     // Счётчик активных читателей
bool writer_active = false; // Флаг активности писателя

int num_readers, num_writers, reader_operations = 5, writer_operations = 5;
std::ofstream outfile("output.txt");

// Мьютекс для вывода
omp_lock_t print_lock;

// Функции вывода
void print_line(const std::string &line) {
    omp_set_lock(&print_lock);
    std::cout << line << std::endl;
    outfile << line << std::endl;
    omp_unset_lock(&print_lock);
}

// Генерация случайных данных
int get_random_index(int n) {
    return rand() % n;
}

int get_random_value() {
    return rand() % 1000;
}

// Функции читателя и писателя

void reader_thread(int id, int n) {
    for (int i = 0; i < reader_operations; i++) {
        while (writer_active) {
            // Ждём, пока писатель завершит работу
            usleep(100);
        }

        #pragma omp atomic
        active_readers++;

        int idx = get_random_index(n);
        int value = database[idx];

        print_line("[Читатель " + std::to_string(id) + "] читает: индекс=" +
                   std::to_string(idx) + ", значение=" + std::to_string(value));

        #pragma omp atomic
        active_readers--;

        usleep(100000);
    }
}

void writer_thread(int id, int n) {
    for (int i = 0; i < writer_operations; i++) {
        // Ждём, пока активные читатели завершат работу
        while (active_readers > 0) {
            usleep(100);
        }

        writer_active = true;

        int idx = get_random_index(n);
        int old_value = database[idx];
        int new_value = get_random_value();

        database[idx] = new_value;
        std::sort(database.begin(), database.end());

        print_line("[Писатель " + std::to_string(id) + "] записал: старый_индекс=" +
                   std::to_string(idx) + ", старое_значение=" + std::to_string(old_value) +
                   ", новое_значение=" + std::to_string(new_value));

        writer_active = false;

        usleep(200000);
    }
}

// main
int main() {
    std::ios::sync_with_stdio(false);
    srand(time(nullptr));

    std::cout << "Введите количество записей в базе данных: ";
    int n;
    std::cin >> n;
    std::cout << "Введите количество читателей: ";
    std::cin >> num_readers;
    std::cout << "Введите количество писателей: ";
    std::cin >> num_writers;

    // Инициализация базы данных
    database.resize(n);
    for (int i = 0; i < n; i++) {
        database[i] = i;
    }

    // Инициализация мьютекса для вывода
    omp_init_lock(&print_lock);

    #pragma omp parallel num_threads(num_readers + num_writers)
    {
        int thread_id = omp_get_thread_num();

        if (thread_id < num_readers) {
            reader_thread(thread_id + 1, n);
        } else {
            writer_thread(thread_id - num_readers + 1, n);
        }
    }

    // Итоговый вывод базы данных
    print_line("Все потоки завершили работу. Итоговый массив:");
    for (int value : database) {
        std::cout << value << " ";
        outfile << value << " ";
    }
    std::cout << std::endl;
    outfile << std::endl;

    // Освобождение мьютекса
    omp_destroy_lock(&print_lock);

    return 0;
}
