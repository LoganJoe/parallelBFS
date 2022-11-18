//
// Created by Logan on 2022/11/16.
//

#include <algorithm>
#include "queue.h"

SlidingQueue::SlidingQueue(int size) {
    queue = new Node[size];
    head = 0, tail = 0, back = 0;
}

SlidingQueue::SlidingQueue() {
    queue = new Node[16384];
    head = 0, tail = 0, back = 0;
}

SlidingQueue::~SlidingQueue() {
    delete[] queue;
}

void SlidingQueue::clear() {
    head = tail = back = 0;
}

void SlidingQueue::push_back(Node val) {
    queue[back++] = val;
}

void SlidingQueue::slide() {
    head = tail;
    tail = back;
}

bool SlidingQueue::empty() {
    return head == tail;
}

Node *SlidingQueue::begin() {
    return queue + head;
}


Node *SlidingQueue::end() {
    return queue + tail;
}

int SlidingQueue::size() {
    return tail - head;
}

BufferredQueue::BufferredQueue(SlidingQueue &master_, int size_) : master(master_) {
    size = size_;
    tail = 0;
    lq = new Node[size];
}

void BufferredQueue::flush() {
    Node *shared = master.queue;
    int from = __sync_fetch_and_add(&master.back, tail);
    std::copy(lq, lq + tail, shared + from);
    tail = 0;
}

void BufferredQueue::push_back(int val) {
    if (tail == size) flush();
    lq[tail++] = val;
}



