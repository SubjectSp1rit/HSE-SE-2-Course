#include <iostream>
#include <random>

struct Node {
    int key;
    Node* left;
    Node* right;
    Node(int k) : key(k), left(nullptr), right(nullptr) {}
};

class RandTree {
public:
    Node* root;
    std::mt19937 gen;

    // Правый поворот
    Node* rotateRight(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        x->right = y;
        return x;
    }

    // Левый поворот
    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        y->left = x;
        return y;
    }

    // 1) Если дерево пусто — создаём новый узел
    // 2) Если key меньше, чем curr->key, идем влево
    // 3) Если key больше, чем curr->key, идем вправо
    // 4) После вставки случайно "поднимаем" вставленный узел вверх (вращаем) с некоторой вероятностью
    Node* insertNode(Node* curr, int key) {
        if (!curr) {
            return new Node(key);
        }
        if (key < curr->key) {
            curr->left = insertNode(curr->left, key);
            if (randomChance())
                curr = rotateRight(curr);
        } else if (key > curr->key) {
            curr->right = insertNode(curr->right, key);
            if (randomChance())
                curr = rotateLeft(curr);
        }
        return curr;
    }

    bool randomChance() {
        // Вероятность 1/4
        std::uniform_int_distribution<int> dist(0, 3);
        return dist(gen) == 0;
    }

    // Слияние
    Node* mergeTrees(Node* l, Node* r) {
        if (!l) return r;
        if (!r) return l;

        if (randomChance()) {
            l->right = mergeTrees(l->right, r);
            return l;
        } else {
            r->left = mergeTrees(l, r->left);
            return r;
        }
    }

    // Удаление
    Node* removeNode(Node* curr, int key) {
        if (!curr) return nullptr;

        if (key < curr->key) {
            curr->left = removeNode(curr->left, key);
        } else if (key > curr->key) {
            curr->right = removeNode(curr->right, key);
        } else {
            Node* tmp = mergeTrees(curr->left, curr->right);
            delete curr;
            return tmp;
        }
        return curr;
    }

    // Поиск по ключу
    bool findNode(Node* curr, int key) {
        if (!curr) return false;
        if (curr->key == key) return true;
        if (key < curr->key) return findNode(curr->left, key);
        else return findNode(curr->right, key);
    }

    // Вывод дерева
    void inorderPrint(Node* curr) {
        if (!curr) return;
        inorderPrint(curr->left);
        std::cout << curr->key << " ";
        inorderPrint(curr->right);
    }

    // Удаление дерева
    void clear(Node* curr) {
        if (!curr) return;
        clear(curr->left);
        clear(curr->right);
        delete curr;
    }

    // Число вершин
    int countNodes(Node* curr) {
        if (!curr) return 0;
        return 1 + countNodes(curr->left) + countNodes(curr->right);
    }

    // Высота дерева
    int calculateHeight(Node* curr) {
        if (!curr) return 0;
        return 1 + std::max(calculateHeight(curr->left), calculateHeight(curr->right));
    }


    RandTree() : root(nullptr) {
        std::random_device rd;
        gen = std::mt19937(rd());
    }

    ~RandTree() {
        clear(root);
    }

    void insert(int key) {
        root = insertNode(root, key);
    }

    void remove(int key) {
        root = removeNode(root, key);
    }

    bool find(int key) {
        return findNode(root, key);
    }

    void printInOrder() {
        inorderPrint(root);
        std::cout << "\n";
    }

    int size() {
        return countNodes(root);
    }

    int height() {
        return calculateHeight(root);
    }
};


int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    RandTree tree;

    // Тесты
    std::cout << "Вставка 10, 5, 20, 15, 25\n";
    tree.insert(10);
    tree.insert(5);
    tree.insert(20);
    tree.insert(15);
    tree.insert(25);
    tree.printInOrder();
    std::cout << "Размер дерева: " << tree.size() << "\n";

    std::cout << "Поиск 15: " << (tree.find(15) ? "найден" : "не найден") << "\n";
    std::cout << "Поиск 100: " << (tree.find(100) ? "найден" : "не найден") << "\n";

    std::cout << "Удаление 10\n";
    tree.remove(10);
    tree.printInOrder();

    std::cout << "Удаление 20\n";
    tree.remove(20);
    tree.printInOrder();

    std::cout << "Вставка 1, 7, 17\n";
    tree.insert(1);
    tree.insert(7);
    tree.insert(17);
    tree.printInOrder();
    std::cout << "Размер дерева: " << tree.size() << "\n";
    std::cout << "Высота дерева: " << tree.height() << "\n";

    return 0;
}