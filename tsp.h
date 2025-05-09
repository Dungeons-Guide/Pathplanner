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
    bool operator ==(const VisitedSet& y) const {
        return requireBitSet == y.requireBitSet && orBitSet == y.orBitSet;
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

struct AwaitJava {
    double result;
    Vec3 coord;
    int mechId;
    int nodeId;

    std::coroutine_handle<> handle;

    bool await_ready() const noexcept {return false;}
    void await_suspend(std::coroutine_handle<> h) {handle = h;}
    pair<RoomStatus, double> await_resume() {return {RoomStatus{coord, mechId}, result}; }
};

struct TSPRoutine {
    struct promise_type {
        TSPRoutine get_return_object() {
            return TSPRoutine{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        bool await_ready() const noexcept {return false;}
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() { }
        void unhandled_exception() {}
    };

    std::coroutine_handle<promise_type> handle;
};

class TSP {
private:
    NodeMapping mapping;
    map<Key, Value> cost;
    Vec3 start;
    int stBitSet;

    bool canVisit(Key key, int node);

    TSPRoutine solveTSP();
public:
    AwaitJava awaiter;
    TSPRoutine routine;

    TSP(NodeMapping mapping, Vec3 start, int stBitset) {
        this->mapping = mapping;
        this->start = start;
        this->stBitSet = stBitset;
        routine = solveTSP();
    }
    ~TSP() {
        routine.handle.destroy();
    }

    vector<int> getSolution(int goal);
};

#endif //DPTSP_TSP_H
