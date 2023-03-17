#include <iostream>
#include "Graph.hpp"
#include "dijkstra.cpp"

using namespace std;

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


    Graph<pair<int, int>, int> f{};

    f.add_edge({0,0}, {0,1}, 2);
    f.add_edge({0,1}, {0,2}, 3);
    f.add_edge({0,2}, {0,3}, 1);
    f.add_edge({0,0}, {0,2}, 5);

    //cout << f.to_string() << endl;

    //cout << "Dijkstra distance: " << dijkstra(f, {0,0}, {0,3}) << endl;

    return 0;
}