#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <ctime>
#include <fstream>

// Глобальные переменные

std::vector<int> database;

int num_readers = 0;
int num_writers = 0;
int reader_operations = 5;  // Количество операций для каждого читателя
int writer_operations = 5;  // Количество операций для каждого писателя

pthread_mutex_t mutex_db = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrier;  // Барьер синхронизации

int active_readers = 0;  // Количество активных читателей
bool writer_flag = false;  // Флаг, указывающий на активность писателя

// Мьютекс для вывода
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
std::ofstream outfile("output.txt");

// Функции ввода-вывода

void print_line(const std::string &line) {
    pthread_mutex_lock(&print_mutex);
    std::cout << line << std::endl;
    outfile << line << std::endl;
    pthread_mutex_unlock(&print_mutex);
}

// Генерация случайных значений
int get_random_index(int n) {
    return rand() % n;
}

int get_random_value() {
    return rand() % 1000;
}

// Потоки

void* reader_thread(void* arg) {
    int id = *((int*)arg);
    for (int i = 0; i < reader_operations; i++) {
        pthread_mutex_lock(&mutex_db);

        while (writer_flag) {
            // Ждем писателя через барьер
            pthread_mutex_unlock(&mutex_db);
            pthread_barrier_wait(&barrier);
            pthread_mutex_lock(&mutex_db);
        }

        // Читатель начинает читать
        active_readers++;
        pthread_mutex_unlock(&mutex_db);

        int idx = get_random_index(database.size());
        int value = database[idx];
        std::string msg = "[Читатель " + std::to_string(id) + "] читает: индекс="
                          + std::to_string(idx) + ", значение=" + std::to_string(value);
        print_line(msg);

        pthread_mutex_lock(&mutex_db);
        active_readers--;
        pthread_mutex_unlock(&mutex_db);

        usleep(100000);
    }
    return nullptr;
}

void* writer_thread(void* arg) {
    int id = *((int*)arg);
    for (int i = 0; i < writer_operations; i++) {
        pthread_mutex_lock(&mutex_db);

        while (active_readers > 0) {
            // Ждем завершения всех читателей через барьер
            pthread_mutex_unlock(&mutex_db);
            pthread_barrier_wait(&barrier);
            pthread_mutex_lock(&mutex_db);
        }

        // Писатель устанавливает флаг
        writer_flag = true;
        pthread_mutex_unlock(&mutex_db);

        int idx = get_random_index(database.size());
        int old_value = database[idx];
        int new_value = get_random_value();

        database[idx] = new_value;
        std::sort(database.begin(), database.end());

        std::string msg = "[Писатель " + std::to_string(id) + "] записал: старый_индекс="
                          + std::to_string(idx) + ", старое_значение=" + std::to_string(old_value)
                          + ", новое_значение=" + std::to_string(new_value);
        print_line(msg);

        pthread_mutex_lock(&mutex_db);
        writer_flag = false;  // Сбрасываем флаг писателя
        pthread_mutex_unlock(&mutex_db);

        usleep(200000);
    }
    return nullptr;
}

// main

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    srand((unsigned)time(nullptr));

    // Ввод данных
    std::cout << "Введите количество записей в базе данных: ";
    int n;
    std::cin >> n;
    std::cout << "Введите количество читателей: ";
    std::cin >> num_readers;
    std::cout << "Введите количество писателей: ";
    std::cin >> num_writers;

    database.resize(n);
    for (int i = 0; i < n; i++) {
        database[i] = i;
    }

    // Инициализация барьера для синхронизации
    pthread_barrier_init(&barrier, nullptr, num_readers + num_writers);

    std::vector<pthread_t> readers(num_readers);
    std::vector<pthread_t> writers(num_writers);
    std::vector<int> readers_id(num_readers), writers_id(num_writers);

    for (int i = 0; i < num_readers; i++) {
        readers_id[i] = i + 1;
        pthread_create(&readers[i], nullptr, reader_thread, &readers_id[i]);
    }

    for (int i = 0; i < num_writers; i++) {
        writers_id[i] = i + 1;
        pthread_create(&writers[i], nullptr, writer_thread, &writers_id[i]);
    }

    for (int i = 0; i < num_readers; i++) {
        pthread_join(readers[i], nullptr);
    }
    for (int i = 0; i < num_writers; i++) {
        pthread_join(writers[i], nullptr);
    }

    pthread_barrier_destroy(&barrier);

    print_line("Все потоки завершили работу. Итоговый массив:");
    for (int i = 0; i < (int)database.size(); i++) {
        std::cout << database[i] << " ";
        outfile << database[i] << " ";
    }
    std::cout << "\n";
    outfile << "\n";

    return 0;
}