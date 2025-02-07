#include <iostream>
#include <vector>
#include <random>
#include <limits>

class SkipList {
public:
    explicit SkipList(int maxLevel = 16, float probability = 0.5f)
        : MAX_LEVEL(maxLevel)
        , probability(probability)
        , level(1)
        , distribution(0.0f, 1.0f)
        , generator(std::random_device{}())
    {
        header = new Node(std::numeric_limits<int>::min(), MAX_LEVEL);
    }

    ~SkipList() {
        Node* node = header;
        while (node != nullptr) {
            Node* next = node->forward[0];
            delete node;
            node = next;
        }
    }

    bool search(int key) const {
        Node* current = header;
        for (int i = level - 1; i >= 0; i--) {
            while (current->forward[i] != nullptr && current->forward[i]->value < key) {
                current = current->forward[i];
            }
        }
        current = current->forward[0];
        return (current != nullptr && current->value == key);
    }

    void insert(int key) {
        std::vector<Node*> update(MAX_LEVEL, nullptr);
        Node* current = header;

        for (int i = level - 1; i >= 0; i--) {
            while (current->forward[i] != nullptr && current->forward[i]->value < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (current == nullptr || current->value != key) {
            int rndLevel = randomLevel();
            if (rndLevel > level) {
                for (int i = level; i < rndLevel; i++) {
                    update[i] = header;
                }
                level = rndLevel;
            }
            Node* newNode = new Node(key, rndLevel);
            for (int i = 0; i < rndLevel; i++) {
                newNode->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = newNode;
            }
        }
    }

    void erase(int key) {
        std::vector<Node*> update(MAX_LEVEL, nullptr);
        Node* current = header;
        for (int i = level - 1; i >= 0; i--) {
            while (current->forward[i] != nullptr && current->forward[i]->value < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }
        current = current->forward[0];

        if (current != nullptr && current->value == key) {
            for (int i = 0; i < level; i++) {
                if (update[i]->forward[i] != current)
                    break;
                update[i]->forward[i] = current->forward[i];
            }
            delete current;

            while (level > 1 && header->forward[level - 1] == nullptr) {
                level--;
            }
        }
    }

    void print() const {
        std::cout << "SkipList:" << std::endl;
        for (int i = level - 1; i >= 0; i--) {
            Node* node = header->forward[i];
            std::cout << "Уровень " << i + 1 << ": ";
            while (node != nullptr) {
                std::cout << node->value << " ";
                node = node->forward[i];
            }
            std::cout << "\n";
        }
    }

private:
    struct Node {
        int value;
        std::vector<Node*> forward;

        Node(int val, int level)
            : value(val), forward(level, nullptr) {}
    };

    int randomLevel() {
        int lvl = 1;
        while (distribution(generator) < probability && lvl < MAX_LEVEL)
            ++lvl;
        return lvl;
    }

    Node* header;
    int level;
    const int MAX_LEVEL;
    const float probability;

    mutable std::default_random_engine generator;
    mutable std::uniform_real_distribution<float> distribution;
};

int main() {
    SkipList skipList;

    skipList.insert(3);
    skipList.insert(6);
    skipList.insert(7);
    skipList.insert(9);
    skipList.insert(12);
    skipList.insert(19);
    skipList.insert(17);
    skipList.insert(26);
    skipList.insert(21);
    skipList.insert(25);

    skipList.print();

    int key = 19;
    if (skipList.search(key))
        std::cout << "Элемент " << key << " найден." << std::endl;
    else
        std::cout << "Элемент " << key << " не найден." << std::endl;

    std::cout << "\nУдаляем элемент " << key << "...\n" << std::endl;
    skipList.erase(key);
    skipList.print();

    return 0;
}
