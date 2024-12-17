#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <vector>
#include <iterator>

// Глобальные переменные

std::vector<int> database;

int num_readers = 0;
int num_writers = 0;

pthread_mutex_t mutex_db = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_readers = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_writer = PTHREAD_COND_INITIALIZER;

int read_count = 0;
bool writer_active = false;

// Мьютекс для вывода
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

int reader_operations = 5;
int writer_operations = 5;

// Функции-утилиты

int get_random_index() {
    return rand() % (int)database.size();
}

int get_random_value() {
    return rand() % 1000;
}

// Синхронизация

void start_read() {
    pthread_mutex_lock(&mutex_db);
    while (writer_active) {
        pthread_cond_wait(&cond_readers, &mutex_db);
    }
    read_count++;
    pthread_mutex_unlock(&mutex_db);
}

void end_read() {
    pthread_mutex_lock(&mutex_db);
    read_count--;
    if (read_count == 0) {
        pthread_cond_signal(&cond_writer);
    }
    pthread_mutex_unlock(&mutex_db);
}

void start_write() {
    pthread_mutex_lock(&mutex_db);
    while (writer_active || read_count > 0) {
        pthread_cond_wait(&cond_writer, &mutex_db);
    }
    writer_active = true;
    pthread_mutex_unlock(&mutex_db);
}

void end_write() {
    pthread_mutex_lock(&mutex_db);
    writer_active = false;
    // Сначала будим читателей
    pthread_cond_broadcast(&cond_readers);
    // Потом писателей (одного из них)
    pthread_cond_signal(&cond_writer);
    pthread_mutex_unlock(&mutex_db);
}

// Потоки

void* reader_thread(void* arg) {
    int id = *((int*)arg);
    for (int i = 0; i < reader_operations; i++) {
        start_read();
        int idx = get_random_index();
        int value = database[idx];

        // Вывод под защитой print_mutex
        pthread_mutex_lock(&print_mutex);
        std::cout << "[Читатель " << id << "] читает: индекс=" << idx
                  << ", значение=" << value << std::endl;
        pthread_mutex_unlock(&print_mutex);

        end_read();
        usleep(100000);
    }
    return nullptr;
}

void* writer_thread(void* arg) {
    int id = *((int*)arg);
    for (int i = 0; i < writer_operations; i++) {
        start_write();
        int idx = get_random_index();
        int old_value = database[idx];
        int new_value = get_random_value();
        database[idx] = new_value;
        std::sort(database.begin(), database.end());

        // Находим новый индекс вставленного значения
        int new_idx = (int)(std::find(database.begin(), database.end(), new_value) - database.begin());

        // Вывод под защитой print_mutex
        pthread_mutex_lock(&print_mutex);
        std::cout << "[Писатель " << id << "] записал: старый_индекс=" << idx
                  << ", старое_значение=" << old_value
                  << ", новое_значение=" << new_value
                  << ", новый_индекс=" << new_idx << std::endl;
        pthread_mutex_unlock(&print_mutex);

        end_write();
        usleep(200000);
    }
    return nullptr;
}

// main

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    srand((unsigned)time(nullptr));

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

    std::vector<pthread_t> readers(num_readers);
    std::vector<pthread_t> writers(num_writers);
    std::vector<int> readers_id(num_readers), writers_id(num_writers);

    for (int i = 0; i < num_readers; i++) {
        readers_id[i] = i+1;
        pthread_create(&readers[i], nullptr, reader_thread, &readers_id[i]);
    }

    for (int i = 0; i < num_writers; i++) {
        writers_id[i] = i+1;
        pthread_create(&writers[i], nullptr, writer_thread, &writers_id[i]);
    }

    for (int i = 0; i < num_readers; i++) {
        pthread_join(readers[i], nullptr);
    }
    for (int i = 0; i < num_writers; i++) {
        pthread_join(writers[i], nullptr);
    }

    pthread_mutex_lock(&print_mutex);
    std::cout << "Все потоки завершили работу. Итоговый массив:\n";
    for (int i = 0; i < (int)database.size(); i++) {
        std::cout << database[i] << " ";
    }
    std::cout << "\n";
    pthread_mutex_unlock(&print_mutex);

    return 0;
}
