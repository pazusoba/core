// pazusoba.cpp
// Try to improve the performance of the solver while being flexible enough

// Compile with
// mac: clang++ -std=c++20 -fopenmp -O2 pazusoba.cpp -o pazusoba
// windows: g++ -std=c++20 -fopenmp -O2 pazusoba.cpp -o pazusoba

#include <pazusoba/core.h>
#include <algorithm>
#include <atomic>
#include <cstring>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace pazusoba {
state solver::adventure() {
    constexpr double BEAM_SIZE_MULTIPLIER = 1.4;
    int REAL_BEAM_SIZE = static_cast<int>(BEAM_SIZE * BEAM_SIZE_MULTIPLIER);
    
    // setup the state, non blocking
    std::vector<state> look;
    look.reserve(REAL_BEAM_SIZE);
    
    // insert to temp, sort and copy back to look
    std::vector<state> temp;
    temp.reserve(REAL_BEAM_SIZE * 3);
    
    state best_state;
    std::atomic<bool> found_max_combo{false};  // Thread-safe flag

    // assign all possible states to look
    for (int i = 0; i < BOARD_SIZE; ++i) {
        state new_state;
        new_state.curr = i;
        new_state.prev = i;
        new_state.begin = i;
        new_state.score = MIN_STATE_SCORE + 1;
        look.push_back(new_state);
    }

    // setup threading with bounds checking
    unsigned int processor_count = std::thread::hardware_concurrency();
    if (processor_count == 0) processor_count = 1;  // Fallback to single thread
    
    std::vector<std::thread> threads;
    threads.reserve(processor_count);

    int stop_count = 0;

    // beam search with thread-safe state management
    for (int i = 0; i < SEARCH_DEPTH; i++) {
        if (found_max_combo.load(std::memory_order_acquire))
            break;

        int look_size = look.size();
        debug_print("Depth %d - size %d\n", i + 1, look_size);
        int look_size_thread = look_size / processor_count;

        // Create thread-local buffers to avoid race conditions
        // Pre-size vectors for indexed assignment in expand()
        std::vector<std::vector<state>> thread_local_states(processor_count);
        for (auto& local_temp : thread_local_states) {
            local_temp.resize(look_size_thread * 4);  // 4 directions max per state
        }

        for (unsigned int thread_num = 0; thread_num < processor_count; thread_num++) {
            threads.emplace_back([&, thread_num]() {
                int start_index = thread_num * look_size_thread;
                int end_index = start_index + look_size_thread;
                
                // Handle last thread getting remaining work
                if (thread_num == processor_count - 1) {
                    end_index = look_size;
                }
                
                auto& local_states = thread_local_states[thread_num];
                
                for (int j = start_index; j < end_index; j++) {
                    if (found_max_combo.load(std::memory_order_acquire))
                        break;  // early stop

                    const state& curr = look[j];

                    if (curr.goal) {
                        // Use atomic compare-exchange to safely update best_state
                        bool expected = false;
                        if (found_max_combo.compare_exchange_strong(expected, true, std::memory_order_release)) {
                            best_state = curr;  // First thread to find goal wins
                        }
                        continue;
                    }

                    expand(curr.board, curr, local_states, j);
                }
            });
        }

        for (auto& t : threads)
            t.join();
        threads.clear();

        // break out as soon as max combo or target is found
        if (found_max_combo.load(std::memory_order_acquire))
            break;

        debug_print("Depth %d - merging and sorting\n", i + 1);

        // Merge thread-local results into temp
        temp.clear();
        for (const auto& local_states : thread_local_states) {
            temp.insert(temp.end(), local_states.begin(), local_states.end());
        }

        // sorting
        auto begin = temp.begin();
        auto end = temp.end();
        std::sort(begin, end, std::greater<state>());

        if constexpr (DEBUG) {
            for (int k = 0; k < 5 && k < static_cast<int>(temp.size()); k++) {
                debug_print("combo %d\n", temp[k].combo);
            }
        }

        // (end - begin) gets the size of the vector, divide by 3 to get the
        // number of states we consider in the next step

        look.clear();
        // we need to filter out the states that are already visited
        // Using insert().second to check and insert in one operation (single hash lookup)
        look.clear();
        for (int j = 0; j < REAL_BEAM_SIZE && j < static_cast<int>(temp.size()); j++) {
            const auto& curr = temp[j];
            
            // Check and insert in one operation
            if (!VISITED.insert(curr.hash).second) {
                // Already visited, skip
                continue;
            }
            
            if (curr.score > best_state.score) {
                best_state = curr;
                stop_count = 0;
            }

            // break if empty boards are hit
            if (curr.score == MIN_STATE_SCORE) {
                break;
            }
            look.push_back(curr);
        }

        // std::copy(begin, begin + (end - begin) / 3, look.begin());
        stop_count++;
        if (stop_count > STOP_THRESHOLD) {
            break;
        }
    }

    // print_state(best_state);
    return best_state;
}  // namespace pazusoba

void solver::expand(const game_board& board,
                    const state& current,
                    std::vector<state>& states,
                    const int loc) {
    const int count = ALLOW_DIAGONAL ? DIRECTION_COUNT : 4;

    const auto prev = current.prev;
    const auto curr = current.curr;
    const auto step = current.step;
    
    for (int i = 0; i < count; i++) {
        // this is set from parse_args()
        const int adjustments = DIRECTION_ADJUSTMENTS[i];
        const tiny next = curr + adjustments;
        
        // Boundary and validity checks - optimized to check once
        if (next == prev) continue;  // invalid, same position
        if (next >= BOARD_SIZE) continue;  // invalid, out of bound
        if (next - curr == 1 && next % COLUMN == 0) continue;  // invalid, on the right edge
        if (curr - next == 1 && curr % COLUMN == 0) continue;  // invalid, on the left edge

        // Use C++20 designated initializers for clarity
        state new_state{
            .board = (step == 0) ? BOARD : board,
            .begin = current.begin,
            .prev = curr,
            .curr = next,
            .step = static_cast<tiny>(step + 1),
            .route = current.route
        };

        // insert to the route
        int route_index = new_state.step / ROUTE_PER_LIST;
        if (new_state.step % ROUTE_PER_LIST == 0)
            route_index--;  // the last one in the previous number
        new_state.route[route_index] = new_state.route[route_index] << 3 | i;

        // swap the board - using std::swap for clarity
        auto& new_board = new_state.board;
        std::swap(new_board[curr], new_board[next]);

        // calculate the hash
        new_state.hash = hash::pazusoba_hash(new_board.data(), new_state.prev);

        // evaluate the board
        evaluate(new_board, new_state);

        // insert to the states - use indexing for test compatibility
        // For thread-local states, this is safe as each thread writes to its own vector
        if (step == 0) {
            states[loc * 4 + i] = new_state;
        } else {
            states[loc * 3 + i] = new_state;
        }
    }
}

void solver::evaluate(game_board& board, state& new_state) {
    short int score = 0;
    
    // scan the board to get the distance between each orb
    std::array<orb_distance, ORB_COUNT> distance{};
    for (int i = 0; i < BOARD_SIZE; i++) {
        const auto& orb_val = board[i];
        const int loc = i % COLUMN;
        if (loc > distance[orb_val].max)
            distance[orb_val].max = loc;
        else if (loc < distance[orb_val].min)
            distance[orb_val].min = loc;
    }

    for (const auto& dist : distance) {
        score -= (dist.max - dist.min);
    }

    // erase the board and find out the combo number
    // Pre-allocate combo_list to reduce allocations
    combo_list list;
    const int reserve_size = (MAX_COMBO > 0) ? MAX_COMBO : 10;  // Safe default
    list.reserve(reserve_size);

    int combo = 0;
    int move_count = 0;
    game_board copy = board;
    
    // Limit cascade simulation to reasonable depth
    constexpr int MAX_CASCADE_DEPTH = 10;
    while (move_count < MAX_CASCADE_DEPTH) {
        list.clear();  // Reuse the vector instead of creating new one
        erase_combo(copy, list);
        const int combo_count = list.size();
        
        // Check if there are more combo
        if (combo_count > combo) {
            combo = combo_count;
            move_orbs_down(copy);
            move_count++;
        } else {
            break;
        }
    }

    // track if all goals are reached
    int goal = 0;
    for (int i = 0; i < PROFILE_COUNT; i++) {
        const auto& profile = PROFILES[i];
        switch (profile.name) {
            case ProfileName::target_combo: {
                int target = profile.target;
                if (target == -1) {
                    // max combo
                    score += combo * 20;
                    if (combo == MAX_COMBO)
                        goal++;
                } else {
                    // only do max target combo
                    if (combo < target)
                        score -= (7 - target) * 30;
                    if (combo == target)
                        score += 50;
                    else if (target > 7)
                        score -= 50;

                    if (combo == target)
                        goal++;
                }
            } break;

            case ProfileName::colour: {
                int colour_counter[ORB_COUNT]{0};
                for (const auto& c : list) {
                    colour_counter[c.info]++;
                }

                bool has_all_target_colours = true;
                for (int j = 0; j < ORB_COUNT; j++) {
                    // this orb should be included
                    if (profile.orbs[j]) {
                        // just add a tiny score, don't do too much
                        if (colour_counter[j] == 0)
                            has_all_target_colours = false;
                        else
                            score += 2;
                    }
                }

                if (has_all_target_colours)
                    goal++;
            } break;

            case ProfileName::colour_combo: {
                int colour_counter[ORB_COUNT]{0};
                for (const auto& c : list) {
                    colour_counter[c.info]++;
                }

                bool fulfilled = true;
                for (int j = 0; j < ORB_COUNT; j++) {
                    // this orb should be included
                    if (profile.orbs[j]) {
                        int colour_combo = colour_counter[j];
                        // just add a tiny score, don't do too much
                        if (colour_combo == 0)
                            fulfilled = false;
                        else if (colour_combo >= profile.target)
                            score += 2;
                    }
                }

                if (fulfilled)
                    goal++;
            } break;

            case ProfileName::connected_orb: {
                int target = profile.target;
                bool fulfilled = false;

                for (const auto& c : list) {
                    int connected_count = c.loc.size();
                    if (ORB_COUNTER[c.info] >= target) {
                        if (connected_count < target) {
                            score += (connected_count - MIN_ERASE) * 10;
                        } else if (connected_count == target) {
                            // fulfilled = true;
                            score += 50;
                        } else {
                            score -= (connected_count - target) * 50;
                        }
                    }
                }

                score += combo * 20;

                if (fulfilled)
                    goal++;
            } break;

            case ProfileName::orb_remaining: {
                int remaining = 0;
                for (int j = 0; j < BOARD_SIZE; j++) {
                    if (copy[j] > 0)
                        remaining++;
                }

                if (remaining <= profile.target)
                    goal++;
                score -= remaining * 10;
            } break;

            case ProfileName::shape_L: {
                for (const auto& c : list) {
                    if (profile.orbs[c.info] && ORB_COUNTER[c.info] >= 5) {
                        int size = c.loc.size();
                        if (size == 5) {
                            // some score for connecting more orbs
                            // check if it is L shape
                            std::map<int, int> vertical;
                            std::map<int, int> horizontal;
                            int bigFirst = -1;
                            int bigSecond = -1;

                            // Collect info
                            for (const auto& loc : c.loc) {
                                int x = loc % COLUMN;
                                int y = loc / COLUMN;
                                vertical[x]++;
                                horizontal[y]++;

                                // Track the largest number
                                if (vertical[x] >= 3)
                                    bigFirst = x;
                                if (horizontal[y] >= 3)
                                    bigSecond = y;
                            }

                            // This is the center point
                            if (bigFirst > -1 && bigSecond > -1) {
                                int counter = 0;
                                // Check if bigFirst -2 or +2 exists
                                if (vertical[bigFirst - 2] > 0 ||
                                    vertical[bigFirst + 2] > 0)
                                    counter++;
                                // Same for bigSecond
                                if (horizontal[bigSecond - 2] > 0 ||
                                    horizontal[bigSecond + 2] > 0)
                                    counter++;

                                if (counter == 2)
                                    score += 50;
                            }
                        } else if (size > 3) {
                            score += 10;
                        }
                    }
                }

                // consider combo here as well
                score += combo * 20;
            } break;

            case ProfileName::shape_plus: {
                for (const auto& c : list) {
                    if (profile.orbs[c.info] && ORB_COUNTER[c.info] >= 5) {
                        int size = c.loc.size();
                        if (size <= 5)
                            score += (size - MIN_ERASE) * 10;

                        // some score for connecting more orbs
                        // check if it is L shape
                        std::map<int, int> vertical;
                        std::map<int, int> horizontal;
                        int bigFirst = -1;
                        int bigSecond = -1;

                        // Collect info
                        for (const auto& loc : c.loc) {
                            int x = loc % COLUMN;
                            int y = loc / COLUMN;
                            vertical[x]++;
                            horizontal[y]++;

                            // Track the largest number
                            if (vertical[x] >= 3)
                                bigFirst = x;
                            if (horizontal[y] >= 3)
                                bigSecond = y;
                        }

                        // This is the center point
                        if (bigFirst > -1 && bigSecond > -1) {
                            int counter = 0;
                            // Check up down left right there is an orb around
                            // center orb
                            if (vertical[bigFirst - 1] > 0 &&
                                vertical[bigFirst + 1] > 0)
                                counter++;
                            if (horizontal[bigSecond - 1] > 0 &&
                                horizontal[bigSecond + 1] > 0)
                                counter++;

                            if (counter == 2)
                                score += 50;
                            if (counter == 1)
                                score += 10;
                        }
                    }
                }
            } break;

            case ProfileName::shape_square: {
            } break;

            case ProfileName::shape_row: {
            } break;

            case ProfileName::shape_column: {
            } break;

            default: {
                std::printf("unknown profile %d\n", static_cast<int>(profile.name));
                exit(1);
            } break;
        }
    }

    new_state.combo = combo;
    new_state.score = score;

    if (goal == PROFILE_COUNT) {
        new_state.goal = true;
    }
}

void solver::erase_combo(game_board& board, combo_list& list) {
    visit_board visited_location{};  // Initialize to zero
    
    // start from the bottom and check for combos
    for (int curr_index = BOARD_SIZE - 1; curr_index >= 0; curr_index--) {
        if (visited_location[curr_index])
            continue;  // already visited even if it is not erased

        const auto orb_val = board[curr_index];
        if (orb_val == 0)
            continue;  // already erased

        combo c(orb_val);
        c.loc.reserve(15);  // Pre-reserve space for connected orbs
        
        std::queue<int> visit_queue;
        visit_queue.emplace(curr_index);

        // start exploring until all connected orbs are visited
        while (!visit_queue.empty()) {
            const int to_visit = visit_queue.front();
            visit_queue.pop();

            // number of connected orbs in all directions
            std::array<int, 4> counter{};

            // check all four directions
            for (int i = 0; i < 4; i++) {
                const int direction = DIRECTION_ADJUSTMENTS[i];
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

                    if (board[next] == orb_val) {
                        // same colour
                        visited_location[next] = true;
                        counter[i]++;

                        // check if there are orbs in the different direction
                        for (int j = 0; j < 4; j++) {
                            if (i < 2 && j < 2)
                                continue;  // only search left & right
                            if (i >= 2 && j >= 2)
                                continue;  // only search up & down

                            const int dir2 = DIRECTION_ADJUSTMENTS[j];
                            tiny nearby = next;
                            
                            if (dir2 == -1 && nearby % COLUMN == 0)
                                continue;  // invalid, on the left edge

                            nearby += dir2;

                            if (dir2 == 1 && nearby % COLUMN == 0)
                                continue;  // invalid, on the right edge
                            if (nearby >= BOARD_SIZE)
                                continue;  // invalid, out of bound
                            if (visited_location[nearby])
                                continue;  // invalid, already visited

                            // same orb in different direction, should visit
                            if (board[nearby] == orb_val) {
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
    // Move orbs down after erasing combos - optimized column-wise iteration
    for (int col = 0; col < COLUMN; ++col) {
        int emptyIndex = -1;
        // signed type is needed or otherwise, j >= won't terminate at all
        // because after -1 is the max value again
        for (int row = ROW - 1; row >= 0; --row) {
            const int index = index_of(row, col);
            const orb o = board[index];
            
            if (o == 0) {
                // Don't override empty index if available
                if (emptyIndex == -1)
                    emptyIndex = row;
            } else if (emptyIndex != -1) {
                // replace last known empty index
                // and replace it with current index
                board[index_of(emptyIndex, col)] = o;
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
        int min_erase = std::atoi(argv[2]);
        set_min_erase(min_erase);
    }

    if (argc > 1) {
        if (std::strcmp(argv[1], "--help") == 0 || std::strcmp(argv[1], "-h") == 0) {
            usage();
        } else {
            debug_print("=============== INFO ===============\n");
            auto board_string = argv[1];
            set_board(board_string);
        }
    }

    if (argc > 3) {
        int depth = std::atoi(argv[3]);
        set_search_depth(depth);
    }

    if (argc > 4) {
        int beam_size = std::atoi(argv[4]);
        set_beam_size(beam_size);
    }

    print_board(BOARD);
    debug_print("board size: %d\n", BOARD_SIZE);
    debug_print("row x column: %d x %d\n", ROW, COLUMN);
    debug_print("min_erase: %d\n", MIN_ERASE);
    debug_print("max_combo: %d\n", MAX_COMBO);
    debug_print("search_depth: %d\n", SEARCH_DEPTH);
    debug_print("beam_size: %d\n", BEAM_SIZE);
    debug_print("====================================\n");
}

void solver::set_board(const char* board_string) {
    const int board_size = std::strlen(board_string);

    // there are only 3 fixed size board -> 20, 30 or 42
    if (board_size > MAX_BOARD_LENGTH) {
        std::printf("Board string is too long\n");
        std::exit(1);
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
        std::printf("Unsupported board size - %d\n", board_size);
        std::exit(1);
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
        debug_print("min_erase is too small, set to 3\n");
    } else if (min_erase > 5) {
        min_erase = 5;
        debug_print("min_erase is too large, set to 5\n");
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

void solver::set_profiles(profile* profiles, int count) {
    PROFILES = profiles;
    PROFILE_COUNT = count;
    for (int i = 0; i < count; i++) {
        // use the largest threshold
        if (STOP_THRESHOLD < profiles[i].stop_threshold)
            STOP_THRESHOLD = profiles[i].stop_threshold;
    }
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
    std::printf("=============== STATE ===============\n");
    if (state.step == 0) {
        std::fprintf(stderr, "Warning: Invalid state (step == 0)\n");
        return;
    }

    std::printf("Score: %d\n", state.score);
    std::printf("Combo: %d/%d\n", state.combo, MAX_COMBO);
    std::printf("Step: %d\n", state.step);
    print_board(state.board);
    print_route(state.route, state.step, state.begin);
    std::printf("Goal: %d\n", state.goal);
    std::printf("=====================================\n");
}

void solver::print_route(const route_list& route,
                         const int step,
                         const int begin) const {
    std::printf("Route: |%d| - ", begin);
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
            std::printf("%c", DIRECTION_NAME[dir]);
            count++;
            // prepare for the next step
            curr <<= 3;
        }
        index++;
    }
    std::printf("\n");
    if (count != step) {
        std::printf("count (%d) should be equal to step (%d)\n", count, step);
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
    std::printf(
        "\nusage: pazusoba [board string] [min erase] [max steps] [max "
        "beam size]\nboard string\t-- "
        "eg. RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB\nmin erase\t-- 3 to 5\nmax "
        "steps\t-- maximum steps before the program stops "
        "searching\nmax beam size\t-- the width of the search space, "
        "larger number means slower speed but better results\n\nMore "
        "at https://github.com/pazusoba/core\n\n");
    std::exit(0);
}
}  // namespace pazusoba
