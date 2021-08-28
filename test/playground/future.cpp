// future::wait_for
#include <chrono>    // std::chrono::milliseconds
#include <future>    // std::async, std::future
#include <iostream>  // std::cout
#include <thread>

#define delay 10000

// a non-optimized way of checking for prime numbers:
bool is_prime(int x) {
    // std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    for (int i = 2; i < x; ++i)
        if (x % i == 0)
            return false;
    return true;
}

int main() {
    std::cout << "checking, please wait";
    // call function asynchronously:
    std::future<bool> fut = std::async(is_prime, 700020007);

    // do something while waiting for function to set future:
    std::cout << "checking 2 ";

    std::chrono::milliseconds span(delay);
    std::cout << "checking 3 ";
    fut.wait_for(span);
    std::cout << "checking 4 ";

    bool x = fut.get();

    std::cout << "\n700020007 " << (x ? "is" : "is not") << " prime.\n";

    return 0;
}