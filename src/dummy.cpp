#include <pazusoba/core.h>
#include <fmt/core.h>

int main()
{
    fmt::print("The answer is {}.\n", 42);
    int a = 0;
    const unsigned char *hello = (const unsigned char *)"Hello World";
    fmt::print("Hello, CMake. Hash is {}.\n", pazusoba::hash::djb2_hash(hello));
    return 0;
}
