#include <deque>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include "../../include/pazusoba/timer.h"

#define SIZE 1000000

void pointer_test();
void heap_test();

std::mutex mtx;

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
    auto thread_count = std::thread::hardware_concurrency();
    // thread_count = 1;

    std::vector<std::thread> threads;
    std::vector<TEST*> v;

    int size = SIZE / thread_count;
    std::cout << "thread_count: " << thread_count << std::endl;
    std::cout << "size: " << size << std::endl;
    for (int i = 0; i < thread_count; i++) {
        threads.emplace_back([&] {
            for (int j = 0; j < size; ++j) {
                mtx.lock();
                v.push_back(new TEST);
                mtx.unlock();
            }
        });
    }

    for (auto& t : threads)
        t.join();
    threads.clear();

    int sum = 0;
    for (int i = 0; i < SIZE; i++) {
        if (v[i] == nullptr)
            continue;
        sum += v[i]->a;
        delete v[i];
    }
    v.clear();
}

void heap_test() {
    pazusoba::Timer timer("=> heap");
    auto thread_count = std::thread::hardware_concurrency();
    // thread_count = 1;

    std::vector<std::thread> threads;
    std::vector<TEST> v;

    int size = SIZE / thread_count;
    std::cout << "thread_count: " << thread_count << std::endl;
    std::cout << "size: " << size << std::endl;
    for (int i = 0; i < thread_count; i++) {
        threads.emplace_back([&] {
            for (int j = 0; j < size; ++j) {
                std::lock_guard<std::mutex> lk(mtx);

                // mtx.lock();
                v.push_back(TEST());
                // mtx.unlock();
            }
        });
    }

    for (auto& t : threads)
        t.join();
    threads.clear();

    int sum = 0;
    for (int i = 0; i < SIZE; ++i) {
        sum += v[i].a;
    }
    v.clear();
}
