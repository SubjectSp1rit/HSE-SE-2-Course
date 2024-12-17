#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <vector>
#include <fstream>
#include <iterator>
#include <string>

// Глобальные переменные

std::vector<int> database;

int num_readers = 0;
int num_writers = 0;
int reader_operations = 5;
int writer_operations = 5;

// Синхронизация доступа к БД
pthread_mutex_t mutex_db = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_readers = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_writer = PTHREAD_COND_INITIALIZER;

int read_count = 0;
bool writer_active = false;

// Мьютекс для вывода
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
std::ofstream outfile("output.txt");

// Режим ввода
// 1 - ручной (консоль), 2 - случайный, 3 - из файла
int input_mode = 2;

// Для ручного или файлового ввода храним заранее операции
// Для читателей - просто индексы
std::vector<std::vector<int>> reader_indices;
// Для писателей - пара (индекс, значение)
std::vector<std::vector<std::pair<int,int>>> writer_ops;

// Функции ввода-вывода

void print_line(const std::string &line) {
    pthread_mutex_lock(&print_mutex);
    std::cout << line << std::endl;
    outfile << line << std::endl;
    pthread_mutex_unlock(&print_mutex);
}

// Случайный индекс и значение
int get_random_index(int n) {
    return rand() % n;
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
    pthread_cond_broadcast(&cond_readers);
    pthread_cond_signal(&cond_writer);
    pthread_mutex_unlock(&mutex_db);
}

// Потоки

void* reader_thread(void* arg) {
    int id = *((int*)arg);
    for (int i = 0; i < reader_operations; i++) {
        int idx;
        if (input_mode == 2) {
            // Случайный ввод
            idx = get_random_index((int)database.size());
        } else {
            // Ручной или файловый – индекс заранее считан
            idx = reader_indices[id-1][i];
        }

        start_read();
        int value = database[idx];
        std::string msg = "[Читатель " + std::to_string(id) + "] читает: индекс="
                          + std::to_string(idx) + ", значение=" + std::to_string(value);
        print_line(msg);
        end_read();

        usleep(100000);
    }
    return nullptr;
}

void* writer_thread(void* arg) {
    int id = *((int*)arg);
    for (int i = 0; i < writer_operations; i++) {
        int idx, new_value;
        if (input_mode == 2) {
            // Случайный ввод
            idx = get_random_index((int)database.size());
            new_value = get_random_value();
        } else {
            // Ручной или файл
            idx = writer_ops[id-1][i].first;
            new_value = writer_ops[id-1][i].second;
        }

        start_write();
        int old_value = database[idx];
        database[idx] = new_value;
        std::sort(database.begin(), database.end());

        // Находим новый индекс вставленного значения
        int new_idx = (int)(std::find(database.begin(), database.end(), new_value) - database.begin());

        std::string msg = "[Писатель " + std::to_string(id) + "] записал: старый_индекс="
                          + std::to_string(idx) + ", старое_значение=" + std::to_string(old_value)
                          + ", новое_значение=" + std::to_string(new_value)
                          + ", новый_индекс=" + std::to_string(new_idx);
        print_line(msg);
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

    std::cout << "Выберите режим ввода (1 - ручной, 2 - случайный, 3 - из файла): ";
    std::cin >> input_mode;

    if (input_mode == 1) {
        // Ручной ввод: запросим данные для всех операций сразу
        // Инициализируем структуры
        reader_indices.resize(num_readers, std::vector<int>(reader_operations));
        writer_ops.resize(num_writers, std::vector<std::pair<int,int>>(writer_operations));

        // Для читателей: просто индексы для чтения
        for (int r = 0; r < num_readers; r++) {
            for (int i = 0; i < reader_operations; i++) {
                std::cout << "[Ручной ввод] Читатель " << (r+1) << ", операция чтения " << (i+1)
                          << ": введите индекс для чтения [0.." << (n-1) << "]: ";
                std::cin >> reader_indices[r][i];
            }
        }

        // Для писателей: индекс и новое значение
        for (int w = 0; w < num_writers; w++) {
            for (int i = 0; i < writer_operations; i++) {
                std::cout << "[Ручной ввод] Писатель " << (w+1) << ", операция записи " << (i+1)
                          << ": введите индекс для записи [0.." << (n-1) << "]: ";
                int idx;
                std::cin >> idx;
                std::cout << "Введите новое значение для записи [0..999]: ";
                int val;
                std::cin >> val;
                writer_ops[w][i] = std::make_pair(idx, val);
            }
        }

    } else if (input_mode == 3) {
        // Ввод из файла
        // Файл должен содержать (num_readers * reader_operations) строк вида "R idx"
        // и (num_writers * writer_operations) строк вида "W idx val"

        std::cout << "Введите имя файла ввода: ";
        std::string filename;
        std::cin >> filename;

        std::ifstream infile(filename);
        if (!infile) {
            std::cerr << "Не удалось открыть файл ввода.\n";
            return 1;
        }

        reader_indices.resize(num_readers, std::vector<int>(reader_operations));
        writer_ops.resize(num_writers, std::vector<std::pair<int,int>>(writer_operations));

        // Считаем сначала все операции для читателей
        for (int r = 0; r < num_readers; r++) {
            for (int i = 0; i < reader_operations; i++) {
                char c;
                int idx;
                infile >> c >> idx;
                if (!infile || c != 'R') {
                    std::cerr << "Ошибка в формате входных данных для читателей.\n";
                    return 1;
                }
                reader_indices[r][i] = idx;
            }
        }

        // Затем считаем операции для писателей
        for (int w = 0; w < num_writers; w++) {
            for (int i = 0; i < writer_operations; i++) {
                char c;
                int idx, val;
                infile >> c >> idx >> val;
                if (!infile || c != 'W') {
                    std::cerr << "Ошибка в формате входных данных для писателей.\n";
                    return 1;
                }
                writer_ops[w][i] = std::make_pair(idx, val);
            }
        }

    }
    // Если режим 2 (случайный), никаких данных дополнительно не нужно

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

    {
        pthread_mutex_lock(&print_mutex);
        std::cout << "Все потоки завершили работу. Итоговый массив:\n";
        outfile << "Все потоки завершили работу. Итоговый массив:\n";
        for (int i = 0; i < (int)database.size(); i++) {
            std::cout << database[i] << " ";
            outfile << database[i] << " ";
        }
        std::cout << "\n";
        outfile << "\n";
        pthread_mutex_unlock(&print_mutex);
    }

    return 0;
}