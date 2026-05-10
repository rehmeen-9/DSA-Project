#pragma once
#include <string>
#include <iostream>
#include <algorithm>
using namespace std;

// =====================================================
//  AVL Tree  — self-balancing BST for task storage
//  Keyed on task ID (string).
//
//  insert   : O(log n) — rotates to rebalance
//  search   : O(log n) — guaranteed even if skewed
//  remove   : O(log n)
//  Space    : O(n)
//
//  Balance factor = height(left) - height(right)
//  Rebalance when |bf| > 1 using 4 rotation cases:
//    LL, RR, LR, RL
// =====================================================
struct TaskNode {
    string   id;
    string   name;
    bool     completed;
    int      height;
    TaskNode* left;
    TaskNode* right;

    TaskNode(string id, string name)
        : id(id), name(name), completed(false),
          height(1), left(nullptr), right(nullptr) {}
};

class AVLTree {
public:
    TaskNode* root;

    AVLTree() : root(nullptr) {}
    ~AVLTree() { destroy(root); }

    // ── insert ──────────────────────────────
    // O(log n)
    void insert(const string& id, const string& name) {
        root = insertNode(root, id, name);
    }

    // ── search ──────────────────────────────
    // O(log n)
    TaskNode* search(const string& id) {
        return searchNode(root, id);
    }

    // ── mark done ───────────────────────────
    // O(log n)
    bool markDone(const string& id) {
        TaskNode* node = searchNode(root, id);
        if (!node) return false;
        node->completed = true;
        return true;
    }

    // ── progress ────────────────────────────
    // O(n) inorder traversal — count completed tasks
    double getProgress() const {
        int total = 0, done = 0;
        countNodes(root, total, done);
        if (total == 0) return 0.0;
        return (double)done / total;
    }

    // ── print inorder ───────────────────────
    // O(n)
    void printInorder() const {
        cout << "  Tasks (inorder):\n";
        inorder(root);
    }

    int getHeight() const { return height(root); }

private:
    // ── height helper ───────────────────────
    int height(TaskNode* n) const { return n ? n->height : 0; }

    void updateHeight(TaskNode* n) {
        if (n) n->height = 1 + max(height(n->left), height(n->right));
    }

    // ── balance factor ──────────────────────
    int bf(TaskNode* n) const {
        return n ? height(n->left) - height(n->right) : 0;
    }

    // ── rotations ───────────────────────────
    //
    //  Right rotation (LL case):
    //       z                y
    //      / \             /   \
    //     y   T4   =>    x     z
    //    / \            / \   / \
    //   x  T3          T1 T2 T3 T4
    TaskNode* rotateRight(TaskNode* z) {
        TaskNode* y  = z->left;
        TaskNode* T3 = y->right;
        y->right = z;
        z->left  = T3;
        updateHeight(z);
        updateHeight(y);
        return y;
    }

    //  Left rotation (RR case):
    //     z                  y
    //    / \               /   \
    //   T1   y    =>      z     x
    //       / \          / \   / \
    //      T2  x        T1 T2 T3 T4
    TaskNode* rotateLeft(TaskNode* z) {
        TaskNode* y  = z->right;
        TaskNode* T2 = y->left;
        y->left  = z;
        z->right = T2;
        updateHeight(z);
        updateHeight(y);
        return y;
    }

    // ── insertNode ──────────────────────────
    TaskNode* insertNode(TaskNode* node, const string& id, const string& name) {
        // standard BST insert
        if (!node) return new TaskNode(id, name);
        if (id < node->id)      node->left  = insertNode(node->left,  id, name);
        else if (id > node->id) node->right = insertNode(node->right, id, name);
        else return node; // duplicate

        updateHeight(node);

        int balance = bf(node);

        // LL case
        if (balance > 1 && id < node->left->id)
            return rotateRight(node);

        // RR case
        if (balance < -1 && id > node->right->id)
            return rotateLeft(node);

        // LR case
        if (balance > 1 && id > node->left->id) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        // RL case
        if (balance < -1 && id < node->right->id) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    // ── searchNode ──────────────────────────
    TaskNode* searchNode(TaskNode* node, const string& id) const {
        if (!node)          return nullptr;
        if (id == node->id) return node;
        if (id < node->id)  return searchNode(node->left,  id);
        return                     searchNode(node->right, id);
    }

    // ── count ───────────────────────────────
    void countNodes(TaskNode* node, int& total, int& done) const {
        if (!node) return;
        total++;
        if (node->completed) done++;
        countNodes(node->left,  total, done);
        countNodes(node->right, total, done);
    }

    // ── inorder ─────────────────────────────
    void inorder(TaskNode* node) const {
        if (!node) return;
        inorder(node->left);
        cout << "    " << (node->completed ? "[x] " : "[ ] ")
             << node->name << " (" << node->id << ")\n";
        inorder(node->right);
    }

    // ── destroy ─────────────────────────────
    void destroy(TaskNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }
};