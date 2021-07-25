/// Try out the best way for storing the game board
/// Array vs Long long int
///
/// => array will be used, if memory is not a concern anymore, INT should be
/// used instead of CHAR

#include <array>
#include <iostream>
#include "../../include/pazusoba/timer.h"

#define LOOP_COUNT 10000000
#define ARRAY_SIZE 42

// => Array Copy, 0.069228s (69.227928ms)
void testArrayCopy() {
    pazusoba::Timer timer("=> Array Copy");
    // Copy an array of ints
    std::array<char, ARRAY_SIZE> a = {-1};
    for (int i = 0; i < LOOP_COUNT; i++) {
        std::array<char, ARRAY_SIZE> b = a;
    }
}

// => Memory Copy, 0.069700s (69.699509ms), about the same as array copy,
// sometimes better and sometimes worse, probably due to alignment issue
void testMemcpy() {
    pazusoba::Timer timer("=> Memory Copy");
    // int is actually faster here compared to unsigned char
    // but still a lot slower than array copy
    int a[ARRAY_SIZE] = {-1};
    for (int i = 0; i < LOOP_COUNT; i++) {
        int b[ARRAY_SIZE];
        memcpy(b, a, sizeof(int) * ARRAY_SIZE);
    }
}

// => Long Copy, 0.019439s(19.438799ms)
// This is a clear winner but this makes access challenging
void testLongCopy() {
    pazusoba::Timer timer("=> Long Copy");
    // Copy 3 long long int
    long long int a = 21321432;
    long long int b = 1245325;
    long long int c = 78576;
    long long int d = 78576;
    long long int e = 78576;
    for (int x = 0; x < LOOP_COUNT; x++) {
        auto f = a;
        auto g = b;
        auto h = c;
        auto i = d;
        auto j = e;
    }
}

void testLongArrayCopy() {
    pazusoba::Timer timer("=> Long Array Copy");
    // Copy an array of long long int
    constexpr int size = ARRAY_SIZE / 16 + 1;
    long long int a[size] = {0};
    for (int i = 0; i < LOOP_COUNT; i++) {
        long long int b[size];
        memcpy(b, a, size * sizeof(long long int));
    }
}

/// => Loop, 0.015667s (15.667294ms)
void emptyLoop() {
    pazusoba::Timer timer("=> Loop");
    for (int i = 0; i < LOOP_COUNT; i++) {
    }
}

void testArrayAccess() {
    pazusoba::Timer timer("=> Array Access");
    // Access an array of ints
    std::array<int, ARRAY_SIZE> a = {0};
    for (int i = 0; i < LOOP_COUNT; i++) {
        auto b = a;
        b[0] = 123234;
    }
}

int extractInt(int orig16BitWord, unsigned int from, unsigned int to) {
    auto mask = ((1 << (to - from + 1)) - 1) << from;
    return (orig16BitWord & mask) >> from;
}

// It is still better to use array instead of long long int
void testBitAccess() {
    pazusoba::Timer timer("=> Bit Access");
    long long int a = 12659832759024;
    long long int b = 1243253254356;
    long long int c = 6573545243;
    for (int i = 0; i < LOOP_COUNT; i++) {
        for (int j = 0; j < ARRAY_SIZE; j++)
            auto r = extractInt(a, 4, 8);
    }
}

int main() {
    emptyLoop();
    testArrayCopy();
    testLongCopy();
    testLongArrayCopy();
    testMemcpy();

    // testArrayAccess();
    // testBitAccess();
    return 0;
}
