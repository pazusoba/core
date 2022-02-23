// pazusoba.cpp
// Try to improve the performance of the solver while being flexible enough

// Compile with
// mac: clang++ -std=c++11 -fopenmp -O2 pazusoba.cpp -o pazusoba
// windows: g++ -std=c++11 -fopenmp -O2 pazusoba.cpp -o pazusoba

#include <omp.h>
#include <algorithm>
#include <array>
#include <cstring>
#include <deque>
#include <iostream>
#include <set>
#include <vector>

namespace pazusoba {
#define DEBUG 1
#define DEBUG_PRINT(...) \
    if (DEBUG)           \
        printf(__VA_ARGS__);

#define MAX_DEPTH 150
#define MIN_BEAM_SIZE 1000
#define MAX_BOARD_LENGTH 42
#define MIN_STATE_SCORE -9999
#define ALLOW_DIAGONAL 0

#define ORB_COUNT 14
#define DIRECTION_COUNT 8

// TODO: 100% needs to be improved
#define INDEX_OF(x, y) (x * COLUMN + y)

typedef unsigned char orb, tiny;
typedef std::array<orb, MAX_BOARD_LENGTH> game_board, visit_board;

/// Match names https://pad.dawnglare.com/ use (not all orbs are supported)
const char ORB_WEB_NAME[ORB_COUNT] = {' ', 'R', 'B', 'G', 'L', 'D', 'H',
                                      'J', ' ', 'P', ' ', ' ', ' ', ' '};

// initalise after board size is decided
tiny DIRECTION_ADJUSTMENTS[DIRECTION_COUNT];
/// All 8 possible directions
enum DIRECTIONS {
    up = 0,
    down,
    left,
    right,
    // after right, all moves are diagonal
    up_left,
    up_right,
    down_left,
    down_right
};

struct state {
    // could be improved by swapping indexes instead of copying
    game_board board;
    tiny begin;
    tiny prev;
    tiny curr;
    tiny step = 0;
    tiny combo = 0;
    short int score = MIN_STATE_SCORE;
    // 64 bits can store 21 steps 3 * 21
    // if we don't include diagonals,
    // 64 bits can store 32 steps 2 * 32
    std::array<long long int, MAX_DEPTH / 21 + 1> route{0};
    int operator>(const state& other) const { return score > other.score; }
};

// this helps to calculate the distance between a kind of orb
struct orb_distance {
    tiny min = 0;
    tiny max = 0;
};

// TODO: can be updated
struct combo {
    orb info;
    std::set<tiny> loc;
    combo(const orb& o) : info(o) {}
};
typedef std::vector<combo> combo_list;

///
/// global variables, they shouldn't be changed outside parse_args()
///
int MIN_ERASE = 3;
int SEARCH_DEPTH = 100;
int BEAM_SIZE = 10000;
int ROW, COLUMN;
int MAX_COMBO;
int BOARD_SIZE;
game_board BOARD;
// count the number of each orb to calculate the max combo (not 100% correct)
std::array<orb, ORB_COUNT> ORB_COUNTER;

///
/// const marks the function pure and testable
///

// find the best possible move by exploring the board
void explore();
// expand current state to all possible next moves
inline void expand(const game_board&, const state&, std::vector<state>&, int);
// erase the board, count the combo and calculate the score
inline void evaluate(game_board&, state&);
// TODO: can be improved
void erase_combo(game_board&, visit_board&, std::deque<int>&, combo&, int, int);
void erase_orbs(game_board&, combo_list&);
void move_orbs_down(game_board&);
const void print_board(const game_board&);
// A naive way to approach max combo, mostly accurate unless it is two colour
const int calc_max_combo(const std::array<orb, ORB_COUNT>&,
                         const int,
                         const int);
void parse_args(int argc, char* argv[]);
const void usage();

void explore() {
    // setup the state, non blocking
    std::vector<state> look;
    look.resize(BEAM_SIZE);
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
        new_state.score = 0;
        look[i] = new_state;
    }

    // beam search with openmp
    for (int i = 0; i < SEARCH_DEPTH; i++) {
#pragma omp parallel for shared(look, temp)
        for (int j = 0; j < BEAM_SIZE; j++) {
            if (found_max_combo)
                continue;  // early stop

            const state& curr = look[j];
            if (curr.score == MIN_STATE_SCORE)
                continue;  // uninitialized state

            if (curr.combo >= MAX_COMBO) {
                best_state = curr;
                found_max_combo = true;
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

        // for (int i = 0; i < 10; i++) {
        //     print_board(temp[i].board);
        //     DEBUG_PRINT("%d\n", temp[i].score);
        // }

        // (end - begin) gets the size of the vector, divide by 3 to get the
        // number of states we consider in the next step
        std::copy(begin, begin + (end - begin) / 3, look.begin());
        best_state = look[0];
    }

    DEBUG_PRINT("best score: %d\n", best_state.combo);
    print_board(best_state.board);
}

inline void expand(const game_board& board,
                   const state& current,
                   std::vector<state>& states,
                   int loc) {
    int count = DIRECTION_COUNT;
    if (!ALLOW_DIAGONAL)
        count = 4;

    auto prev = current.prev;
    auto curr = current.curr;
    auto step = current.step;
    for (int i = 0; i < count; i++) {
        // this is set from parse_args()
        tiny adjustments = DIRECTION_ADJUSTMENTS[i];
        tiny next = curr + adjustments;
        if (next == prev)
            continue;  // invalid, same position
        if (next - prev == 1 && next % COLUMN == 0)
            continue;  // invalid, on the right edge
        if (prev - next == 1 && prev % COLUMN == 0)
            continue;  // invalid, on the left edge
        if (next >= BOARD_SIZE)
            continue;  // invalid, out of bound

        state new_state;
        new_state.step = step + 1;
        new_state.curr = next;
        new_state.prev = curr;
        new_state.begin = current.begin;

        // insert to the route
        tiny route_index = new_state.step / 21;
        new_state.route[route_index] = current.route[route_index] << 3 | i;

        if (step == 0)
            new_state.board = BOARD;
        else
            new_state.board = current.board;

        // swap the board
        auto& new_board = new_state.board;
        auto temp = new_board[curr];
        new_board[curr] = new_board[next];
        new_board[next] = temp;

        evaluate(new_board, new_state);

        // insert to the states
        if (step == 0)
            states[loc * 4 + i] = new_state;
        else
            states[loc * 3 + i] = new_state;
    }
}

inline void evaluate(game_board& board, state& new_state) {
    short int score = 0;
    // scan the board to get the distance between each orb
    orb_distance distance[ORB_COUNT];
    for (int i = 0; i < BOARD_SIZE; i++) {
        auto& orb = board[i];
        // 0 to 5 only for 6x5, 6 to 11 will convert to 0 to 5
        tiny loc = i % COLUMN;
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

    tiny combo = 0;
    tiny move_count = 0;
    while (true) {
        erase_orbs(board, list);
        tiny combo_count = list.size();
        // Check if there are more combo
        if (combo_count > combo) {
            move_orbs_down(board);
            combo = combo_count;
            move_count++;
        } else {
            break;
        }
    }

    combo = list.size();
    new_state.score = score + (combo * 20);
}

void erase_combo(game_board& board,
                 visit_board& visited,
                 std::deque<int>& queue,
                 combo& combo,
                 int ox,
                 int oy) {
    // assume this index is valid
    auto orb_index = INDEX_OF(ox, oy);
    auto orb = board[orb_index];
    // should reduce the time, this function is called but how?
    queue.emplace_front(orb_index);  // 25%

    while (!queue.empty()) {
        auto currIndex = queue.front();
        queue.pop_front();
        if (visited[currIndex])
            continue;
        else
            visited[currIndex] = 1;

        int x = currIndex / COLUMN;
        // int y = currIndex % COLUMN;
        int y = currIndex - x * COLUMN;

        // Check vertically from x
        // go up from x
        bool vup = true;
        int vupIndex = x;
        while (vup) {
            int cx = vupIndex - 1;
            if (cx >= ROW) {
                vup = false;
            } else if (board[INDEX_OF(cx, y)] == orb) {
                vupIndex--;
            } else {
                vup = false;
            }
        }
        // go down from x
        bool vdown = true;
        int vdownIndex = x;
        while (vdown) {
            int cx = vdownIndex + 1;
            if (cx >= ROW) {
                vdown = false;
            } else if (board[INDEX_OF(cx, y)] == orb) {
                vdownIndex++;
            } else {
                vdown = false;
            }
        }

        // as long as three orbs are connected, it can be erased
        // min erase doesn't matter at all here
        bool vErase = vdownIndex - vupIndex >= 2;
        for (int i = vupIndex; i <= vdownIndex; i++) {
            auto currIndex = INDEX_OF(i, y);
            if (vErase) {
                // add this location and clear the orb
                combo.loc.insert(currIndex);  // 7.8%
                board[currIndex] = 0;
                // to be visited
                if (i != x)
                    queue.emplace_front(currIndex);
            } else {
                // simply go and visit it
                queue.emplace_front(currIndex);
            }
        }

        // Check horizontally from y
        // go left from y
        bool hleft = true;
        int hleftIndex = y;
        while (hleft) {
            int cy = hleftIndex - 1;
            if (cy >= COLUMN) {
                hleft = false;
            } else if (board[INDEX_OF(x, cy)] == orb) {
                hleftIndex--;
            } else {
                hleft = false;
            }
        }
        // go right from y
        bool hright = true;
        int hrightIndex = y;
        while (hright) {
            int cy = hrightIndex + 1;
            if (cy >= COLUMN) {
                hright = false;
            } else if (board[INDEX_OF(x, cy)] == orb) {
                hrightIndex++;
            } else {
                hright = false;
            }
        }

        bool hErase = hrightIndex - hleftIndex >= 2;
        for (int i = hleftIndex; i <= hrightIndex; i++) {
            auto currIndex = INDEX_OF(x, i);
            if (hErase) {
                combo.loc.insert(currIndex);  // 7.8%
                board[currIndex] = 0;
                if (i != y)
                    queue.emplace_front(currIndex);
            } else {
                queue.emplace_front(currIndex);
            }
        }
    }
}

void erase_orbs(game_board& board, combo_list& list) {
    visit_board erased;
    game_board copy = board;

    for (int x = 0; x < ROW; x++) {
        for (int y = 0; y < COLUMN; y++) {
            auto orb = board[INDEX_OF(x, y)];
            // ignore empty orbs
            if (orb == 0)
                continue;

            combo combo(orb);
            // 58%, can be optimised, improving erasecombo can increase
            // the speed very significantly
            std::deque<int> queue;
            erase_combo(copy, erased, queue, combo, x, y);
            if (combo.loc.size() >= MIN_ERASE) {
                list.push_back(combo);  // 24% here, can be optimized
                // maybe a callback not sure how
            }
        }
    }
}

void move_orbs_down(game_board& board) {
    // TODO: maybe should taking min erase into account
    // because it is impossible to erase only one orb
    for (int i = 0; i < COLUMN; ++i) {
        int emptyIndex = -1;
        // signed type is needed or otherwise, j >= won't terminate at all
        // because after -1 is the max value again
        for (int j = ROW - 1; j >= 0; --j) {
            auto index = INDEX_OF(j, i);
            auto orb = board[index];
            if (orb == 0) {
                // Don't override empty index if available
                if (emptyIndex == -1)
                    emptyIndex = j;
            } else if (emptyIndex != -1) {
                // replace last known empty index
                // and replace it with current index
                board[INDEX_OF(emptyIndex, i)] = orb;
                board[index] = 0;
                // simply move it up from last index
                --emptyIndex;
            }
        }
    }
}

const void print_board(const game_board& board) {
    DEBUG_PRINT("board: ");
    for (int i = 0; i < MAX_BOARD_LENGTH; i++) {
        DEBUG_PRINT("%c", ORB_WEB_NAME[board[i]]);
    }
    DEBUG_PRINT("\n");
}

const int calc_max_combo(const std::array<orb, ORB_COUNT>& counter,
                         const int size,
                         const int min_erase) {
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

void parse_args(int argc, char* argv[]) {
    if (argc <= 1)
        usage();

    // min_erase needs to know before parsing the board,
    // this is to calculate the max combo
    if (argc > 2) {
        int min_erase = atoi(argv[2]);
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

    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            usage();
        } else {
            DEBUG_PRINT("=============== INFO ===============\n");
            auto board_string = argv[1];
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
                    printf("orb %c not found, only RBGLDHJP are valid\n",
                           orb_char);
                    exit(1);
                }
            }

            MAX_COMBO = calc_max_combo(ORB_COUNTER, BOARD_SIZE, MIN_ERASE);
        }
    }

    if (argc > 3) {
        int depth = atoi(argv[3]);
        if (depth > MAX_DEPTH)
            depth = MAX_DEPTH;
        SEARCH_DEPTH = depth;
    }

    if (argc > 4) {
        int beam_size = atoi(argv[4]);
        if (beam_size < MIN_BEAM_SIZE)
            beam_size = MIN_BEAM_SIZE;
        BEAM_SIZE = beam_size;
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

const void usage() {
    printf(
        "\nusage: pazusoba [board string] [min erase] [max steps] [max "
        "beam size]\nboard string\t-- "
        "eg. RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB\nmin erase\t-- 3 to 5\nmax "
        "steps\t-- maximum steps before the program stops "
        "searching\nmax beam size\t-- the width of the search space, "
        "larger number means slower speed but better results\n\nMore "
        "at https://github.com/HenryQuan/pazusoba\n\n");
    exit(0);
}
};  // namespace pazusoba

int main(int argc, char* argv[]) {
    pazusoba::parse_args(argc, argv);
    pazusoba::explore();
    return 0;
}