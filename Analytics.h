#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Models.h"
#include "GroupManager.h"
#include "ProjectManager.h"
using namespace std;

// =====================================================
//  Analytics Dashboard
//  Ranks groups by project completion using Merge Sort.
//
//  Why Merge Sort?
//  - O(n log n) guaranteed — no worst-case O(n^2) like quicksort
//  - Stable sort — groups with equal progress keep original order
//  - Divide and conquer is a core DSA concept
// =====================================================

struct GroupStat {
    string groupId;
    string groupName;
    double avgProgress;   // 0.0 - 1.0
    int    memberCount;
    int    projectCount;

    void print(int rank) const {
        cout << "  #" << rank << "  " << groupName
             << "  progress=" << (int)(avgProgress*100) << "%"
             << "  members=" << memberCount
             << "  projects=" << projectCount << "\n";
    }
};

// ── Merge Sort ──────────────────────────────────────
// Sorts descending by avgProgress (highest first).
// O(n log n) time, O(n) space.

void merge(vector<GroupStat>& arr, int l, int m, int r) {
    vector<GroupStat> left (arr.begin()+l, arr.begin()+m+1);
    vector<GroupStat> right(arr.begin()+m+1, arr.begin()+r+1);

    int i = 0, j = 0, k = l;
    while (i < (int)left.size() && j < (int)right.size()) {
        // descending — higher progress first
        if (left[i].avgProgress >= right[j].avgProgress)
            arr[k++] = left[i++];
        else
            arr[k++] = right[j++];
    }
    while (i < (int)left.size())  arr[k++] = left[i++];
    while (j < (int)right.size()) arr[k++] = right[j++];
}

void mergeSort(vector<GroupStat>& arr, int l, int r) {
    if (l >= r) return;
    int m = l + (r-l)/2;
    mergeSort(arr, l, m);
    mergeSort(arr, m+1, r);
    merge(arr, l, m, r);
}

// ── Analytics class ─────────────────────────────────
class Analytics {
    GroupManager&   gm;
    ProjectManager& pm;

public:
    Analytics(GroupManager& g, ProjectManager& p) : gm(g), pm(p) {}

    // Build stats for each group then merge sort them
    // O(G*P) to build + O(G log G) to sort
    void showLeaderboard() {
        cout << "\n========================================\n";
        cout << "   Group Progress Leaderboard\n";
        cout << "   (sorted by Merge Sort, O(n log n))\n";
        cout << "========================================\n";

        // collect all groups
        vector<Group> groups;
        gm.getAllGroups(groups);

        // collect all projects
        vector<Project*> projects;
        pm.getAllProjects(projects);

        // build stats per group
        vector<GroupStat> stats;
        for (auto& g : groups) {
            GroupStat gs;
            gs.groupId    = g.id;
            gs.groupName  = g.name;
            gs.memberCount= g.members.size();

            double totalProgress = 0.0;
            int count = 0;
            for (auto* p : projects) {
                if (p->groupId == g.id) {
                    totalProgress += p->getProgress();
                    count++;
                }
            }
            gs.avgProgress  = count > 0 ? totalProgress / count : 0.0;
            gs.projectCount = count;
            stats.push_back(gs);
        }

        if (stats.empty()) {
            cout << "  No groups found.\n";
            return;
        }

        // merge sort descending by progress
        mergeSort(stats, 0, stats.size()-1);

        for (int i = 0; i < (int)stats.size(); i++)
            stats[i].print(i+1);

        cout << "========================================\n";
        cout << "  Lagging groups (below 50%):\n";
        for (auto& s : stats)
            if (s.avgProgress < 0.5)
                cout << "  >> " << s.groupName
                     << " at " << (int)(s.avgProgress*100) << "%\n";
        cout << "========================================\n\n";
    }
};