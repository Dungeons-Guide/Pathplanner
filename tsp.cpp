//
// Created by syeyoung on 5/8/25.
//

#include <vector>
#include <queue>
#include <map>
#include <set>
#include "tsp.h"


bool TSP::canVisit(Key key, int node) {
    if (mapping.nodeType[node] == 0) return false; // don't visit.
    if (!key.first.canBeAdded(mapping, node)) return false;
    if ((key.first.requireBitSet & mapping.require[node]) != mapping.require[node]) return false;

    bool flag = !mapping.orReq[node].empty();
    for (int nodeID : mapping.orReq[node]) {
        if (mapping.nodeType[nodeID] == 0 || key.first.contains(mapping, nodeID)) {
            flag = false;
            break;
        }
    }
    if (flag) return false;
    return true;
}

void TSP::solveTSP() {
    queue<Key> toVisit;
    set<Key> visited;
    for (int i = 0; i < mapping.N; ++i) {
        if (!canVisit({{0,0}, -1}, i)) continue;

        VisitedSet v{0,0}; v.add(mapping,i);
        toVisit.emplace(v, i);
        auto c = eval(start, 0, i);
        cost[{v,i}] = {
                c.second,
                {{0,0}, -1},
                c.first
        };
    }

    while (!toVisit.empty()) {
        Key top = toVisit.front();
        Value& val = cost[top];
        toVisit.pop();

        auto add = [&](int node) {
            VisitedSet copy = top.first;
            copy.add(mapping, node);

            auto c = eval(val.curr.pos, val.curr.mechanicSet, node);
            Key newKey = {copy, node};
            auto& target = cost[newKey];
            if (target.cost < 0) {
                target = {val.cost + c.second, top, c.first};
                toVisit.push(newKey);
            } else {
                if (target.cost > val.cost + c.second)
                    target = {val.cost + c.second, top, c.first};
            }
        };

        bool sanity = false;
        for (int i = 0; i < mapping.N; i++) {
            if (!mapping.sanity[i]) continue;
            if (!canVisit(top, i)) continue;
            sanity = true;
            add(i);
            break;
        }
        if (sanity) continue;


        // possible next visits.
        for (int i = 0; i < mapping.N; i++) {
            if (mapping.sanity[i]) continue;
            if (!canVisit(top, i)) continue;

            add(i);
        }
    }
}

vector<int> TSP::getSolution(int goalNode) {
    double minCost = INFINITY;
    Key key;
    for (const auto &item: cost) {
        if (!item.first.first.contains(mapping, goalNode)) continue;
        if (item.second.cost < minCost) {
            minCost = item.second.cost;
            key = item.first;
        }
    }
    if (minCost == INFINITY) return {};
    // backtrack!
    vector<int> sol;
    while (true) {
        Value elem = cost[key];
        sol.push_back(key.second);
        key = elem.prev;
        if (elem.prev.second == -1) break;
    }
    std::reverse(sol.begin(), sol.end());
    return sol;
}