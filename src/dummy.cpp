#include <stdio.h>
#include "pazusoba.h"
#include <fmt/core.h>

int main()
{
    fmt::print("The answer is {}.\n", 42);
    int a = 0;
    const unsigned char *hello = (const unsigned char *)"Hello World";
    printf("Hello, CMake. Hash is %u\n", pazusoba::hash::djb2_hash(hello));
    return 0;
}
