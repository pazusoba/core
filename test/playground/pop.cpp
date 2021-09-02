#include <deque>
#include <iostream>
#include <queue>
#include <vector>
#include "../../include/pazusoba/timer.h"

#define SIZE 10000

void testVector();
void testDeque();
void testQueue();

int main() {
    pazusoba::Timer timer("=> main");
    // Deque is the fastest and queue is just a bit slower than it
    // Vector is slow but still similar to Queue
    testVector();
    testDeque();
    testQueue();
    return 0;
}

void testVector() {
    pazusoba::Timer timer("=> testVector");
    std::vector<int> v;
    for (int i = 0; i < SIZE; ++i) {
        v.emplace_back(i);
    }

    int counter = 0;
    while (!v.empty()) {
        auto c = v.back();
        v.pop_back();
        std::cout << c << std::endl;
        counter++;
    }

    // 0.004001s (4.001000ms)
    // 0.002999s (2.999100ms) with emplace
    std::cout << "Vector: " << counter << std::endl;
}

void testDeque() {
    pazusoba::Timer timer("=> testDeque");
    std::deque<int> d;
    for (int i = 0; i < SIZE; ++i) {
        d.emplace_front(i);
    }

    int counter = 0;
    while (!d.empty()) {
        auto c = d.front();
        d.pop_front();
        std::cout << c << std::endl;
        counter++;
    }

    // 0.003007s (3.007400ms)
    // 0.001999s (1.999200ms) with emplace/push front
    std::cout << "Deque: " << counter << std::endl;
}

void testQueue() {
    pazusoba::Timer timer("=> testQueue");
    std::queue<int> q;
    for (int i = 0; i < SIZE; ++i) {
        q.emplace(i);
    }

    int counter = 0;
    while (!q.empty()) {
        auto c = q.front();
        q.pop();
        std::cout << c << std::endl;
        counter++;
    }

    // 0.003028s (3.028500ms)
    // 0.003434s (3.434500ms) with emplace
    std::cout << "Queue: " << counter << std::endl;
}