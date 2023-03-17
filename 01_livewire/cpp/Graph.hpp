#pragma once

#include <iostream>
#include <list>
#include <queue>
#include <map>

using namespace std;

template<typename T, typename U>
class Graph {

private:
    map<T, list<pair<T, U>>> adj{}; // list of adjacencies

public:
    T size() {
        return adj.size();
    }

    list<pair<T, U>>& adjacencies(T index) {
        return adj[index];
    }

    void add_edge(T v1, T v2, U cost) {
        if (!adj.count(v1)) {
            //cout << "add " << v1 << endl;
            list<pair<T,U>> l {make_pair(v2, cost)};
            adj.insert(make_pair(v1, l));
        } else {
            adj[v1].push_back(make_pair(v2, cost));
        }            
    }

    string to_string() const {
        for (auto &i : adj) {
            cout << i << endl;
        }
    }
};