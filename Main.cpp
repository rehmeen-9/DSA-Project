#include <iostream>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <string>
#include "StudentManager.h"
#include "GroupManager.h"
#include "ProjectManager.h"
#include "Analytics.h"
using namespace std;
using namespace chrono;

// =====================================================
//  Memory tracking — overrides global new/delete
// =====================================================
static size_t s_allocatedBytes = 0;
static size_t s_peakBytes      = 0;

void* operator new(size_t size) {
    s_allocatedBytes += size;
    if (s_allocatedBytes > s_peakBytes) s_peakBytes = s_allocatedBytes;
    return malloc(size);
}
void operator delete(void* ptr, size_t size) noexcept {
    if (s_allocatedBytes >= size) s_allocatedBytes -= size;
    free(ptr);
}
void operator delete(void* ptr) noexcept { free(ptr); }

// =====================================================
//  Profiler
// =====================================================
struct Profiler {
    template<typename Fn>
    static void measure(const string& opName,
                        const string& timeComplexity,
                        const string& spaceComplexity,
                        Fn fn) {
        cout << "\n  >> " << opName << "\n";
        cout << "     Theory  : Time=" << timeComplexity
             << "  Space=" << spaceComplexity << "\n";
        size_t memBefore = s_allocatedBytes;
        auto   start     = high_resolution_clock::now();
        fn();
        auto   end       = high_resolution_clock::now();
        size_t memAfter  = s_allocatedBytes;
        double us        = duration_cast<nanoseconds>(end - start).count() / 1000.0;
        long long delta  = (long long)memAfter - (long long)memBefore;
        cout << "     Actual  : Time=" << fixed << setprecision(3) << us << " us";
        if      (delta > 0) cout << "  Mem=+" << delta  << " bytes (heap alloc)\n";
        else if (delta < 0) cout << "  Mem="  << delta  << " bytes (heap freed)\n";
        else                cout << "  Mem=0 bytes (no heap change)\n";
        cout << "     " << string(54, '-') << "\n";
    }
};

// =====================================================
//  Data generators
// =====================================================
string pad(int n, int width=4) {
    ostringstream ss; ss << setw(width) << setfill('0') << n; return ss.str();
}

// 1000 first names and last names to mix
static const vector<string> FIRST = {
    "Ali","Sara","Zain","Hira","Omar","Nida","Bilal","Sana","Hassan","Ayesha",
    "Usman","Fatima","Tariq","Maria","Asad","Amna","Kamran","Zara","Faisal","Rabia",
    "Ahmed","Layla","Imran","Saba","Saad","Noor","Waqas","Iram","Hamid","Maryam",
    "Adeel","Tooba","Junaid","Huma","Shoaib","Rida","Aamir","Sidra","Talha","Mahnoor"
};
static const vector<string> LAST = {
    "Khan","Ahmed","Ali","Hassan","Malik","Raza","Sheikh","Baig","Qureshi","Mirza",
    "Siddiqui","Chaudhry","Ansari","Butt","Awan","Niazi","Gill","Lodhi","Rizvi","Naqvi",
    "Farooqi","Hashmi","Abbasi","Bokhari","Kazmi","Jafri","Zaidi","Shirazi","Hussain","Bhutto"
};

string genName(int i) {
    return FIRST[i % FIRST.size()] + " " + LAST[(i / FIRST.size()) % LAST.size()];
}
string genEmail(int i, const string& name) {
    string e = "";
    for (char c : name) { if (c == ' ') e += '.'; else if (isalpha(c)) e += tolower(c); }
    e += pad(i) + "@uni.edu";
    return e;
}

// =====================================================
//  Summary table
// =====================================================
void printSummaryTable() {
    cout << "\n\n========================================\n";
    cout << "  Complexity Summary Table\n";
    cout << "========================================\n";
    cout << left << setw(32) << "Operation"
                 << setw(18) << "Time"
                 << setw(16) << "Space" << "\n";
    cout << string(66, '=') << "\n";

    auto row = [](const string& op, const string& t, const string& s){
        cout << left << setw(32) << op << setw(18) << t << setw(16) << s << "\n";
    };
    auto hdr = [](const string& h){
        cout << "\n  [" << h << "]\n";
    };

    hdr("HashMap");
    row("  insert(id, student)",      "O(1) avg",       "O(n)");
    row("  search(id)",               "O(1) avg",       "O(1)");
    row("  remove(id)",               "O(1) avg",       "O(1)");
    row("  rehash (at 0.75 load)",    "O(n)",           "O(n)");

    hdr("KMP String Search");
    row("  buildFailure(pattern)",    "O(m)",           "O(m)");
    row("  kmpSearch(text, pat)",     "O(n + m)",       "O(m)");
    row("  searchByName(pattern)",    "O(n*(n+m))",     "O(k) results");

    hdr("Vector Group (swap-pop)");
    row("  addMember",                "O(1) amortised", "O(1)");
    row("  removeMember(swap-pop)",   "O(n) + O(1)",    "O(1)");
    row("  contains",                 "O(n)",           "O(1)");

    hdr("AVL Tree");
    row("  insert(task)",             "O(log n)",       "O(n) total");
    row("  search(id)",               "O(log n)",       "O(1)");
    row("  markDone(id)",             "O(log n)",       "O(1)");
    row("  getProgress()",            "O(n)",           "O(1)");
    row("  rotations (rebalance)",    "O(1) per node",  "O(1)");

    hdr("DAG + Kahn's Topo Sort");
    row("  addNode",                  "O(1)",           "O(V+E)");
    row("  addEdge / prereq",         "O(1)",           "O(1)");
    row("  markDone (prereq check)",  "O(deg(v))",      "O(1)");
    row("  topoSort (Kahn's BFS)",    "O(V + E)",       "O(V)");
    row("  getProgress",              "O(V)",           "O(1)");
    row("  hasCycle",                 "O(V + E)",       "O(V)");

    hdr("MinHeap");
    row("  insert",                   "O(log n)",       "O(n)");
    row("  extractMin",               "O(log n)",       "O(1)");
    row("  peekMin",                  "O(1)",           "O(1)");
    row("  heapifyUp / Down",         "O(log n)",       "O(1)");

    hdr("Merge Sort (Analytics)");
    row("  mergeSort (leaderboard)",  "O(n log n)",     "O(n)");
    row("  merge (combine step)",     "O(n)",           "O(n)");

    cout << "\n" << string(66, '=') << "\n";
    cout << "  Peak heap usage : " << s_peakBytes       << " bytes ("
         << s_peakBytes / 1024 << " KB)\n";
    cout << "  Live heap now   : " << s_allocatedBytes  << " bytes ("
         << s_allocatedBytes / 1024 << " KB)\n";
    cout << string(66, '=') << "\n\n";
}

// =====================================================
//  Main
// =====================================================
int main() {
    cout << "\n========================================\n";
    cout << "  Course Project Management System\n";
    cout << "  n=1000 students | with Profiling\n";
    cout << "========================================\n";

    StudentManager sm;
    GroupManager   gm(sm);
    ProjectManager pm(gm);

    // ── 1. Bulk insert 1000 students ────────
    cout << "\n-- 1. HashMap: Bulk Insert 1000 Students\n";

    Profiler::measure("addStudent x1000 (HashMap bulk insert)", "O(1) avg each", "O(n)",
    [&](){
        for (int i = 1; i <= 1000; i++) {
            string id    = "S" + pad(i);
            string name  = genName(i);
            string email = genEmail(i, name);
            sm.addStudent(id, name, email);
        }
    });

    sm.stats();

    // Single lookup in 1000-entry map
    Profiler::measure("getStudent(S0500) — 1 lookup in 1000 entries", "O(1) avg", "O(1)",
    [&](){
        Student* s = sm.getStudent("S0500");
        if (s) { cout << "     Found: "; s->print(); }
    });

    Profiler::measure("getStudent(S0999)", "O(1) avg", "O(1)",
    [&](){
        Student* s = sm.getStudent("S0999");
        if (s) { cout << "     Found: "; s->print(); }
    });

    // Duplicate test
    Profiler::measure("addStudent duplicate S0001 (rejected)", "O(1) avg", "O(1)",
    [&](){ sm.addStudent("S0001", "Duplicate", "dup@uni.edu"); });

    // Remove one student
    Profiler::measure("removeStudent(S0999)", "O(1) avg", "O(1)",
    [&](){ sm.removeStudent("S0999"); });

    // ── 2. KMP name search over 1000 students
    cout << "\n-- 2. KMP: Name Search (n=1000) ----------\n";

    Profiler::measure("searchByName('Ali') across 1000 students", "O(n*(n+m))", "O(m)",
    [&](){
        auto res = sm.searchByName("Ali");
        cout << "     Found " << res.size() << " matches\n";
        // print first 3
        for (int i = 0; i < min((int)res.size(), 3); i++) {
            cout << "     "; res[i].print();
        }
        if (res.size() > 3)
            cout << "     ... and " << res.size()-3 << " more\n";
    });

    Profiler::measure("searchByName('Khan') across 1000 students", "O(n*(n+m))", "O(m)",
    [&](){
        auto res = sm.searchByName("Khan");
        cout << "     Found " << res.size() << " matches\n";
    });

    Profiler::measure("searchByName('xyz') — no matches", "O(n*(n+m))", "O(m)",
    [&](){
        auto res = sm.searchByName("xyz");
        cout << "     Found " << res.size() << " matches\n";
    });

    // ── 3. Groups — 20 groups of ~50 students
    cout << "\n-- 3. Vector: 20 Groups of ~50 Students -\n";

    Profiler::measure("createGroup x20", "O(1) avg each", "O(1)",
    [&](){
        for (int g = 1; g <= 20; g++)
            gm.createGroup("G" + pad(g,2), "Team " + to_string(g));
    });

    Profiler::measure("addStudentToGroup x1000 (50 per group)", "O(1) amortised", "O(n)",
    [&](){
        for (int i = 1; i <= 999; i++) {   // S0999 was removed, skip
            string sid = "S" + pad(i);
            if (!sm.exists(sid)) continue;
            int gNum = ((i-1) / 50) % 20 + 1;
            gm.addStudentToGroup("G" + pad(gNum,2), sid);
        }
    });

    gm.listAll();

    // swap-and-pop removal demo
    Profiler::measure("removeStudentFromGroup S0050 (swap-and-pop)", "O(n) find + O(1) swap", "O(1)",
    [&](){ gm.removeStudentFromGroup("G01", "S0050"); });

    // ── 4. Projects — 20 projects (1 per group)
    cout << "\n-- 4. MinHeap: 20 Projects ---------------\n";

    Profiler::measure("addProject x20 (MinHeap insert each)", "O(log n) each", "O(n)",
    [&](){
        long long base = 20250601;
        for (int p = 1; p <= 20; p++) {
            string pid  = "P" + pad(p,2);
            string name = "Project " + to_string(p);
            string gid  = "G" + pad(p,2);
            pm.addProject(pid, name, gid, base + p);
        }
    });

    Profiler::measure("peekNextDeadline (MinHeap peekMin)", "O(1)", "O(1)",
    [&](){ pm.peekNextDeadline(); });

    pm.listAll();

    // ── 5. AVL tree — add 10 tasks per project
    cout << "\n-- 5. AVL Tree: 10 Tasks per Project ----\n";
    static const vector<string> TASK_NAMES = {
        "Requirements", "UI Design", "Database Schema", "Backend API",
        "Frontend Dev", "Integration", "Unit Testing", "QA Review",
        "Deployment", "Documentation"
    };

    Profiler::measure("addTask x200 (10 tasks x 20 projects, AVL insert)", "O(log n) each", "O(n) per tree",
    [&](){
        for (int p = 1; p <= 20; p++) {
            string pid = "P" + pad(p,2);
            for (int t = 1; t <= 10; t++) {
                string tid   = pid + "T" + pad(t,2);
                string tname = TASK_NAMES[t-1];
                pm.addTask(pid, tid, tname);
            }
        }
    });

    // ── 6. DAG prerequisites — chain for P01
    cout << "\n-- 6. DAG: Prerequisites for P01 --------\n";

    Profiler::measure("addPrerequisite x9 (chain: T1->T2->...->T10)", "O(1) each", "O(V+E)",
    [&](){
        for (int t = 1; t < 10; t++) {
            string before = "P01T" + pad(t,2);
            string after  = "P01T" + pad(t+1,2);
            pm.addPrerequisite("P01", before, after);
        }
    });

    Profiler::measure("getTaskOrder P01 (Kahn's topo sort, V=10 E=9)", "O(V+E)", "O(V)",
    [&](){ pm.getTaskOrder("P01"); });

    Profiler::measure("viewProgress P01 at 0%", "O(V)", "O(1)",
    [&](){ pm.viewProgress("P01"); });

    // ── 7. DAG enforcement — complete P01 tasks in order
    cout << "\n-- 7. DAG: Complete P01 (10 tasks in order)\n";

    // Try out-of-order first
    Profiler::measure("markDone P01T05 BEFORE prereqs (DAG blocks)", "O(deg)", "O(1)",
    [&](){ pm.markTaskDone("P01", "P01T05"); });

    // Complete in correct order
    Profiler::measure("markDone T01->T05 (5 tasks)", "O(deg) each", "O(1)",
    [&](){
        for (int t = 1; t <= 5; t++)
            pm.markTaskDone("P01", "P01T" + pad(t,2));
    });

    Profiler::measure("viewProgress P01 at 50%", "O(V)", "O(1)",
    [&](){ pm.viewProgress("P01"); });

    Profiler::measure("markDone T06->T10 (complete project)", "O(deg) each", "O(1)",
    [&](){
        for (int t = 6; t <= 10; t++)
            pm.markTaskDone("P01", "P01T" + pad(t,2));
    });

    Profiler::measure("viewProgress P01 at 100%", "O(V)", "O(1)",
    [&](){ pm.viewProgress("P01"); });

    // ── 8. Partially complete other projects for analytics
    cout << "\n-- 8. Populating progress for analytics --\n";
    Profiler::measure("Mark partial tasks for P02-P20", "O(deg) each", "O(1)",
    [&](){
        for (int p = 2; p <= 20; p++) {
            string pid = "P" + pad(p,2);
            // add a simple chain prereq
            for (int t = 1; t < 10; t++)
                pm.addPrerequisite(pid, pid+"T"+pad(t,2), pid+"T"+pad(t+1,2));
            // complete random number of tasks (p % 10 of them)
            int done = p % 11;
            for (int t = 1; t <= done; t++)
                pm.markTaskDone(pid, pid + "T" + pad(t,2));
        }
    });

    // ── 9. Merge sort analytics over 20 groups
    cout << "\n-- 9. Merge Sort: Analytics (n=20 groups)\n";
    Analytics analytics(gm, pm);

    Profiler::measure("showLeaderboard (Merge Sort n=20)", "O(n log n)", "O(n)",
    [&](){ analytics.showLeaderboard(); });

    // ── 10. MinHeap extract all 20
    cout << "\n-- 10. MinHeap: Extract All 20 Deadlines -\n";
    MinHeap<ProjectEntry> demo;
    for (int p = 1; p <= 20; p++) {
        string pid  = "P" + pad(p,2);
        string name = "Project " + to_string(p);
        string gid  = "G" + pad(p,2);
        demo.insert(ProjectEntry(pid, name, gid, 20250601 + p));
    }

    Profiler::measure("extractMin x20 (sorted by deadline)", "O(log n) each", "O(1)",
    [&](){
        while (!demo.isEmpty()) {
            ProjectEntry p = demo.extractMin();
            cout << "     " << p.deadline << "  " << p.name << "\n";
        }
    });

    // ── Summary table ────────────────────────
    printSummaryTable();

    cout << "========================================\n";
    cout << "  All operations complete. n=1000\n";
    cout << "========================================\n\n";
    return 0;
}