#include "pazusoba.h"

int main(int argc, char* argv[]) {
    auto solver = pazusoba::solver();
    solver.parse_args(argc, argv);
    solver.explore();
    return 0;
}