#pragma once
#ifndef PAZUSOBA_H
#define PAZUSOBA_H

#include <array>
#include <deque>
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
#define STOP_THRESHOLD 10

#define ORB_COUNT 14
#define DIRECTION_COUNT 8

// TODO: 100% needs to be improved
#define INDEX_OF(x, y) (x * COLUMN + y)

typedef unsigned char orb, tiny;
typedef std::array<orb, MAX_BOARD_LENGTH> game_board, visit_board;

/// Match names https://pad.dawnglare.com/ use (not all orbs are supported)
const char ORB_WEB_NAME[ORB_COUNT] = {' ', 'R', 'B', 'G', 'L', 'D', 'H',
                                      'J', ' ', 'P', ' ', ' ', ' ', ' '};

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
const void print_state(const state&);
// A naive way to approach max combo, mostly accurate unless it is two colour
const int calc_max_combo(const std::array<orb, ORB_COUNT>&,
                         const int,
                         const int);
void parse_args(int argc, char* argv[]);
const void usage();
};  // namespace pazusoba

#endif
