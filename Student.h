#pragma once
#include <string>
#include <vector>
#include <iostream>
using namespace std;

// =====================================================
//  KMP String Search
//  Used for partial name-based student search.
//
//  buildFailure : O(m)  — preprocess pattern
//  kmpSearch    : O(n)  — search text
//  Total        : O(n + m) vs O(n*m) naive
// =====================================================
vector<int> buildFailure(const string& pattern) {
    int m = pattern.size();
    vector<int> fail(m, 0);
    int j = 0;
    for (int i = 1; i < m; i++) {
        while (j > 0 && pattern[i] != pattern[j]) j = fail[j-1];
        if (pattern[i] == pattern[j]) j++;
        fail[i] = j;
    }
    return fail;
}

// Returns true if pattern found in text (case-insensitive)
bool kmpSearch(string text, string pattern) {
    // lowercase both for case-insensitive match
    for (char& c : text)    c = tolower(c);
    for (char& c : pattern) c = tolower(c);

    int n = text.size(), m = pattern.size();
    if (m == 0) return true;
    vector<int> fail = buildFailure(pattern);
    int j = 0;
    for (int i = 0; i < n; i++) {
        while (j > 0 && text[i] != pattern[j]) j = fail[j-1];
        if (text[i] == pattern[j]) j++;
        if (j == m) return true;
    }
    return false;
}

// =====================================================
//  Student
// =====================================================
struct Student {
    string id;
    string name;
    string email;

    Student() {}
    Student(string id, string name, string email)
        : id(id), name(name), email(email) {}

    void print() const {
        cout << "  [" << id << "] " << name << " (" << email << ")\n";
    }
};