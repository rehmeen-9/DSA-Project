#pragma once
#include <iostream>
#include <vector>
#include "HashMap.h"
#include "Student.h"
using namespace std;

// =====================================================
//  StudentManager
//  - HashMap for O(1) ID-based lookup
//  - KMP for O(n+m) partial name search
// =====================================================
class StudentManager {
    HashMap<Student> table;

public:
    // O(1) avg
    bool addStudent(const string& id, const string& name, const string& email) {
        if (table.contains(id)) {
            cout << "  [StudentManager] ID already exists: " << id << "\n";
            return false;
        }
        table.insert(id, Student(id, name, email));
        cout << "  [StudentManager] Added: " << name << "\n";
        return true;
    }

    // O(1) avg — exact ID lookup
    Student* getStudent(const string& id) { return table.search(id); }

    // O(1) avg
    bool removeStudent(const string& id) {
        if (!table.remove(id)) {
            cout << "  [StudentManager] Not found: " << id << "\n";
            return false;
        }
        cout << "  [StudentManager] Removed: " << id << "\n";
        return true;
    }

    bool exists(const string& id) { return table.contains(id); }

    // ── KMP name search ─────────────────────
    // Search all students whose name contains the pattern.
    // O(n * (n+m)) where n=students, m=pattern length
    // vs O(n * n*m) for naive nested loop search
    vector<Student> searchByName(const string& pattern) const {
        vector<Student> results;
        table.forEach([&](const string&, const Student& s) {
            if (kmpSearch(s.name, pattern))
                results.push_back(s);
        });
        return results;
    }

    void listAll() const {
        cout << "\n=== All Students ===\n";
        table.forEach([](const string&, const Student& s){ s.print(); });
        cout << "===================\n";
    }

    void stats() const { table.printStats(); }

    // needed for analytics
    void getAllStudents(vector<Student>& out) const {
        table.forEach([&](const string&, const Student& s){ out.push_back(s); });
    }
};