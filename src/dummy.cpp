#include <fmt/core.h>
#include <pazusoba/core.h>

int main() {
    pazusoba::Timer timer("Main");
    fmt::print("The answer is {}.\n", 42);
    const unsigned char* hello = (const unsigned char*)"Hello World";
    fmt::print("Hello, CMake. Hash is {}.\n", pazusoba::hash::djb2_hash(hello));
    return 0;
}
