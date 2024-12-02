#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <cmath>

class BinaryTree {
public:
    BinaryTree() : root(NULL) {}

    void insert(int key) {
        insert(root, key);
    }

    bool isAVL() {
        bool is_balanced = true;
        checkAVL(root, is_balanced);
        return is_balanced;
    }

private:
    struct Node {
        int key;
        Node* left;
        Node* right;
        Node(int k) : key(k), left(NULL), right(NULL) {}
    };

    Node* root;

    void insert(Node*& node, int key) {
        if (node == NULL) {
            node = new Node(key);
            return;
        }
        if (key < node->key) {
            insert(node->left, key);
        } else if (key > node->key) {
            insert(node->right, key);
        }
    }

    int checkAVL(Node* node, bool& is_balanced) {
        if (node == NULL || !is_balanced) {
            return 0;
        }
        int h_left = checkAVL(node->left, is_balanced);
        int h_right = checkAVL(node->right, is_balanced);
        if (std::abs(h_left - h_right) > 1) {
            is_balanced = false;
        }
        return std::max(h_left, h_right) + 1;
    }
};

int main() {
    BinaryTree tree;
    int num;
    while (std::cin >> num && num != 0) {
        tree.insert(num);
    }

    if (tree.isAVL()) {
        std::cout << "YES" << "\n";
    } else {
        std::cout << "NO" << "\n";
    }
    return 0;
}