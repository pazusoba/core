#include "pazusoba.h"
#include "timer.h"

int main(int argc, char* argv[]) {
    auto solver = pazusoba::solver();
    solver.parse_args(argc, argv);

    // pass in combo as different as there is no way
    // this can be done from the command line
    pazusoba::profile profiles[1];
    profiles[0].name = pazusoba::target_combo;
    solver.set_profiles(profiles, 1);

    pazusoba::Timer timer("adventure");
    solver.adventure();
    return 0;
}