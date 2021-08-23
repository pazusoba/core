#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <vector>

class OOO {
    int _n;
    int _random;

public:
    OOO(int n) : _n(n) {
        _random = (rand() % 9999999) + 1;
        std::cout << "OOO(" << _n << ") - " << _random << std::endl;
    }
    // ~OOO() { std::cout << "~OOO(" << _n << ") - " << _random << std::endl; }
    OOO& operator=(const OOO& other) {
        std::cout << "OOO(" << _n << ") - Copied -" << _random << std::endl;
        return *this;
    }
    const int& n() const { return _n; }
};

void loop(const std::function<void(OOO)>& f);

int main() {
    std::vector<OOO> l;
    // it works
    loop([&l](OOO i) { l.push_back(i); });

    for (const auto& n : l) {
        std::cout << n.n() << std::endl;
    }
    return 0;
}

void loop(const std::function<void(OOO)>& f) {
    for (int i = 0; i < 100; ++i) {
        f(OOO(i));
    }
}