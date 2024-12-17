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
std::ofstream outfile;

// Режим ввода
// true - режим файла, false - случайный режим
bool file_mode = false;

// Данные для режима файла
std::vector<std::vector<int>> reader_indices;
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
        if (file_mode) {
            // Индекс уже считан из файла
            idx = reader_indices[id-1][i];
        } else {
            // Случайный режим
            idx = get_random_index((int)database.size());
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
        if (file_mode) {
            idx = writer_ops[id-1][i].first;
            new_value = writer_ops[id-1][i].second;
        } else {
            idx = get_random_index((int)database.size());
            new_value = get_random_value();
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

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);

    srand((unsigned)time(nullptr));

    if (argc == 5) {
        // Случайный режим: n R W output.txt
        int n;
        n = std::atoi(argv[1]);
        num_readers = std::atoi(argv[2]);
        num_writers = std::atoi(argv[3]);
        std::string output_filename = argv[4];
        outfile.open(output_filename);
        if (!outfile) {
            std::cerr << "Не удалось открыть файл для вывода: " << output_filename << "\n";
            return 1;
        }

        file_mode = false;
        database.resize(n);
        for (int i = 0; i < n; i++) {
            database[i] = i;
        }

    } else if (argc == 3) {
        // Режим из файла: input.txt output.txt
        std::string input_filename = argv[1];
        std::string output_filename = argv[2];

        outfile.open(output_filename);
        if (!outfile) {
            std::cerr << "Не удалось открыть файл для вывода: " << output_filename << "\n";
            return 1;
        }

        std::ifstream infile(input_filename);
        if (!infile) {
            std::cerr << "Не удалось открыть входной файл: " << input_filename << "\n";
            return 1;
        }

        file_mode = true;

        int n;
        infile >> n;
        infile >> num_readers;
        infile >> num_writers;

        database.resize(n);
        for (int i = 0; i < n; i++) {
            database[i] = i;
        }

        // Считываем операции для читателей
        reader_indices.resize(num_readers, std::vector<int>(reader_operations));
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

        // Считываем операции для писателей
        writer_ops.resize(num_writers, std::vector<std::pair<int,int>>(writer_operations));
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

    } else {
        std::cerr << "Использование:\n";
        std::cerr << "Случайный режим: " << argv[0] << " n R W output.txt\n";
        std::cerr << "Из файла: " << argv[0] << " input.txt output.txt\n";
        return 1;
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
        std::string line = "Все потоки завершили работу. Итоговый массив:";
        std::cout << line << std::endl;
        outfile << line << std::endl;
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
