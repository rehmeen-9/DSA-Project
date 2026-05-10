#pragma once
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <iostream>
using namespace std;

// =====================================================
//  DAG  — Directed Acyclic Graph
//  Used for task prerequisites AND progress tracking.
//
//  Why DAG over plain tree?
//  - A tree can't express "Task C needs both A and B done"
//  - DAG models multiple prerequisites naturally
//  - Kahn's topo sort gives valid execution order
//  - Detects cycles (impossible dependency chains)
//
//  addNode     : O(1)
//  addEdge     : O(1)  (prerequisite: from -> to)
//  topoSort    : O(V+E) Kahn's BFS algorithm
//  hasCycle    : O(V+E)
//  getProgress : O(V)
// =====================================================
class DAG {
    unordered_map<string, vector<string>> adj;      // node -> dependents
    unordered_map<string, vector<string>> prereqs;  // node -> prerequisites
    unordered_map<string, int>            indegree;
    unordered_map<string, bool>           completed;
    unordered_map<string, string>         names;    // id -> display name

public:
    // ── addNode ─────────────────────────────
    void addNode(const string& id, const string& name) {
        if (adj.find(id) == adj.end()) {
            adj[id]      = {};
            prereqs[id]  = {};
            indegree[id] = 0;
            completed[id]= false;
            names[id]    = name;
        }
    }

    // ── addEdge ─────────────────────────────
    // "before" must complete before "after"
    // O(1)
    bool addPrerequisite(const string& before, const string& after) {
        if (adj.find(before) == adj.end() || adj.find(after) == adj.end()) {
            cout << "  [DAG] Node not found\n";
            return false;
        }
        adj[before].push_back(after);
        prereqs[after].push_back(before);
        indegree[after]++;
        return true;
    }

    // ── markDone ────────────────────────────
    // O(1)
    bool markDone(const string& id) {
        if (adj.find(id) == adj.end()) return false;
        // check all prerequisites are done first
        for (auto& pre : prereqs[id]) {
            if (!completed[pre]) {
                cout << "  [DAG] Cannot complete '" << names[id]
                     << "' — prerequisite '" << names[pre] << "' not done yet!\n";
                return false;
            }
        }
        completed[id] = true;
        cout << "  [DAG] Task completed: " << names[id] << "\n";
        return true;
    }

    // ── getProgress ─────────────────────────
    // O(V)
    double getProgress() const {
        if (adj.empty()) return 0.0;
        int total = adj.size(), done = 0;
        for (auto& [id, c] : completed) if (c) done++;
        return (double)done / total;
    }

    // ── topoSort (Kahn's) ───────────────────
    // Returns valid execution order.
    // If result.size() < V -> cycle detected.
    // O(V+E)
    vector<string> topoSort() const {
        unordered_map<string, int> deg = indegree;
        queue<string> q;
        for (auto& [node, d] : deg)
            if (d == 0) q.push(node);

        vector<string> order;
        while (!q.empty()) {
            string cur = q.front(); q.pop();
            order.push_back(cur);
            for (const string& nb : adj.at(cur)) {
                if (--deg[nb] == 0) q.push(nb);
            }
        }
        return order;
    }

    bool hasCycle() const {
        return topoSort().size() != adj.size();
    }

    // ── print execution order ────────────────
    void printOrder() const {
        vector<string> order = topoSort();
        if (order.size() != adj.size()) {
            cout << "  [DAG] Cycle detected! Check dependencies.\n";
            return;
        }
        cout << "  Execution order:\n  ";
        for (int i = 0; i < (int)order.size(); i++) {
            cout << names.at(order[i]);
            if (i+1 < (int)order.size()) cout << " -> ";
        }
        cout << "\n";
    }

    // ── print all tasks with status ──────────
    void printTasks() const {
        cout << "  Tasks:\n";
        vector<string> order = topoSort();
        for (auto& id : order) {
            cout << "    " << (completed.at(id) ? "[x] " : "[ ] ")
                 << names.at(id) << " (" << id << ")";
            if (!prereqs.at(id).empty()) {
                cout << "  needs: ";
                for (auto& p : prereqs.at(id)) cout << names.at(p) << " ";
            }
            cout << "\n";
        }
    }

    bool isEmpty() const { return adj.empty(); }
    int  nodeCount() const { return adj.size(); }
};