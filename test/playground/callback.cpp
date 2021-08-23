#include <functional>
#include <iostream>
#include <vector>

void loop(const std::function<void(int)>& f);

int main() {
    std::vector<int> l;
    // it works
    loop([&l](int i) { l.push_back(i); });

    for (const auto& n : l) {
        std::cout << n << std::endl;
    }
    return 0;
}

void loop(const std::function<void(int)>& f) {
    for (int i = 0; i < 100; ++i) {
        f(i);
    }
}