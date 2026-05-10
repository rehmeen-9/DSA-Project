# Course Project Management System
A C++ console app to manage students, groups, projects, and task progress — built with hand-crafted data structures.

## What it does
- Add/search/remove students
- Create groups and assign students
- Assign projects with deadlines
- Add tasks with prerequisites (must complete in order)
- Track progress and view group leaderboard

## Data Structures Used
| Structure | Used For |
|-----------|----------|
| HashMap | O(1) student & group lookup |
| AVL Tree | O(log n) task storage per project |
| DAG + Kahn's Sort | Task prerequisite enforcement |
| MinHeap | Projects sorted by earliest deadline |
| KMP Search | Fast partial name search |
| Merge Sort | Group leaderboard ranking |

## How to Run
```bash
g++ -std=c++17 Main.cpp -o main
./main
```

## Author
**Rehmeen Ali** — [@rehmeen-9](https://github.com/rehmeen-9)
