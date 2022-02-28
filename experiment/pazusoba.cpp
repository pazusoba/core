// pazusoba.cpp
// Try to improve the performance of the solver while being flexible enough

// Compile with
// mac: clang++ -std=c++11 -fopenmp -O2 pazusoba.cpp -o pazusoba
// windows: g++ -std=c++11 -fopenmp -O2 pazusoba.cpp -o pazusoba

#include "pazusoba.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <queue>
#include "hash.h"

namespace pazusoba {
state solver::explore() {
    // setup the state, non blocking
    std::vector<state> look;
    look.reserve(BEAM_SIZE);
    // insert to temp, sort and copy back to look
    std::vector<state> temp;
    temp.resize(BEAM_SIZE * 3);
    // TODO: using array can definitely things a lot because the vector needs to
    // write a lot of useless data before using it, reverse is better but the
    // address calculation can be tricky

    state best_state;
    bool found_max_combo = false;

    // assign all possible states to look
    for (int i = 0; i < BOARD_SIZE; ++i) {
        state new_state;
        new_state.curr = i;
        new_state.prev = i;
        new_state.begin = i;
        new_state.score = MIN_STATE_SCORE + 1;
        look.push_back(new_state);
    }

    int stop_count = 0;
    // beam search with openmp
    for (int i = 0; i < SEARCH_DEPTH; i++) {
        if (found_max_combo)
            break;

        int look_size = look.size();
        DEBUG_PRINT("Depth %d - size %d\n", i + 1, look_size);
#pragma omp parallel for
        for (int j = 0; j < look_size; j++) {
            if (found_max_combo)
                continue;  // early stop

            const state& curr = look[j];

            if (curr.combo >= MAX_COMBO) {
                best_state = curr;
                found_max_combo = true;
                continue;
            }

            expand(curr.board, curr, temp, j);
        }

        // break out as soon as max combo or target is found
        // TODO: this should be the target
        if (found_max_combo)
            break;

        // DEBUG_PRINT("%d... ", i);
        DEBUG_PRINT("Depth %d - sorting\n", i + 1);

        // sorting
        auto begin = temp.begin();
        auto end = temp.end();
        std::sort(begin, end, std::greater<state>());

        for (int i = 0; i < 5; i++) {
            // print_state(temp[i]);
            DEBUG_PRINT("combo %d\n", temp[i].combo);
        }

        // (end - begin) gets the size of the vector, divide by 3 to get the
        // number of states we consider in the next step

        look.clear();
        // we need to filter out the states that are already visited
        int index = 0;
        for (int j = 0; j < BEAM_SIZE; j++, index++) {
            auto& curr = temp[j];
            if (VISITED[curr.prev][curr.hash]) {
                index--;
            } else {
                VISITED[curr.prev][curr.hash] = true;
                if (curr.combo > best_state.combo) {
                    best_state = curr;
                    stop_count = 0;
                }

                // break if empty boards are hit
                if (curr.score == MIN_STATE_SCORE) {
                    break;
                }
                look.push_back(curr);
            }
        }

        // std::copy(begin, begin + (end - begin) / 3, look.begin());
        stop_count++;
        if (stop_count > STOP_THRESHOLD) {
            break;
        }
    }

    print_state(best_state);
    return best_state;
}  // namespace pazusoba

void solver::expand(const game_board& board,
                    const state& current,
                    std::vector<state>& states,
                    const int loc) {
    int count = DIRECTION_COUNT;
    if (!ALLOW_DIAGONAL)
        count = 4;

    auto prev = current.prev;
    auto curr = current.curr;
    auto step = current.step;
    for (int i = 0; i < count; i++) {
        // this is set from parse_args()
        int adjustments = DIRECTION_ADJUSTMENTS[i];
        tiny next = curr + adjustments;
        // todo: right edge can be checked before the calculation
        if (next == prev)
            continue;  // invalid, same position
        if (next - curr == 1 && next % COLUMN == 0)
            continue;  // invalid, on the right edge
        if (curr - next == 1 && curr % COLUMN == 0)
            continue;  // invalid, on the left edge
        if (next >= BOARD_SIZE)
            continue;  // invalid, out of bound

        state new_state;
        new_state.step = step + 1;
        new_state.curr = next;
        new_state.prev = curr;
        new_state.begin = current.begin;
        new_state.route = current.route;

        // insert to the route
        int route_index = new_state.step / ROUTE_PER_LIST;
        new_state.route[route_index] = new_state.route[route_index] << 3 | i;

        if (step == 0)
            new_state.board = BOARD;
        else
            new_state.board = board;

        // swap the board
        auto& new_board = new_state.board;
        auto temp = new_board[curr];
        new_board[curr] = new_board[next];
        new_board[next] = temp;

        // calculate the hash
        new_state.hash = hash::djb2_hash(new_board.data());

        // backup the board
        evaluate(new_board, new_state);

        // insert to the states
        if (step == 0)
            states[loc * 4 + i] = new_state;
        else
            states[loc * 3 + i] = new_state;
    }
}

void solver::evaluate(game_board& board, state& new_state) {
    short int score = 0;
    // scan the board to get the distance between each orb
    orb_distance distance[ORB_COUNT];
    for (int i = 0; i < BOARD_SIZE; i++) {
        auto& orb = board[i];
        // 0 to 5 only for 6x5, 6 to 11 will convert to 0 to 5
        int loc = i % COLUMN;
        if (loc > distance[orb].max)
            distance[orb].max = loc;
        else if (loc < distance[orb].min)
            distance[orb].min = loc;
    }

    for (int i = 0; i < ORB_COUNT; i++) {
        auto& dist = distance[i];
        score -= (dist.max - dist.min);
    }

    // erase the board and find out the combo number
    combo_list list;  // TODO: 515ms here, destructor is slow

    int combo = 0;
    int move_count = 0;
    game_board copy = board;
    while (true) {
        erase_combo(copy, list);
        int combo_count = list.size();
        // Check if there are more combo
        if (combo_count > combo) {
            combo = combo_count;
            move_orbs_down(copy);
            move_count++;
        } else {
            break;
        }
    }

    new_state.combo = combo;
    new_state.score = score + (combo * 20);
}

void solver::erase_combo(game_board& board, combo_list& list) {
    visit_board visited_location{0};
    // start from the bottom and check for combos
    for (int curr_index = BOARD_SIZE - 1; curr_index >= 0; curr_index--) {
        if (visited_location[curr_index])
            continue;  // already visited even if it is not erased

        auto orb = board[curr_index];
        if (orb == 0)
            continue;  // already erased

        combo c(orb);
        std::queue<int> visit_queue;
        visit_queue.emplace(curr_index);

        // start exploring until all connected orbs are visited
        while (!visit_queue.empty()) {
            int to_visit = visit_queue.front();
            visit_queue.pop();

            // if (visited_location[to_visit])
            //     continue;  // already visited

            // number of connected orbs in all directions
            int counter[4]{0};

            // check all four directions
            for (int i = 0; i < 4; i++) {
                int direction = DIRECTION_ADJUSTMENTS[i];
                // this needs to be unsigned to avoid negatives
                tiny next = to_visit;
                // going in that direction until a different orb is found
                while (true) {
                    if (direction == -1 && next % COLUMN == 0)
                        break;  // invalid, on the left edge

                    next += direction;

                    if (direction == 1 && next % COLUMN == 0)
                        break;  // invalid, on the right edge
                    if (next >= BOARD_SIZE)
                        break;  // invalid, out of bound

                    if (board[next] == orb) {
                        // same colour
                        visited_location[next] = true;
                        counter[i]++;

                        // check if there are orbs in the different direction
                        for (int j = 0; j < 4; j++) {
                            if (i < 2 && j < 2)
                                continue;  // only search left & right
                            if (i >= 2 && j >= 2)
                                continue;  // only search up & down

                            int direction = DIRECTION_ADJUSTMENTS[j];
                            tiny nearby = next;
                            if (direction == -1 && nearby % COLUMN == 0)
                                continue;  // invalid, on the left edge

                            nearby += direction;

                            if (direction == 1 && nearby % COLUMN == 0)
                                continue;  // invalid, on the right edge
                            if (nearby >= BOARD_SIZE)
                                continue;  // invalid, out of bound
                            if (visited_location[nearby])
                                continue;  // invalid, already visited

                            // same orb in different direction, should visit
                            if (board[nearby] == orb) {
                                // check next first before nearby
                                visit_queue.emplace(next);
                                visit_queue.emplace(nearby);
                            }
                        }
                    } else {
                        break;  // different colour
                    }
                }
            }

            // only 2 same orbs are needed to make 3 in a row
            if (counter[0] + counter[1] >= 2) {
                c.loc.insert(to_visit);
                board[to_visit] = 0;
                // up & down
                for (int i = -counter[0]; i <= counter[1]; i++) {
                    if (i == 0)
                        continue;  // this is the source orb itself
                    // convert index to location, -1 moves -6 for 6x5
                    auto index = to_visit + i * COLUMN;
                    c.loc.insert(index);
                    board[index] = 0;
                }
            }

            if (counter[2] + counter[3] >= 2) {
                c.loc.insert(to_visit);
                board[to_visit] = 0;
                // left & right
                for (int i = -counter[2]; i <= counter[3]; i++) {
                    if (i == 0)
                        continue;  // this is the source orb itself
                    auto index = to_visit + i;
                    c.loc.insert(index);
                    board[index] = 0;
                }
            }
        }

        // add this combo to the list
        if ((int)c.loc.size() >= MIN_ERASE)
            list.push_back(c);
    }
}

void solver::move_orbs_down(game_board& board) {
    // TODO: maybe should taking min erase into account
    // because it is impossible to erase only one orb
    for (int i = 0; i < COLUMN; ++i) {
        int emptyIndex = -1;
        // signed type is needed or otherwise, j >= won't terminate at all
        // because after -1 is the max value again
        for (int j = ROW - 1; j >= 0; --j) {
            int index = INDEX_OF(j, i);
            orb o = board[index];
            if (o == 0) {
                // Don't override empty index if available
                if (emptyIndex == -1)
                    emptyIndex = j;
            } else if (emptyIndex != -1) {
                // replace last known empty index
                // and replace it with current index
                board[INDEX_OF(emptyIndex, i)] = o;
                board[index] = 0;
                // simply move it up from last index
                --emptyIndex;
            }
        }
    }
}

int solver::calc_max_combo(const orb_list& counter,
                           const int size,
                           const int min_erase) const {
    // at least one combo when the board has only one orb
    int max_combo = 0;
    int threshold = size / 2;
    for (const auto& count : counter) {
        int combo = count / min_erase;
        // based on my experience, it is not possible to do more combo
        // if one colour has more than half the board
        // the max combo needs to be reduced by 2 times
        // RRRRRRRRRRRRRRRRRRRRRRRRGGGBBB can do max 4 combos naively
        // this is because R is taking up too much spaces
        // MAX_COMBO might not be 100% correct but it's a good reference
        if (count > threshold) {
            int extra_combo = (count - threshold) * 2 / min_erase;
            combo -= extra_combo;
        }
        max_combo += combo;
    }

    if (max_combo == 0)
        return 1;
    return max_combo;
}

void solver::parse_args(int argc, char* argv[]) {
    if (argc <= 1)
        usage();

    // min_erase needs to know before parsing the board,
    // this is to calculate the max combo
    if (argc > 2) {
        int min_erase = atoi(argv[2]);
        set_min_erase(min_erase);
    }

    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            usage();
        } else {
            DEBUG_PRINT("=============== INFO ===============\n");
            auto board_string = argv[1];
            set_board(board_string);
        }
    }

    if (argc > 3) {
        int depth = atoi(argv[3]);
        set_search_depth(depth);
    }

    if (argc > 4) {
        int beam_size = atoi(argv[4]);
        set_beam_size(beam_size);
    }

    print_board(BOARD);
    DEBUG_PRINT("board size: %d\n", BOARD_SIZE);
    DEBUG_PRINT("row x column: %d x %d\n", ROW, COLUMN);
    DEBUG_PRINT("min_erase: %d\n", MIN_ERASE);
    DEBUG_PRINT("max_combo: %d\n", MAX_COMBO);
    DEBUG_PRINT("search_depth: %d\n", SEARCH_DEPTH);
    DEBUG_PRINT("beam_size: %d\n", BEAM_SIZE);
    DEBUG_PRINT("====================================\n");
}

void solver::set_board(const char* board_string) {
    int board_size = strlen(board_string);

    // there are only 3 fixed size board -> 20, 30 or 42
    if (board_size > MAX_BOARD_LENGTH) {
        printf("Board string is too long\n");
        exit(1);
    } else if (board_size == 20) {
        ROW = 4;
        COLUMN = 5;
    } else if (board_size == 30) {
        ROW = 5;
        COLUMN = 6;
    } else if (board_size == 42) {
        ROW = 6;
        COLUMN = 7;
    } else {
        printf("Unsupported board size - %d\n", board_size);
        exit(1);
    }
    BOARD_SIZE = board_size;

    // set up DIRECTION_ADJUSTMENTS
    DIRECTION_ADJUSTMENTS[0] = -COLUMN;
    DIRECTION_ADJUSTMENTS[1] = COLUMN;
    DIRECTION_ADJUSTMENTS[2] = -1;
    DIRECTION_ADJUSTMENTS[3] = 1;
    DIRECTION_ADJUSTMENTS[4] = -COLUMN - 1;
    DIRECTION_ADJUSTMENTS[5] = -COLUMN + 1;
    DIRECTION_ADJUSTMENTS[6] = COLUMN - 1;
    DIRECTION_ADJUSTMENTS[7] = COLUMN + 1;

    // setup the board here by finding the orb using the string
    for (int i = 0; i < board_size; i++) {
        char orb_char = board_string[i];
        // find the orb name from ORB_WEB_NAME and make it a number
        bool found = false;
        for (orb j = 0; j < ORB_COUNT; j++) {
            if (orb_char == ORB_WEB_NAME[j]) {
                found = true;
                BOARD[i] = j;
                ORB_COUNTER[j]++;
                break;
            }
        }

        if (!found) {
            printf("orb %c not found, only RBGLDHJP are valid\n", orb_char);
            exit(1);
        }
    }

    MAX_COMBO = calc_max_combo(ORB_COUNTER, BOARD_SIZE, MIN_ERASE);
}

void solver::set_min_erase(int min_erase) {
    // min 3, max 5 for now
    if (min_erase < 3) {
        min_erase = 3;
        DEBUG_PRINT("min_erase is too small, set to 3\n");
    } else if (min_erase > 5) {
        min_erase = 5;
        DEBUG_PRINT("min_erase is too large, set to 5\n");
    }
    MIN_ERASE = min_erase;
}

void solver::set_search_depth(int depth) {
    if (depth > MAX_DEPTH)
        depth = MAX_DEPTH;
    SEARCH_DEPTH = depth;
}

void solver::set_beam_size(int beam_size) {
    if (beam_size < MIN_BEAM_SIZE)
        beam_size = MIN_BEAM_SIZE;
    BEAM_SIZE = beam_size;
}

void solver::print_board(const game_board& board) const {
    printf("Board: ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        auto orb = board[i];
        if (orb == 0)
            printf("P");
        else
            printf("%c", ORB_WEB_NAME[orb]);
    }
    printf("\n");
}

void solver::print_state(const state& state) const {
    printf("=============== STATE ===============\n");
    if (state.step == 0) {
        printf("Invalid state\n");
        exit(-1);
    }

    printf("Score: %d\n", state.score);
    printf("Combo: %d/%d\n", state.combo, MAX_COMBO);
    printf("Step: %d\n", state.step);
    print_board(state.board);
    print_route(state.route, state.step, state.begin);
    printf("=====================================\n");
}

void solver::print_route(const route_list& route,
                         const int step,
                         const int begin) const {
    printf("Route: |%d| - ", begin);
    int max_index = step / ROUTE_PER_LIST;
    // in case, it doesn't fill up the space, check the offset
    int offset = step % ROUTE_PER_LIST;

    int count = 0;
    int index = 0;
    while (index <= max_index) {
        auto curr = route[index];
        int limit = ROUTE_PER_LIST;
        if (index == max_index) {
            limit = offset;
            // shift the number to the left
            curr <<= (ROUTE_PER_LIST - offset) * 3;
        }

        for (int i = 0; i < limit; i++) {
            // get first 3 bits and shift to the right, 3 * 20
            int dir = (curr & ROUTE_MASK) >> 60;
            printf("%d", dir);
            // printf("%c", DIRECTION_NAME[dir]);
            count++;
            // prepare for the next step
            curr <<= 3;
        }
        index++;
    }
    printf("\n");
    if (count != step) {
        printf("count (%d) should be equal to step (%d)\n", count, step);
        exit(1);
    }
}

// This is for debugging only, don't use it in pazusoba.cpp
std::string solver::get_board_string(const game_board& board) const {
    char board_string[MAX_BOARD_LENGTH + 1]{};
    for (int i = 0; i < MAX_BOARD_LENGTH; i++) {
        auto orb = board[i];
        if (orb == 0)
            break;
        board_string[i] = ORB_WEB_NAME[orb];
    }
    return std::string(board_string);
}

void solver::usage() const {
    printf(
        "\nusage: pazusoba [board string] [min erase] [max steps] [max "
        "beam size]\nboard string\t-- "
        "eg. RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB\nmin erase\t-- 3 to 5\nmax "
        "steps\t-- maximum steps before the program stops "
        "searching\nmax beam size\t-- the width of the search space, "
        "larger number means slower speed but better results\n\nMore "
        "at https://github.com/pazusoba/core\n\n");
    exit(0);
}
}  // namespace pazusoba
