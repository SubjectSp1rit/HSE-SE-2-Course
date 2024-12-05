#include <iostream>
#include <vector>
#include <limits>

struct Node {
    long long key;
    int left;
    int right;
    char color;
};

bool check(int node_index, long long min_key, long long max_key, int& black_height, const std::vector<Node>& nodes) {
    if (node_index == 0) {
        black_height = 1;
        return true;
    }

    const Node& node = nodes[node_index];

    if (node.key <= min_key || node.key >= max_key) {
        return false;
    }

    if (node.color == 'R') {
        if ((node.left != 0 && nodes[node.left].color != 'B') || (node.right != 0 && nodes[node.right].color != 'B')) {
            return false;
        }
    }

    int left_black_height = 0, right_black_height = 0;

    if (!check(node.left, min_key, node.key, left_black_height, nodes)) {
        return false;
    }
    if (!check(node.right, node.key, max_key, right_black_height, nodes)) {
        return false;
    }

    if (left_black_height != right_black_height) {
        return false;
    }

    black_height = left_black_height + (node.color == 'B' ? 1 : 0);

    return true;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    int n;
    while (std::cin >> n) {
        if (n == 0) {
            std::cout << "NO\n";
            continue;
        }

        int root;
        std::cin >> root;

        std::vector<Node> nodes(n + 1);
        nodes[0] = {0, 0, 0, 'B'};

        for (int i = 0; i < n; ++i) {
            int number;
            std::string left_str, right_str;
            char color;
            std::cin >> number;

            long long key;
            std::cin >> key;

            std::cin >> left_str;
            int left = (left_str == "null") ? 0 : std::stoi(left_str);

            std::cin >> right_str;
            int right = (right_str == "null") ? 0 : std::stoi(right_str);

            std::cin >> color;

            nodes[number] = {key, left, right, color};
        }

        if (nodes[root].color != 'B') {
            std::cout << "NO\n";
            continue;
        }

        int black_height = 0;
        if (check(root, std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max(), black_height, nodes)) {
            std::cout << "YES\n";
        } else {
            std::cout << "NO\n";
        }
    }
    return 0;
}