#include <iostream>
#include <vector>
#include <time.h>
using namespace std;

struct Edge {
    int start;
    int end;
    int value;
    Edge(int s, int e, int v) : start(s), end(e), value(v) {}
};

bool equal(const Edge e1, const Edge e2)
{
    return (e1.start == e2.start) && (e1.end == e2.end);
}

struct TestCase {
    int node_num;
    vector<Edge> edges;
    vector<int> path;
    int min_dist;
    TestCase() {
        node_num = rand() % 21 + 5;
        int edge_num = rand() % 300 + 10;

        for (int i = 0; i < edge_num; ++i) {
            int start = rand() % node_num;
            int end = rand() % node_num;
            while(start == end) {
                end = rand() % node_num;
            }
            int value = rand() % 100;

            Edge e = Edge(start, end, value);

            edges.push_back(e);
        }
    }
};

struct Answer {
    vector<int> path;
    int min_dist;
    Answer(vector<int> &p, int d=-100):path(p), min_dist(d) {}
};

Answer dfs(const vector<Edge> &edges, int cur_node, int node_num, Answer& ans, vector<bool> visited, int cur_dist, vector<int> path) {
    if (cur_node == node_num - 1) {
        if (cur_dist < ans.min_dist) {
            ans.min_dist = cur_dist;
            ans.path = path;
        }
        return ans;
    }

    for (int i = 0; i < edges.size(); ++i) {
        int end = edges[i].end;
        if (visited[end]) {
            continue;
        }

        path.push_back(end);
        visited[cur_node] = true;
        dfs(edges, end, node_num, ans, visited, cur_dist + edges[i].value, path);
        visited[cur_node] = false;
        path.pop_back();
    }

    return ans;
}

Answer dfs(TestCase tc)
{
    vector<int> temp;
    Answer ans = Answer(temp);

    int node_num = tc.node_num;
    vector<Edge> edges = tc.edges;

    vector<bool> visited;
    for (int i = 0; i < node_num; ++i)
    {
        visited.push_back(false);
    }
    visited[0] = true;

    vector<int> path;
    path.push_back(0);
    dfs(edges, 0, node_num, ans, visited, 0, path);
    return ans;
}

int main() {
    srand(time(NULL));

    int testcase = 100;

    for (int i = 0; i < testcase; ++i) {
        TestCase t = TestCase();
        Answer ans = dfs(t);
        if (ans.min_dist > 0) {
            cout << "Aloja";
        }
    }
}