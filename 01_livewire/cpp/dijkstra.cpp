#include <iostream>
#include <list>
#include <queue>
#include <limits>
#include <algorithm>

using namespace std;

template<typename T, typename U>
int dijkstra(Graph<T,U> &G, T source, T destination) {
    vector<T> distance(G.size(), numeric_limits<U>::max());
    vector<bool> visited(G.size(), false);

    priority_queue<
        pair<T, T>, 
        vector<pair<T, T>>,
        greater<pair<T, T>>> pq;

    distance[source] = 0;
    pq.push(make_pair(0, source));

    while (!pq.empty()) {
        auto u = pq.top().second;
        pq.pop();

        if (!visited[u]) {
            visited[u] = true;

            for (auto it = G.adjacencies(u).begin(); it != G.adjacencies(u).end(); it++) {
                T v = (*it).first;
                U cost = (*it).second;

                if (distance[v] > (distance[u] + cost)) {
                    distance[v] = distance[u] + cost;
                    pq.push(make_pair(distance[v], v));
                }
            }
        }
    }

    return distance[destination];
};