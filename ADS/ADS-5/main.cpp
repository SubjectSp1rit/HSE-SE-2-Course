#include <iostream>
#include <cstdint>

// Класс узла дерева
struct TreeNode {
    int64_t value;              // Значение узла
    TreeNode* left = nullptr;   // Левый потомок
    TreeNode* right = nullptr;  // Правый потомок
    TreeNode* parent = nullptr; // Родительский узел

    TreeNode(const int64_t &val) : value(val) {}
};

// Класс бинарного дерева поиска
class BinarySearchTree {
public:
    // Итератор
    class Iterator {
    public:
        Iterator() : current(nullptr) {}
        Iterator(TreeNode* node) : current(node) {}

        // Операторы равенства
        bool operator==(const Iterator& other) const { return current == other.current; }
        bool operator!=(const Iterator& other) const { return current != other.current; }

        // Оператор разыменования
        const int64_t& operator*() const {
            if (!current) {
                throw std::runtime_error("Разыменовывать nullptr итератор нельзя");
            }
            return current->value;
        }

        // Префиксный инкремент
        Iterator& operator++() {
            if (!current) return *this; // Ничего не делаем для nullptr
            // Если есть правый потомок, идем в него и затем максимально влево
            if (current->right) {
                current = current->right;
                while (current->left) {
                    current = current->left;
                }
            } else {
                // Поднимаемся вверх по дереву
                TreeNode* parent = current->parent;
                while (parent && current == parent->right) {
                    current = parent;
                    parent = parent->parent;
                }
                current = parent;
            }
            return *this;
        }

        // Префиксный декремент
        Iterator& operator--() {
            if (!current) return *this; // Ничего не делаем для nullptr
            // Если есть левый потомок, идем в него и затем максимально вправо
            if (current->left) {
                current = current->left;
                while (current->right) {
                    current = current->right;
                }
            } else {
                // Поднимаемся вверх по дереву
                TreeNode* parent = current->parent;
                while (parent && current == parent->left) {
                    current = parent;
                    parent = parent->parent;
                }
                current = parent;
            }
            return *this;
        }

    private:
        TreeNode* current; // Текущий узел итератора

        friend class BinarySearchTree;
    };

    BinarySearchTree() : root(nullptr) {}
    ~BinarySearchTree() { clear(root); }

    void insert(const int64_t& value) {
        root = insert(root, nullptr, value);
    }

    Iterator begin() const {
        TreeNode* node = root;
        if (!node) return Iterator(nullptr);
        // Идем в самый левый узел
        while (node->left) {
            node = node->left;
        }
        return Iterator(node);
    }

    Iterator end() const {
        return Iterator(nullptr);
    }

    Iterator rbegin() const {
        TreeNode* node = root;
        if (!node) return Iterator(nullptr);
        // Идем в самый правый узел
        while (node->right) {
            node = node->right;
        }
        return Iterator(node);
    }

private:
    TreeNode* root; // Корень дерева

    TreeNode* insert(TreeNode* node, TreeNode* parent, const int64_t& value) {
        if (!node) {
            node = new TreeNode(value);
            node->parent = parent;
        } else if (value < node->value) {
            node->left = insert(node->left, node, value);
        } else if (value > node->value) {
            node->right = insert(node->right, node, value);
        }
        return node;
    }

    void clear(TreeNode* node) {
        if (node) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }
};

int main() {
    BinarySearchTree bst;

    bst.insert(50);
    bst.insert(30);
    bst.insert(70);
    bst.insert(20);
    bst.insert(40);
    bst.insert(60);
    bst.insert(80);

    // Обходим дерево с помощью цикла for по диапазону
    std::cout << "Обход дерева в порядке возрастания:" << std::endl;
    for (auto it = bst.begin(); it != bst.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    // Обходим дерево в обратном порядке
    std::cout << "Обход дерева в порядке убывания:" << std::endl;
    for (auto it = bst.rbegin(); it != bst.end(); --it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    return 0;
}