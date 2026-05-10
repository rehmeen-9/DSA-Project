#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "AVLTree.h"
#include "DAG.h"
using namespace std;

// =====================================================
//  Group
//  Members stored in a vector<string> (student IDs).
//
//  Why vector over LinkedList?
//  - Cache locality: contiguous memory = faster iteration
//  - O(1) index access
//  - Removal uses swap-and-pop: O(1) instead of O(n) shift
//
//  addMember    : O(1) push_back
//  removeMember : O(n) find + O(1) swap-and-pop
//  contains     : O(n) scan
// =====================================================
struct Group {
    string          id;
    string          name;
    vector<string>  members;   // student IDs

    Group() {}
    Group(string id, string name) : id(id), name(name) {}

    // O(1) amortised
    void addMember(const string& sid) {
        if (!contains(sid)) members.push_back(sid);
    }

    // O(n) find + O(1) swap-and-pop
    // Order not preserved — acceptable since display sorts anyway
    bool removeMember(const string& sid) {
        for (int i = 0; i < (int)members.size(); i++) {
            if (members[i] == sid) {
                swap(members[i], members.back());   // O(1)
                members.pop_back();                 // O(1)
                return true;
            }
        }
        return false;
    }

    // O(n)
    bool contains(const string& sid) const {
        for (auto& m : members) if (m == sid) return true;
        return false;
    }

    void print() const {
        cout << "  Group [" << id << "] " << name
             << " (" << members.size() << " members)\n";
    }
};

// =====================================================
//  ProjectEntry  — stored in MinHeap, sorted by deadline
// =====================================================
struct ProjectEntry {
    string   id;
    string   name;
    string   groupId;
    long long deadline;

    ProjectEntry() : deadline(0) {}
    ProjectEntry(string id, string name, string gid, long long dl)
        : id(id), name(name), groupId(gid), deadline(dl) {}

    bool operator<(const ProjectEntry& o) const { return deadline < o.deadline; }

    void print() const {
        cout << "  Project [" << id << "] " << name
             << "  group=" << groupId
             << "  deadline=" << deadline << "\n";
    }
};

// =====================================================
//  Project  — full project with AVL task tree + DAG
// =====================================================
struct Project {
    string       id;
    string       name;
    string       groupId;
    long long    deadline;
    AVLTree      taskTree;   // O(log n) insert/search
    DAG          taskDag;    // prerequisite enforcement + progress

    Project() : deadline(0) {}
    Project(string id, string name, string gid, long long dl)
        : id(id), name(name), groupId(gid), deadline(dl) {}

    // AVL insert + DAG node — O(log n)
    void addTask(const string& tid, const string& tname) {
        taskTree.insert(tid, tname);
        taskDag.addNode(tid, tname);
    }

    // DAG prerequisite edge — O(1)
    bool addPrerequisite(const string& before, const string& after) {
        return taskDag.addPrerequisite(before, after);
    }

    // DAG enforces prereqs before marking done — O(1)
    bool markDone(const string& tid) {
        bool ok = taskDag.markDone(tid);
        if (ok) taskTree.markDone(tid);   // sync AVL node
        return ok;
    }

    // DAG-based progress — O(V)
    double getProgress() const { return taskDag.getProgress(); }

    void printProgress() const {
        cout << "\n=== Progress: " << name << " ===\n";
        cout << "  Completion: " << (int)(getProgress()*100) << "%\n";
        taskDag.printOrder();
        taskDag.printTasks();
        cout << "  AVL tree height: " << taskTree.getHeight()
             << " (balanced = O(log n) search)\n";
        cout << "===========================\n";
    }

    void print() const {
        cout << "  Project [" << id << "] " << name
             << "  group=" << groupId
             << "  deadline=" << deadline
             << "  progress=" << (int)(getProgress()*100) << "%\n";
    }
};