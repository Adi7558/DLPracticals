#include <iostream>
#include <vector>
#include <queue>
#include <omp.h>

using namespace std;

struct Graph {
    int V;
    vector<vector<int>> adj;

    Graph(int V) {
        this->V = V;
        adj.resize(V);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u); // Undirected
    }

    // ==================== Parallel BFS ====================
    void BFS(int start) {
        vector<bool> visited(V, false);
        queue<int> q;

        visited[start] = true;
        q.push(start);

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            cout << u << " ";

            vector<int> neighbors_to_add;

#pragma omp parallel for
            for (int i = 0; i < adj[u].size(); i++) {
                int v = adj[u][i];
#pragma omp critical
                {
                    if (!visited[v]) {
                        visited[v] = true;
                        neighbors_to_add.push_back(v);
                    }
                }
            }

            for (int v : neighbors_to_add) {
                q.push(v);
            }
        }
        cout << endl;
    }

    // ==================== Parallel DFS ====================
    void DFS(int start) {
        vector<bool> visited(V, false);

#pragma omp parallel
        {
#pragma omp single
            {
                DFSUtil(start, visited);
            }
        }
        cout << endl;
    }

private:
    void DFSUtil(int u, vector<bool> &visited) {
        bool alreadyVisited = false;

#pragma omp critical
        {
            if (visited[u])
                alreadyVisited = true;
            else
                visited[u] = true;
        }

        if (alreadyVisited) return;

        cout << u << " ";

#pragma omp parallel for
        for (int i = 0; i < adj[u].size(); i++) {
            int v = adj[u][i];
            bool visitChild = false;

#pragma omp critical
            {
                if (!visited[v])
                    visitChild = true;
            }

            if (visitChild) {
#pragma omp task
                DFSUtil(v, visited);
            }
        }

#pragma omp taskwait
    }
};

// ==================== MAIN FUNCTION ====================
int main() {
    int V;
    cout << "Enter number of vertices: ";
    cin >> V;

    Graph g(V);

    int edgeCount;
    cout << "Enter number of edges: ";
    cin >> edgeCount;

    cout << "Enter edges (format: u v):\n";
    for (int i = 0; i < edgeCount; i++) {
        int u, v;
        cin >> u >> v;
        g.addEdge(u, v);
    }

    int choice;
    cout << "\nChoose traversal method:\n";
    cout << "1. Parallel BFS\n";
    cout << "2. Parallel DFS\n";
    cout << "Enter your choice: ";
    cin >> choice;

    if (choice == 1) {
        cout << "Parallel BFS traversal starting from node 0:\n";
        g.BFS(0);
    } else if (choice == 2) {
        cout << "Parallel DFS traversal starting from node 0:\n";
        g.DFS(0);
    } else {
        cout << "Invalid choice.\n";
    }

    return 0;
}
