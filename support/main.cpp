#include <pazusoba/core.h>

int main(int argc, char* argv[]) {
    auto solver = pazusoba::solver();
    solver.parse_args(argc, argv);

    // pass in combo as different as there is no way
    // this can be done from the command line
    pazusoba::profile profiles[1];
    profiles[0].name = pazusoba::target_combo;
    profiles[0].stop_threshold = 100;
    solver.set_profiles(profiles, 1);

    pazusoba::Timer timer("adventure");
    auto state = solver.adventure();
    solver.print_state(state);
    return 0;
}