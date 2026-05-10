#pragma once
#include <string>
#include <iostream>
using namespace std;

// =====================================================
//  HashMap  (separate chaining)
//  Key = string, Value = templated T
//
//  Uses djb2 hash. Each bucket is a linked chain.
//
//  insert   : O(1) avg
//  search   : O(1) avg
//  remove   : O(1) avg
//  Space    : O(n)
//  Rehashes at 0.75 load factor — O(n)
// =====================================================
template <typename V>
class HashMap {
    struct Node {
        string key;
        V      value;
        Node*  next;
        Node(string k, V v) : key(k), value(v), next(nullptr) {}
    };

    static const int INITIAL_CAP = 16;
    int    capacity;
    int    count;
    Node** buckets;

public:
    HashMap() : capacity(INITIAL_CAP), count(0) {
        buckets = new Node*[capacity]();
    }

    ~HashMap() {
        for (int i = 0; i < capacity; i++) {
            Node* cur = buckets[i];
            while (cur) { Node* tmp = cur->next; delete cur; cur = tmp; }
        }
        delete[] buckets;
    }

    // O(1) avg
    void insert(const string& key, const V& value) {
        if ((double)count / capacity >= 0.75) rehash();
        int idx = hash(key);
        Node* cur = buckets[idx];
        while (cur) {
            if (cur->key == key) { cur->value = value; return; }
            cur = cur->next;
        }
        Node* node  = new Node(key, value);
        node->next  = buckets[idx];
        buckets[idx] = node;
        count++;
    }

    // O(1) avg — returns nullptr if not found
    V* search(const string& key) {
        Node* cur = buckets[hash(key)];
        while (cur) {
            if (cur->key == key) return &cur->value;
            cur = cur->next;
        }
        return nullptr;
    }

    // O(1) avg
    bool remove(const string& key) {
        int idx = hash(key);
        Node* cur  = buckets[idx];
        Node* prev = nullptr;
        while (cur) {
            if (cur->key == key) {
                if (prev) prev->next = cur->next;
                else       buckets[idx] = cur->next;
                delete cur; count--; return true;
            }
            prev = cur; cur = cur->next;
        }
        return false;
    }

    bool contains(const string& key) { return search(key) != nullptr; }
    int  getCount() const { return count; }

    template <typename Fn>
    void forEach(Fn fn) const {
        for (int i = 0; i < capacity; i++) {
            Node* cur = buckets[i];
            while (cur) { fn(cur->key, cur->value); cur = cur->next; }
        }
    }

    void printStats() const {
        cout << "  HashMap: " << count << " entries, "
             << capacity << " buckets, load="
             << (double)count/capacity << "\n";
    }

private:
    int hash(const string& key) const {
        unsigned long h = 5381;
        for (char c : key) h = ((h << 5) + h) + (unsigned char)c;
        return (int)(h % (unsigned long)capacity);
    }

    void rehash() {
        int    oldCap     = capacity;
        Node** oldBuckets = buckets;
        capacity  = capacity * 2;
        count     = 0;
        buckets   = new Node*[capacity]();
        for (int i = 0; i < oldCap; i++) {
            Node* cur = oldBuckets[i];
            while (cur) {
                insert(cur->key, cur->value);
                Node* tmp = cur->next; delete cur; cur = tmp;
            }
        }
        delete[] oldBuckets;
    }
};