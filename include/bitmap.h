//
// Created by Logan on 2022/11/16.
//

#ifndef BFS_BITMAP_H
#define BFS_BITMAP_H

#include <algorithm>
#include <cstdint>

#define BITWIDTH 64

class Bitmap {
    uint64_t *bitset, *tail;
public:
    explicit Bitmap(int size);

    Bitmap();

    Bitmap(const Bitmap &bitmap);

    ~Bitmap();
private:
    static uint64_t arrIdx(int n);

    static uint64_t bitIdx(int n);

public:
    void reset();

    bool checkBit(int n);

    void setBit(int n);

    void setBitAtomic(int n);

    void swap(Bitmap *bitmap);

};


#endif //BFS_BITMAP_H
