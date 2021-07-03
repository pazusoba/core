#include <stdio.h>
#include "include/core.h"

int main()
{
    printf("Hello, CMake. Hash is %u\n", pazusoba::hash::djb2_hash("Hello World"));
    return 0;
}
