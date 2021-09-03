#include <deque>
#include <iostream>
#include <queue>
#include <vector>
#include "../../include/pazusoba/timer.h"

#define SIZE 1000000

void pointer_test();
void heap_test();

struct TEST {
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;
    int e = 0;
    int f = 0;
    int g = 0;
    int h = 0;
    std::vector<int> v;
    std::deque<int> dq;
    std::queue<int> q;
};

int main() {
    pointer_test();
    heap_test();
    return 0;
}

void pointer_test() {
    pazusoba::Timer timer("=> pointer");
    std::vector<TEST*> v;
    for (int i = 0; i < SIZE; ++i) {
        v.push_back(new TEST);
    }

    int sum = 0;
    for (int i = 0; i < SIZE; ++i) {
        sum += v[i]->a;
        delete v[i];
    }
    v.clear();
}

void heap_test() {
    pazusoba::Timer timer("=> heap");
    std::vector<TEST> v;
    for (int i = 0; i < SIZE; ++i) {
        v.push_back(TEST());
    }

    int sum = 0;
    for (int i = 0; i < SIZE; ++i) {
        sum += v[i].a;
    }
    v.clear();
}
