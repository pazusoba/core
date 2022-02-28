#include <cstdio>
#include "pazusoba.h"

extern "C" {
void explore(int argc, char* argv[]) {
    DEBUG_PRINT("Calling from shared library\n");
    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    auto solver = pazusoba::solver();
    solver.parse_args(argc, argv);
    solver.explore();
}
}