#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>
#include "MinHeap.h"
#include "HashMap.h"
#include "Models.h"
#include "GroupManager.h"
using namespace std;

// =====================================================
//  ProjectManager
//
//  projectHeap : MinHeap<ProjectEntry>   O(log n) deadline order
//  projectMap  : HashMap<id, Project*>   O(1) project lookup
//
//  Each Project contains:
//    AVLTree  — O(log n) task insert/search (self-balancing)
//    DAG      — O(V+E) prerequisite enforcement + topo sort
//
//  addProject      : O(log n) heap + O(1) map
//  addTask         : O(log n) AVL + O(1) DAG
//  addPrerequisite : O(1)
//  markTaskDone    : O(1) DAG prereq check + O(log n) AVL sync
//  getProgress     : O(V)
//  getTaskOrder    : O(V+E) Kahn's
// =====================================================
class ProjectManager {
    MinHeap<ProjectEntry>           projectHeap;
    unordered_map<string, Project*> projectMap;
    GroupManager&                   groups;

public:
    ProjectManager(GroupManager& gm) : groups(gm) {}

    ~ProjectManager() {
        for (auto& [id, p] : projectMap) delete p;
    }

    // ── add project ─────────────────────────
    bool addProject(const string& id, const string& name,
                    const string& gid, long long deadline) {
        if (!groups.groupExists(gid)) {
            cout << "  [ProjectManager] Group not found: " << gid << "\n";
            return false;
        }
        if (projectMap.count(id)) {
            cout << "  [ProjectManager] Project ID exists: " << id << "\n";
            return false;
        }
        Project* p = new Project(id, name, gid, deadline);
        projectMap[id] = p;
        projectHeap.insert(ProjectEntry(id, name, gid, deadline));
        cout << "  [ProjectManager] Added project: " << name << "\n";
        return true;
    }

    // ── add task ────────────────────────────
    // AVL insert O(log n) + DAG node O(1)
    bool addTask(const string& pid, const string& tid, const string& tname) {
        auto it = projectMap.find(pid);
        if (it == projectMap.end()) {
            cout << "  [ProjectManager] Project not found: " << pid << "\n";
            return false;
        }
        it->second->addTask(tid, tname);
        return true;
    }

    // ── add prerequisite ────────────────────
    // O(1) DAG edge
    bool addPrerequisite(const string& pid,
                         const string& before, const string& after) {
        auto it = projectMap.find(pid);
        if (it == projectMap.end()) return false;
        return it->second->addPrerequisite(before, after);
    }

    // ── mark task done ──────────────────────
    // DAG checks prereqs first — won't allow out-of-order completion
    bool markTaskDone(const string& pid, const string& tid) {
        auto it = projectMap.find(pid);
        if (it == projectMap.end()) return false;
        return it->second->markDone(tid);
    }

    // ── view progress ───────────────────────
    void viewProgress(const string& pid) {
        auto it = projectMap.find(pid);
        if (it == projectMap.end()) {
            cout << "  Project not found\n"; return;
        }
        it->second->printProgress();
    }

    // ── peek earliest deadline ──────────────
    // O(1)
    void peekNextDeadline() {
        if (projectHeap.isEmpty()) { cout << "  No projects.\n"; return; }
        cout << "  Next deadline -> ";
        projectHeap.peekMin().print();
    }

    // ── list all projects ───────────────────
    void listAll() {
        cout << "\n=== All Projects ===\n";
        for (auto& [id, p] : projectMap) p->print();
        cout << "====================\n";
    }

    // ── get task execution order ─────────────
    // Kahn's topo sort O(V+E)
    void getTaskOrder(const string& pid) {
        auto it = projectMap.find(pid);
        if (it == projectMap.end()) return;
        cout << "\n=== Task order for: " << it->second->name << " ===\n";
        it->second->taskDag.printOrder();
        cout << "=========================================\n";
    }

    // For analytics
    void getAllProjects(vector<Project*>& out) {
        for (auto& [id, p] : projectMap) out.push_back(p);
    }
};