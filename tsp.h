//
// Created by syeyoung on 5/8/25.
//

#ifndef DPTSP_TSP_H
#define DPTSP_TSP_H

#include <vector>
#include <functional>
#include <map>
#include <coroutine>

using namespace std;
typedef long long ll;

struct NodeMapping {
    int N;
    vector<int> nodeType;
    vector<int> requireBitIndex;
    vector<int> orSubIndex;
    vector<bool> sanity;

    vector<ll> orMult;
    vector<int> orCount;

    vector<ll> require;
    vector<vector<int>> orReq;
};


struct VisitedSet {
    ll requireBitSet;
    ll orBitSet;

    void add(NodeMapping& map, int id) {
        if (map.nodeType[id] == 0) return;
        if (map.nodeType[id] == 1) {
            requireBitSet |= (1ll << map.requireBitIndex[id]);
            return;
        }
        int orId = map.nodeType[id] - 2;
        int targetIndex = map.orSubIndex[id];
        ll curr = (orBitSet / map.orMult[orId]) % map.orCount[orId];
        orBitSet += (targetIndex-curr) * map.orMult[orId];
    }

    void remove(NodeMapping& map, int id) {
        if (map.nodeType[id] == 0) return;
        if (map.nodeType[id] == 1) {
            requireBitSet ^= (1ll << map.requireBitIndex[id]);
            return;
        }
        int orId = map.nodeType[id] - 2;
        int targetIndex = 0;
        ll curr = (orBitSet / map.orMult[orId]) % map.orCount[orId];
        orBitSet += (targetIndex-curr) * map.orMult[orId];
    }

    bool contains(NodeMapping& map, int id) const {
        if (map.nodeType[id] == 0) return false;
        if (map.nodeType[id] == 1) return (requireBitSet & (1ll << map.requireBitIndex[id]));
        int orId = map.nodeType[id] - 2;
        ll curr = (orBitSet / map.orMult[orId]) % map.orCount[orId];
        return curr == map.orSubIndex[id];
    }

    bool canBeAdded(NodeMapping& map, int id) const{
        if (map.nodeType[id] == 0) return false;
        if (map.nodeType[id] == 1) return !(requireBitSet & (1ll << map.requireBitIndex[id]));
        int orId = map.nodeType[id] - 2;
        ll curr = (orBitSet / map.orMult[orId]) % map.orCount[orId];
        return curr == 0;
    }
    bool operator <(const VisitedSet& y) const {
        return std::tie(requireBitSet, orBitSet) < std::tie(y.requireBitSet, y.orBitSet);
    }
};

typedef pair<VisitedSet, int> Key;
typedef array<double, 3> Vec3;
struct RoomStatus {
    Vec3 pos;
    int mechanicSet;
};

struct Value {
    double cost = -1;
    Key prev;
    RoomStatus curr;
};

typedef std::function<pair<RoomStatus, double>(Vec3, int, int)> CostEvaluator;

struct Coroutine {
    struct promise_type {
        double x, y, z;
        int bitSet;
        int node;

        Coroutine get_return_object() {return {this}; }
        suspend_always initial_suspend() {return {}; }
    };
    promise_type* promise;
};

class TSP {
private:
    NodeMapping& mapping;
    map<Key, Value> cost;
    Vec3 start;
    int stBitSet;

    CostEvaluator eval;

    bool canVisit(Key key, int node);

public:
    TSP(NodeMapping &mapping, Vec3 start, CostEvaluator eval, int stBitset) : mapping(mapping) {
        this->start = start;
        this->eval = eval;
        this->stBitSet = stBitset;
    }

    void solveTSP();
    vector<int> getSolution(int goal);
};

#endif //DPTSP_TSP_H
