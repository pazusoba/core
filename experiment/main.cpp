#include "pazusoba.h"
#include "timer.h"

int main(int argc, char* argv[]) {
    auto solver = pazusoba::solver();
    solver.parse_args(argc, argv);
    pazusoba::Timer timer("explore");
    solver.explore();
    return 0;
}