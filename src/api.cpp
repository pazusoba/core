#include <pazusoba/core.h>
#include <cstdio>

extern "C" {
// Holds the row & column of where to go next
struct c_location {
    int row = -1;
    int column = -1;
};

// C interface of pazusoba::state
struct c_state {
    int combo;
    int max_combo;
    int step;
    int row;
    int column;
    bool goal;
    // add the first step here as well
    c_location routes[MAX_DEPTH + 1];
    // TODO: this should be a c type not std::array but it works
    pazusoba::game_board board;
};

c_state convert(const pazusoba::solver& solver, const pazusoba::state& state) {
    int column = solver.column();
    int step = state.step;

    c_state c_state;
    c_state.combo = state.combo;
    c_state.max_combo = solver.max_combo();
    c_state.step = state.step;
    c_state.row = solver.row();
    c_state.column = solver.column();
    c_state.goal = state.goal;
    c_state.board = state.board;

    // set the first location
    c_state.routes[0].row = state.begin / column;
    c_state.routes[0].column = state.begin % column;

    // up, down, left and right
    int directions[] = {-column, column, -1, 1};
    // get the route from state
    const auto& route = state.route;

    int max_index = step / ROUTE_PER_LIST;
    // in case, it doesn't fill up the space, check the offset
    int offset = step % ROUTE_PER_LIST;

    int count = 0;
    int index = 0;
    int curr_loc = state.begin;
    while (index <= max_index) {
        auto curr = route[index];
        int limit = ROUTE_PER_LIST;
        if (index == max_index) {
            limit = offset;
            // shift the number to the left
            curr <<= (ROUTE_PER_LIST - offset) * 3;
        }

        for (int i = 0; i < limit; i++) {
            count++;
            // get first 3 bits and shift to the right, 3 * 20
            curr_loc += directions[(curr & ROUTE_MASK) >> 60];
            auto& loc = c_state.routes[count];
            loc.row = curr_loc / column;
            loc.column = curr_loc % column;

            // prepare for the next step
            curr <<= 3;
        }
        index++;
    }

    return c_state;
}

c_state adventureEx(const char* board,
                    int min_erase,
                    int search_depth,
                    int beam_size,
                    pazusoba::profile* profiles,
                    int count) {
    auto solver = pazusoba::solver();
    solver.set_board(board);
    solver.set_min_erase(min_erase);
    solver.set_search_depth(search_depth);
    solver.set_beam_size(beam_size);
    solver.set_profiles(profiles, count);
    auto state = solver.adventure();
    return convert(solver, state);
}

c_state adventure(int argc, char* argv[]) {
    DEBUG_PRINT("Calling from shared library\n");
    for (int i = 0; i < argc; i++) {
        DEBUG_PRINT("argv[%d] = %s\n", i, argv[i]);
    }

    auto solver = pazusoba::solver();
    solver.parse_args(argc, argv);
    auto state = solver.adventure();
    return convert(solver, state);
}
}