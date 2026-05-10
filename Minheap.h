#pragma once
#include <iostream>
#include <stdexcept>
using namespace std;

// =====================================================
//  MinHeap  (array-based, templated)
//  T must support operator<
//
//  insert      : O(log n) — heapifyUp
//  extractMin  : O(log n) — heapifyDown
//  peekMin     : O(1)
//  Space       : O(n)
//
//  Parent of i   : (i-1)/2
//  Left child    : 2*i + 1
//  Right child   : 2*i + 2
// =====================================================
template <typename T>
class MinHeap {
    T*  data;
    int capacity;
    int size;

    static const int INITIAL_CAP = 16;

public:
    MinHeap() : capacity(INITIAL_CAP), size(0) {
        data = new T[capacity];
    }
    ~MinHeap() { delete[] data; }

    // O(log n)
    void insert(const T& item) {
        if (size == capacity) grow();
        data[size] = item;
        heapifyUp(size);
        size++;
    }

    // O(log n)
    T extractMin() {
        if (size == 0) throw runtime_error("Heap is empty");
        T minVal = data[0];
        data[0]  = data[size-1];
        size--;
        if (size > 0) heapifyDown(0);
        return minVal;
    }

    // O(1)
    T peekMin() const {
        if (size == 0) throw runtime_error("Heap is empty");
        return data[0];
    }

    bool isEmpty()  const { return size == 0; }
    int  getSize()  const { return size; }

private:
    void heapifyUp(int i) {
        while (i > 0) {
            int p = (i-1)/2;
            if (data[i] < data[p]) { swap(data[i], data[p]); i = p; }
            else break;
        }
    }

    void heapifyDown(int i) {
        while (true) {
            int l = 2*i+1, r = 2*i+2, s = i;
            if (l < size && data[l] < data[s]) s = l;
            if (r < size && data[r] < data[s]) s = r;
            if (s != i) { swap(data[i], data[s]); i = s; }
            else break;
        }
    }

    void grow() {
        capacity *= 2;
        T* nd = new T[capacity];
        for (int i = 0; i < size; i++) nd[i] = data[i];
        delete[] data; data = nd;
    }
};