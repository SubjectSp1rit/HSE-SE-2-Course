#include <iostream>
#include <vector>

enum class EulerianStatus {
    None,
    EulerianPath,
    EulerianCycle
};

class Graph {
public:
    explicit Graph(int vertices) : V(vertices), adj(vertices) {}

    void addEdge(int u, int v) {
        if(u < 0 || u >= V || v < 0 || v >= V)
            return;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    std::vector<int> dfs(int start) const {
        std::vector<bool> visited(V, false);
        std::vector<int> order;
        dfsUtil(start, visited, order);
        return order;
    }

    EulerianStatus checkEulerian() const {
        int startVertex = -1;
        for (int i = 0; i < V; ++i) {
            if (!adj[i].empty()) {
                startVertex = i;
                break;
            }
        }
        if (startVertex == -1)
            return EulerianStatus::EulerianCycle;

        std::vector<bool> visited(V, false);
        dfsUtil(startVertex, visited);

        for (int i = 0; i < V; ++i)
            if (!adj[i].empty() && !visited[i])
                return EulerianStatus::None;

        int oddCount = 0;
        for (int i = 0; i < V; ++i)
            if (adj[i].size() % 2 != 0)
                ++oddCount;

        if (oddCount == 0)
            return EulerianStatus::EulerianCycle;
        if (oddCount == 2)
            return EulerianStatus::EulerianPath;
        return EulerianStatus::None;
    }

private:
    int V;
    std::vector<std::vector<int>> adj;

    void dfsUtil(int v, std::vector<bool>& visited, std::vector<int>& order) const {
        visited[v] = true;
        order.push_back(v);
        for (int u : adj[v])
            if (!visited[u])
                dfsUtil(u, visited, order);
    }

    void dfsUtil(int v, std::vector<bool>& visited) const {
        visited[v] = true;
        for (int u : adj[v])
            if (!visited[u])
                dfsUtil(u, visited);
    }
};

int main() {
    {
        std::cout << "Граф g1 (Эйлеров цикл - квадрат):\n";
        Graph g1(4);
        g1.addEdge(0, 1);
        g1.addEdge(1, 2);
        g1.addEdge(2, 3);
        g1.addEdge(3, 0);
        auto order = g1.dfs(0);
        std::cout << "DFS: ";
        for (int v : order)
            std::cout << v << " ";
        std::cout << "\n";
        auto status = g1.checkEulerian();
        if (status == EulerianStatus::EulerianCycle)
            std::cout << "Эйлеров цикл\n";
        else if (status == EulerianStatus::EulerianPath)
            std::cout << "Эйлеров путь\n";
        else
            std::cout << "Не эйлеров граф\n";
    }

    {
        std::cout << "\nГраф g2 (Эйлеров путь):\n";
        Graph g2(4);
        g2.addEdge(0, 1);
        g2.addEdge(1, 2);
        g2.addEdge(2, 3);
        auto order = g2.dfs(0);
        std::cout << "DFS: ";
        for (int v : order)
            std::cout << v << " ";
        std::cout << "\n";
        auto status = g2.checkEulerian();
        if (status == EulerianStatus::EulerianCycle)
            std::cout << "Эйлеров цикл\n";
        else if (status == EulerianStatus::EulerianPath)
            std::cout << "Эйлеров путь\n";
        else
            std::cout << "Не эйлеров граф\n";
    }

    {
        std::cout << "\nГраф g3 (Отсоединенный граф):\n";
        Graph g3(4);
        g3.addEdge(0, 1);
        g3.addEdge(2, 3);
        auto order = g3.dfs(0);
        std::cout << "DFS: ";
        for (int v : order)
            std::cout << v << " ";
        std::cout << "\n";
        auto status = g3.checkEulerian();
        if (status == EulerianStatus::EulerianCycle)
            std::cout << "Эйлеров цикл\n";
        else if (status == EulerianStatus::EulerianPath)
            std::cout << "Эйлеров путь\n";
        else
            std::cout << "Не эйлеров граф\n";
    }

    {
        std::cout << "\nГраф g4 (граф без ребер):\n";
        Graph g4(3);
        auto order = g4.dfs(0);
        std::cout << "DFS: ";
        for (int v : order)
            std::cout << v << " ";
        std::cout << "\n";
        auto status = g4.checkEulerian();
        if (status == EulerianStatus::EulerianCycle)
            std::cout << "Эйлеров цикл\n";
        else if (status == EulerianStatus::EulerianPath)
            std::cout << "Эйлеров путь\n";
        else
            std::cout << "Не эйлеров граф\n";
    }
    return 0;
}
