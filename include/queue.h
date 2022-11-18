//
// Created by Logan on 2022/11/16.
//

#ifndef BFS_QUEUE_H
#define BFS_QUEUE_H

#include <algorithm>

class BufferredQueue;

typedef int32_t Node;

class SlidingQueue {
    Node *queue;
    int head, tail, back;
public:
    explicit SlidingQueue(int size);

    SlidingQueue();

    ~SlidingQueue();

    void clear();

    void push_back(Node val);

    void slide();

    bool empty();

    Node *begin();

    Node *end();

    int size();

    friend class BufferredQueue;
};

class BufferredQueue {
    SlidingQueue &master;
    int size;
    int tail;
    Node *lq;

public:
    BufferredQueue(SlidingQueue &master, int size = 16384*4);

    ~BufferredQueue() { delete[] lq; }

    void flush();

    void push_back(int val);
};

#endif //BFS_QUEUE_H
