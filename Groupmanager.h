#pragma once
#include <iostream>
#include "HashMap.h"
#include "Models.h"
#include "StudentManager.h"
using namespace std;

// =====================================================
//  GroupManager
//  Groups stored in HashMap<string, Group>.
//  Each Group uses vector<string> for members
//  with swap-and-pop removal.
// =====================================================
class GroupManager {
    HashMap<Group>  groups;
    StudentManager& students;

public:
    GroupManager(StudentManager& sm) : students(sm) {}

    // O(1) avg
    bool createGroup(const string& id, const string& name) {
        if (groups.contains(id)) {
            cout << "  [GroupManager] Group already exists: " << id << "\n";
            return false;
        }
        groups.insert(id, Group(id, name));
        cout << "  [GroupManager] Created: " << name << "\n";
        return true;
    }

    // O(1) avg HashMap + O(n) duplicate check in vector
    bool addStudentToGroup(const string& gid, const string& sid) {
        Group* g = groups.search(gid);
        if (!g) { cout << "  [GroupManager] Group not found: " << gid << "\n"; return false; }
        if (!students.exists(sid)) { cout << "  [GroupManager] Student not found: " << sid << "\n"; return false; }
        g->addMember(sid);
        cout << "  [GroupManager] " << sid << " added to " << g->name << "\n";
        return true;
    }

    // O(n) find + O(1) swap-and-pop
    bool removeStudentFromGroup(const string& gid, const string& sid) {
        Group* g = groups.search(gid);
        if (!g) return false;
        bool removed = g->removeMember(sid);
        if (removed)
            cout << "  [GroupManager] " << sid << " removed from " << g->name << "\n";
        return removed;
    }

    Group*  getGroup(const string& id)  { return groups.search(id); }
    bool    groupExists(const string& id) { return groups.contains(id); }

    void listMembers(const string& gid) {
        Group* g = groups.search(gid);
        if (!g) { cout << "  Group not found\n"; return; }
        cout << "\n=== " << g->name << " members ===\n";
        for (auto& sid : g->members) {
            Student* s = students.getStudent(sid);
            if (s) s->print();
        }
        cout << "========================\n";
    }

    void listAll() const {
        cout << "\n=== All Groups ===\n";
        groups.forEach([](const string&, const Group& g){ g.print(); });
        cout << "==================\n";
    }

    // For analytics — collect all groups
    void getAllGroups(vector<Group>& out) const {
        groups.forEach([&](const string&, const Group& g){ out.push_back(g); });
    }
};