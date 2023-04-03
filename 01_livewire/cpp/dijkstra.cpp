#include <iostream>
#include <list>
#include <queue>
#include <limits>
#include <algorithm>
#include <map>

using namespace std;

template<typename T, typename U>
int dijkstra(Graph<T,U> &G, T source, T destination) {
    map<T, U> distance = {{source, numeric_limits<U>::infinity()}}; // distance(G.size(), numeric_limits<U>::max());
    map<T, bool> visited = {};

    priority_queue<pair<U, T>, 
        vector<pair<U, T>>,
        greater<pair<U, T>>> pq;

	distance.insert({source, 0});
    pq.push(make_pair(0, source));

    while (!pq.empty()) {
        auto u = pq.top().second;
        pq.pop();

        if (!visited.contains(u)) {
            visited.insert({u, true});

            for (auto it = G.adjacencies(u).begin(); it != G.adjacencies(u).end(); it++) {
                T v = (*it).first;
                U cost = (*it).second;

                if (!distance.contains(v))
                    distance.insert({v, numeric_limits<U>::max()});

                if (distance.at(v) > (distance.at(u) + cost)) {
                    distance.at(v) = distance.at(u) + cost;
                    pq.push(make_pair(distance.at(v), v));
                }
            }
        }
    }

    return distance[destination];
};