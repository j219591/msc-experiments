#include <iostream>
#include "Graph.hpp"
#include "dijkstra.cpp"

using namespace std;

struct Node {
    int x, y;
    Node(int _x, int _y): x(_x), y(_y) {}
};

bool operator<(const Node &lhs, const Node &rhs) {
    return (lhs.x < rhs.x || lhs.y < rhs.y);
}

bool operator>(const Node &lhs, const Node &rhs) {
    return (lhs.x > rhs.x || lhs.y > rhs.y);
}

int main() {
    Graph<int, int> g{};

    g.add_edge(0, 1, 4);
	g.add_edge(0, 2, 2);
	g.add_edge(0, 3, 5);
	g.add_edge(1, 4, 1);
	g.add_edge(2, 1, 1);
	g.add_edge(2, 3, 2);
	g.add_edge(2, 4, 1);
	g.add_edge(3, 4, 1);

    cout << "Dijkstra distance: " << dijkstra(g, 0, 4) << endl;

    Graph<Node, int> h{};

    h.add_edge(Node{0,0}, Node{0,1}, 2);
    h.add_edge(Node{0,1}, Node{0,2}, 3);
    h.add_edge(Node{0,2}, Node{0,3}, 1);
    h.add_edge(Node{0,0}, Node{0,2}, 5);

    cout << "Dijkstra distance Node: " << dijkstra(h, Node{0,0}, Node{0,3}) << endl;

    Graph<pair<int, int>, int> f{};

    f.add_edge({0,0}, {0,1}, 2);
    f.add_edge({0,1}, {0,2}, 3);
    f.add_edge({0,2}, {0,3}, 1);
    f.add_edge({0,0}, {0,2}, 5);

    //cout << f.to_string() << endl;

    cout << "Dijkstra distance: " << dijkstra(f, {0,0}, {0,3}) << endl;

    return 0;
}