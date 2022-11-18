//
// Created by Logan on 2022/11/16.
//

#include "bitmap.h"

Bitmap::Bitmap(int size) {
    uint64_t num_words = (size + BITWIDTH - 1) / BITWIDTH;
    bitset = new uint64_t[num_words];
    tail = bitset + num_words;
    reset();
}

Bitmap::Bitmap(const Bitmap &bitmap) {
    uint64_t num_words = bitmap.tail - bitmap.bitset;
    bitset = new uint64_t[num_words];
    tail = bitset + num_words;
    std::copy(bitmap.bitset, bitmap.tail, bitset);
}

Bitmap::Bitmap() {
    uint64_t num_words = 16384;
    bitset = new uint64_t[num_words];
    tail = bitset + num_words;
    reset();
}

Bitmap::~Bitmap() {
    delete [] bitset;
}

void Bitmap::reset() {
    std::fill(bitset, tail, 0);
}

uint64_t Bitmap::arrIdx(int n) {
    return n / BITWIDTH;
}

uint64_t Bitmap::bitIdx(int n) {
    return n & (BITWIDTH - 1);
}

bool Bitmap::checkBit(int n) {
    return bitset[arrIdx(n)] & (1l << bitIdx(n));
}

void Bitmap::setBit(int n) {
    bitset[arrIdx(n)] |= ((uint64_t) 1l << bitIdx(n));
}

void Bitmap::setBitAtomic(int n) {
    uint64_t curVal, newVal;
    do {
        curVal = bitset[arrIdx(n)];
        newVal = curVal | ((uint64_t) 1l << bitIdx(n));
    } while (!__sync_bool_compare_and_swap(&bitset[arrIdx(n)], curVal, newVal));
}

void Bitmap::swap(Bitmap *bitmap) {
    std::swap(bitset, bitmap->bitset);
    std::swap(tail, bitmap->tail);
}

