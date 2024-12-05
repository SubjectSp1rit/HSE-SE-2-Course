#include "avl_tree.h"
#include <algorithm>

Node::Node(int value) {
    this->value = value;
    this->left = nullptr;
    this->right = nullptr;
    this->height = 1;
}

Node::~Node() {
    delete left;
    delete right;
}

AVLTree::AVLTree() {
    root_ = nullptr;
    size_ = 0;
}

AVLTree::AVLTree(int value) {
    root_ = new Node(value);
    size_ = 1;
}

AVLTree::~AVLTree() {
    delete root_;
}

int AVLTree::getHeight() {
    return root_ ? root_->height : 0;
}

bool AVLTree::empty() {
    return size_ == 0;
}

int AVLTree::getSize() {
    return size_;
}

Node* AVLTree::getRoot() {
    return root_;
}

void AVLTree::insert(int value) {
    root_ = insertNode(root_, value);
}

void AVLTree::erase(int value) {
    root_ = removeNode(root_, value);
}

int* AVLTree::find(int value) {
    Node* node = findNode(root_, value);
    return node ? &(node->value) : nullptr;
}

int* AVLTree::lowerBound(int value) {
    Node* node = lowerBoundInternal(root_, value);
    return node ? &(node->value) : nullptr;
}

int* AVLTree::traversal() {
    if (size_ == 0) {
        return nullptr;
    }
    int* array = new int[size_];
    int index = 0;
    traversalInternal(root_, array, &index);
    return array;
}

int AVLTree::getNodeHeight(Node* node) {
    return node ? node->height : 0;
}

int AVLTree::balanceFactor(Node* node) {
    return getNodeHeight(node->right) - getNodeHeight(node->left);
}

void AVLTree::balanceHeight(Node* node) {
    int leftHeight = getNodeHeight(node->left);
    int rightHeight = getNodeHeight(node->right);
    node->height = std::max(leftHeight, rightHeight) + 1;
}

Node* AVLTree::rotateRight(Node* node) {
    Node* leftChild = node->left;
    node->left = leftChild->right;
    leftChild->right = node;
    balanceHeight(node);
    balanceHeight(leftChild);
    return leftChild;
}

Node* AVLTree::rotateLeft(Node* node) {
    Node* rightChild = node->right;
    node->right = rightChild->left;
    rightChild->left = node;
    balanceHeight(node);
    balanceHeight(rightChild);
    return rightChild;
}

Node* AVLTree::balanceNode(Node* node) {
    balanceHeight(node);
    int bf = balanceFactor(node);

    if (bf == 2) {
        if (balanceFactor(node->right) < 0) {
            node->right = rotateRight(node->right);
        }
        return rotateLeft(node);
    }

    if (bf == -2) {
        if (balanceFactor(node->left) > 0) {
            node->left = rotateLeft(node->left);
        }
        return rotateRight(node);
    }

    return node;
}

Node* AVLTree::insertNode(Node* node, int value) {
    if (!node) {
        size_++;
        return new Node(value);
    }

    if (value < node->value) {
        node->left = insertNode(node->left, value);
    } else if (value > node->value) {
        node->right = insertNode(node->right, value);
    } else {
        return node;
    }

    return balanceNode(node);
}

Node* AVLTree::findMinNode(Node* node) {
    return node->left ? findMinNode(node->left) : node;
}

Node* AVLTree::removeNode(Node* node, int value) {
    if (!node) {
        return nullptr;
    }

    if (value < node->value) {
        node->left = removeNode(node->left, value);
    } else if (value > node->value) {
        node->right = removeNode(node->right, value);
    } else {
        if (!node->right) {
            Node* left = node->left;
            node->left = nullptr;
            delete node;
            size_--;
            return left;
        } else if (!node->left) {
            Node* right = node->right;
            node->right = nullptr;
            delete node;
            size_--;
            return right;
        } else {
            Node* min = findMinNode(node->right);
            node->value = min->value;
            node->right = removeNode(node->right, min->value);
        }
    }

    return balanceNode(node);
}

Node* AVLTree::findNode(Node* node, int value) {
    if (!node) {
        return nullptr;
    }

    if (value < node->value) {
        return findNode(node->left, value);
    } else if (value > node->value) {
        return findNode(node->right, value);
    } else {
        return node;
    }
}

void AVLTree::traversalInternal(Node* node, int* array, int* index) {
    if (!node) {
        return;
    }
    traversalInternal(node->left, array, index);
    array[*index] = node->value;
    (*index)++;
    traversalInternal(node->right, array, index);
}

Node* AVLTree::lowerBoundInternal(Node* current, int value) const {
    if (!current) {
        return nullptr;
    }

    if (current->value >= value) {
        Node* leftResult = lowerBoundInternal(current->left, value);
        return leftResult ? leftResult : current;
    } else {
        return lowerBoundInternal(current->right, value);
    }
}