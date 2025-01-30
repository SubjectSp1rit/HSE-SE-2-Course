#include <iostream>
#include <vector>
#include <stdexcept>

class HashLPTable {
private:
    enum State { EMPTY, OCCUPIED, DELETED };
    struct Entry {
        int key;
        State state;
        Entry() : key(0), state(EMPTY) {}
    };

    std::vector<Entry> table;
    size_t capacity;
    size_t num_elements;
    double max_load_factor;

    size_t hash(int key) const {
        return key % capacity;
    }

    void rehash() {
        size_t new_capacity = capacity * 2 + 1;
        std::vector<Entry> new_table(new_capacity);
        size_t old_capacity = capacity;
        capacity = new_capacity;

        for (size_t i = 0; i < old_capacity; ++i) {
            if (table[i].state == OCCUPIED) {
                int key = table[i].key;
                size_t index = hash(key);
                size_t j = 0;
                while (j < new_capacity) {
                    size_t current_index = (index + j) % new_capacity;
                    Entry& entry = new_table[current_index];
                    if (entry.state != OCCUPIED) {
                        entry.key = key;
                        entry.state = OCCUPIED;
                        break;
                    }
                    j++;
                }
            }
        }

        table = std::move(new_table);
    }

public:
    explicit HashLPTable(size_t initial_capacity = 11, double max_load_factor = 0.7)
        : capacity(initial_capacity), num_elements(0), max_load_factor(max_load_factor) {
        table.resize(capacity);
    }

    bool insert(int key) {
        if (num_elements >= max_load_factor * capacity) {
            rehash();
        }

        size_t index = hash(key);
        size_t original_index = index;
        bool found_deleted = false;
        size_t deleted_index = 0;

        for (size_t i = 0; i < capacity; ++i) {
            size_t current_index = (index + i) % capacity;
            Entry& entry = table[current_index];
            if (entry.state == OCCUPIED) {
                if (entry.key == key) {
                    return false;
                }
            } else {
                if (entry.state == DELETED) {
                    if (!found_deleted) {
                        found_deleted = true;
                        deleted_index = current_index;
                    }
                } else {
                    if (found_deleted) {
                        current_index = deleted_index;
                    }
                    table[current_index].key = key;
                    table[current_index].state = OCCUPIED;
                    num_elements++;
                    return true;
                }
            }
        }

        rehash();
        return insert(key);
    }

    bool find(int key) const {
        size_t index = hash(key);
        for (size_t i = 0; i < capacity; ++i) {
            size_t current_index = (index + i) % capacity;
            const Entry& entry = table[current_index];
            if (entry.state == OCCUPIED && entry.key == key) {
                return true;
            } else if (entry.state == EMPTY) {
                break;
            }
        }
        return false;
    }

    bool remove(int key) {
        size_t index = hash(key);
        for (size_t i = 0; i < capacity; ++i) {
            size_t current_index = (index + i) % capacity;
            Entry& entry = table[current_index];
            if (entry.state == OCCUPIED && entry.key == key) {
                entry.state = DELETED;
                num_elements--;
                return true;
            } else if (entry.state == EMPTY) {
                break;
            }
        }
        return false;
    }

    size_t getCapacity() const { return capacity; }
};

int main() {
    // Тест 1: Вставка и поиск
    HashLPTable table1;
    table1.insert(5);
    table1.insert(15);
    std::cout << "Тест 1: " << (table1.find(5) && table1.find(15) && !table1.find(10) ? "Пройден" : "Провален") << std::endl;

    // Тест 2: Удаление
    table1.remove(5);
    std::cout << "Тест 2: " << (!table1.find(5) && table1.find(15) ? "Пройден" : "Провален") << std::endl;

    // Тест 3: Коллизии
    HashLPTable table2(11);
    table2.insert(5);
    table2.insert(16);
    std::cout << "Тест 3: " << (table2.find(5) && table2.find(16) ? "Пройден" : "Провален") << std::endl;

    // Тест 4: Рехеширование
    HashLPTable table3(11);
    for (int i = 0; i < 8; ++i) {
        table3.insert(i);
    }
    bool allFound = true;
    for (int i = 0; i < 8; ++i) {
        if (!table3.find(i)) {
            allFound = false;
            break;
        }
    }
    std::cout << "Тест 4: " << (allFound ? "Пройден" : "Провален") << std::endl;

    // Тест 5: Удаление несуществующего ключа
    std::cout << "Тест 5: " << (!table3.remove(100) ? "Пройден" : "Провален") << std::endl;

    // Тест 6: Вставка после удаления
    HashLPTable table4(11);
    table4.insert(5);
    table4.insert(16);
    table4.remove(5);
    table4.insert(27);
    std::cout << "Тест 6: " << (table4.find(27) && !table4.find(5) ? "Пройден" : "Провален") << std::endl;

    // Тест 7: Удаление всех элементов
    HashLPTable table5(11);
    for (int i = 0; i < 11; ++i) {
        table5.insert(i);
    }
    for (int i = 0; i < 11; ++i) {
        table5.remove(i);
    }
    bool empty = true;
    for (int i = 0; i < 11; ++i) {
        if (table5.find(i)) {
            empty = false;
            break;
        }
    }
    std::cout << "Тест 7: " << (empty ? "Пройден" : "Провален") << std::endl;

    // Тест 8: Повторная вставка после удаления
    HashLPTable table6(11);
    table6.insert(5);
    table6.remove(5);
    table6.insert(5);
    std::cout << "Тест 8: " << (table6.find(5) ? "Пройден" : "Провален") << std::endl;

    return 0;
}