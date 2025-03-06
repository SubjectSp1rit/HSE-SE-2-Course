#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
using namespace std;

struct Edge {
    int to; // конечная вершина ребра
    int capacity; // пропускная способность ребра
    int flow; // текущий поток через ребро
    int rev; // индекс ребра в списке смежности вершины to
};

class Network {
private:
    int n; // количество вершин в сети
    vector<vector<Edge>> adj; // список смежности для каждой вершины
    vector<int> level; // уровень вершины в графе (обход в ширину)
    vector<int> start; // указатель для итераций обхода в глубину

    bool bfs(int s, int t) {
        fill(level.begin(), level.end(), -1);
        level[s] = 0;
        queue<int> q;
        q.push(s);
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (const Edge &e : adj[u]) {
                if (level[e.to] < 0 && e.flow < e.capacity) {
                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                }
            }
        }
        return level[t] != -1;
    }

    int dfs(int u, int t, int flow) {
        if (u == t)
            return flow;
        for (; start[u] < adj[u].size(); start[u]++) {
            Edge &e = adj[u][start[u]];
            if (level[e.to] == level[u] + 1 && e.flow < e.capacity) {
                int curr_flow = min(flow, e.capacity - e.flow);
                int temp_flow = dfs(e.to, t, curr_flow);
                if (temp_flow > 0) {
                    e.flow += temp_flow;
                    adj[e.to][e.rev].flow -= temp_flow;
                    return temp_flow;
                }
            }
        }
        return 0;
    }

public:
    Network(int n) : n(n) {
        adj.resize(n);
        level.resize(n);
        start.resize(n);
    }

    void addEdge(int u, int v, int capacity) {
        Edge a = { v, capacity, 0, (int)adj[v].size() };
        Edge b = { u, 0, 0, (int)adj[u].size() };
        adj[u].push_back(a);
        adj[v].push_back(b);
    }

    int maxFlow(int s, int t) {
        if (s == t) return -1;
        int total = 0;
        while (bfs(s, t)) {
            fill(start.begin(), start.end(), 0);
            while (int flow = dfs(s, t, INT_MAX))
                total += flow;
        }
        return total;
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n = 4;
    Network net(n);
    net.addEdge(0, 1, 3);
    net.addEdge(0, 2, 2);
    net.addEdge(1, 2, 1);
    net.addEdge(1, 3, 2);
    net.addEdge(2, 3, 4);

    int source = 0, sink = 3;
    cout << "Максимальный поток: " << net.maxFlow(source, sink) << "\n";

    return 0;
}
