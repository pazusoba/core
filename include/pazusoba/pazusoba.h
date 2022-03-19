#pragma once
#ifndef _PAZUSOBA_H_
#define _PAZUSOBA_H_

#include <array>
#include <deque>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace pazusoba {
// TODO: should replace all of these into const
#define DEBUG 1
#define DEBUG_PRINT(...) \
    if (DEBUG)           \
        printf(__VA_ARGS__);

#define MAX_DEPTH 150
#define MIN_BEAM_SIZE 1000
#define MAX_BOARD_LENGTH 42
#define MIN_STATE_SCORE -9999
// diagonal moves are no yet supported
#define ALLOW_DIAGONAL 0

#define ROUTE_PER_LIST 21
#define ROUTE_MASK 0x7000000000000000

#define ORB_COUNT 11
#define DIRECTION_COUNT 8

// TODO: 100% needs to be improved
#define INDEX_OF(x, y) (x * COLUMN + y)

typedef unsigned char orb, tiny;
typedef std::array<orb, MAX_BOARD_LENGTH> game_board, visit_board;
typedef std::array<orb, ORB_COUNT> orb_list;
typedef std::array<long long int, MAX_DEPTH / ROUTE_PER_LIST + 1> route_list;

// Empty, Fire, Water, Wood, Light, Dark, Heal, Jammer, Bomb, Poison, Poison+
/// Match names https://pad.dawnglare.com/ use (not all orbs are supported)
const char ORB_WEB_NAME[ORB_COUNT] = {' ', 'R', 'B', 'G', 'L', 'D',
                                      'H', 'J', 'E', 'P', 'T'};

const char DIRECTION_NAME[4] = {'U', 'D', 'L', 'R'};

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
    game_board board{0};
    tiny begin;
    tiny prev;
    tiny curr;
    tiny step = 0;
    tiny combo = 0;
    bool goal = false;
    long long int hash;
    short int score = MIN_STATE_SCORE;
    // 64 bits can store 21 steps 3 * 21
    // if we don't include diagonals,
    // 64 bits can store 32 steps 2 * 32
    route_list route{0};
    int operator>(const state& other) const { return score > other.score; }
};

enum PROFILE_NAME {
    target_combo = 0,  // target certain combo, -1 means max combo
    colour,            // how many colours should be included, 5, 6
    colour_combo,      // how many combo for one colour, 2, 3
    connected_orb,     // how many orbs connected, 4, 5, 6
    orb_remaining,     // how many orbs remaining, usually less than 5
    shape_L,           // L shape
    shape_plus,        // + plus shape, 十字
    shape_square,      // square shape, 無効貫通
    shape_row,         // row shape, one line
    shape_column,      // column shape, 追撃
};

struct profile {
    // PROFILE_NAME
    int name = -1;
    // After how many steps should it stop if better states can't be found
    int stop_threshold = 20;
    // Which orbs should be considered
    int target = -1;
    bool orbs[ORB_COUNT]{false};
};

// this helps to calculate the distance between a kind of orb
struct orb_distance {
    int min = 0;
    int max = 0;
};

// TODO: can be updated
struct combo {
    orb info;
    std::unordered_set<int> loc;
    combo(const orb& o) : info(o) {}
};
typedef std::vector<combo> combo_list;

class solver {
    ///
    /// class variables, they shouldn't be changed outside parse_args()
    ///
    int MIN_ERASE = 3;
    int SEARCH_DEPTH = 100;
    int BEAM_SIZE = 10000;
    int ROW, COLUMN;
    int MAX_COMBO;
    int BOARD_SIZE;
    int STOP_THRESHOLD = 20;
    profile* PROFILES;
    int PROFILE_COUNT = 0;

    game_board BOARD;
    // count the number of each orb to calculate the max combo (not 100%
    // correct)
    std::array<orb, ORB_COUNT> ORB_COUNTER;
    std::unordered_map<long long int, bool> VISITED[MAX_BOARD_LENGTH];

    // initalise after board size is decided
    int DIRECTION_ADJUSTMENTS[DIRECTION_COUNT];

public:
    ///
    /// const marks the function pure and testable
    ///
    // find the best possible move by exploring the board
    state adventure();
    // expand current state to all possible next moves
    void expand(const game_board&,
                const state&,
                std::vector<state>&,
                const int);
    // erase the board, count the combo and calculate the score
    void evaluate(game_board&, state&);
    void erase_combo(game_board&, combo_list&);
    void move_orbs_down(game_board&);
    // A naive way to approach max combo, mostly accurate unless 2 colours
    int calc_max_combo(const orb_list&, const int, const int) const;

    void parse_args(int argc, char* argv[]);
    // set board from string, setup row and column, calculate max combo and
    // also, setup DIRECTION_ADJUSTMENTS for expand()
    void set_board(const char*);
    void set_min_erase(int);
    void set_search_depth(int);
    void set_beam_size(int);
    void set_profiles(profile*, int);

    void print_board(const game_board&) const;
    void print_state(const state&) const;
    void print_route(const route_list&, const int, const int) const;
    std::string get_board_string(const game_board&) const;
    void usage() const;

    // getters
    int min_erase() const { return MIN_ERASE; }
    int search_depth() const { return SEARCH_DEPTH; }
    int beam_size() const { return BEAM_SIZE; }
    int row() const { return ROW; }
    int column() const { return COLUMN; }
    int max_combo() const { return MAX_COMBO; }
    int board_size() const { return BOARD_SIZE; }
    const game_board& board() const { return BOARD; }
};
}  // namespace pazusoba

#endif
