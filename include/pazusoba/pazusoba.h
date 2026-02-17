#pragma once
#ifndef _PAZUSOBA_H_
#define _PAZUSOBA_H_

#include <array>
#include <atomic>
#include <concepts>
#include <cstdio>
#include <deque>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace pazusoba {
// Modern C++20 constants replacing macros - using inline constexpr for ODR-safety
inline constexpr bool DEBUG = false;
inline constexpr int MAX_DEPTH = 150;
inline constexpr int MIN_BEAM_SIZE = 100;
inline constexpr int MAX_BOARD_LENGTH = 42;
inline constexpr int MIN_STATE_SCORE = -9999;
inline constexpr bool ALLOW_DIAGONAL = false;
inline constexpr int ROUTE_PER_LIST = 21;
inline constexpr long long ROUTE_MASK = 0x7000000000000000;
inline constexpr int ORB_COUNT = 11;
inline constexpr int DIRECTION_COUNT = 8;
inline constexpr double BEAM_SIZE_MULTIPLIER = 1.4;

// C++20 Concepts for type safety
template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template<typename T>
concept OrbType = std::same_as<T, unsigned char>;

// Debug print helper - using if constexpr for zero runtime cost in release builds
template<typename... Args>
inline void debug_print([[maybe_unused]] const char* format, [[maybe_unused]] Args... args) noexcept {
    if constexpr (DEBUG) {
        std::printf(format, args...);
    }
}

// Modern type aliases using C++20 style
using orb = unsigned char;
using tiny = unsigned char;
using game_board = std::array<orb, MAX_BOARD_LENGTH>;
using visit_board = std::array<orb, MAX_BOARD_LENGTH>;
using orb_list = std::array<orb, ORB_COUNT>;
using route_list = std::array<long long int, MAX_DEPTH / ROUTE_PER_LIST + 1>;

// Empty, Fire, Water, Wood, Light, Dark, Heal, Jammer, Bomb, Poison, Poison+
/// Match names https://pad.dawnglare.com/ use (not all orbs are supported)
inline constexpr std::array<char, ORB_COUNT> ORB_WEB_NAME = {
    ' ', 'R', 'B', 'G', 'L', 'D', 'H', 'J', 'E', 'P', 'T'
};

inline constexpr std::array<char, 4> DIRECTION_NAME = {'U', 'D', 'L', 'R'};

/// All 8 possible directions - using enum class for type safety
enum class Direction : unsigned char {
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

// C++20 three-way comparison for Direction
[[nodiscard]] constexpr auto operator<=>(Direction lhs, Direction rhs) noexcept {
    return static_cast<unsigned char>(lhs) <=> static_cast<unsigned char>(rhs);
}

struct state {
    // C++20 designated initializers can be used to construct this
    game_board board{};
    tiny begin = 0;
    tiny prev = 0;
    tiny curr = 0;
    tiny step = 0;
    tiny combo = 0;
    bool goal = false;
    long long int hash = 0;
    short int score = MIN_STATE_SCORE;
    route_list route{};
    
    // C++20 three-way comparison operator for sorting
    [[nodiscard]] constexpr auto operator<=>(const state& other) const noexcept {
        return score <=> other.score;
    }
    
    // Equality operator needed alongside <=>
    [[nodiscard]] constexpr bool operator==(const state& other) const noexcept = default;
    
    // For compatibility with existing code
    [[nodiscard]] constexpr bool operator>(const state& other) const noexcept {
        return score > other.score;
    }
};

enum class ProfileName : int {
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
    ProfileName name = ProfileName::target_combo;
    // After how many steps should it stop if better states can't be found
    int stop_threshold = 20;
    // Which orbs should be considered
    int target = -1;
    std::array<bool, ORB_COUNT> orbs{};
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
    explicit combo(const orb& o) : info(o) {}
};
using combo_list = std::vector<combo>;

class solver {
    ///
    /// class variables, they shouldn't be changed outside parse_args()
    ///
    int MIN_ERASE = 3;
    int SEARCH_DEPTH = 100;
    int BEAM_SIZE = 10000;
    int ROW = 0, COLUMN = 0;
    int MAX_COMBO = 0;
    int BOARD_SIZE = 0;
    int STOP_THRESHOLD = 20;
    profile* PROFILES = nullptr;
    int PROFILE_COUNT = 0;

    game_board BOARD{};
    // count the number of each orb to calculate the max combo (not 100%
    // correct)
    orb_list ORB_COUNTER{};
    std::unordered_set<long long int> VISITED;

    // initialize after board size is decided
    std::array<int, DIRECTION_COUNT> DIRECTION_ADJUSTMENTS{};

    // Helper function to convert 2D coordinates to 1D index
    [[nodiscard]] inline int index_of(int x, int y) const noexcept {
        return x * COLUMN + y;
    }

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

    // getters - marked [[nodiscard]] to encourage proper usage
    [[nodiscard]] int min_erase() const noexcept { return MIN_ERASE; }
    [[nodiscard]] int search_depth() const noexcept { return SEARCH_DEPTH; }
    [[nodiscard]] int beam_size() const noexcept { return BEAM_SIZE; }
    [[nodiscard]] int row() const noexcept { return ROW; }
    [[nodiscard]] int column() const noexcept { return COLUMN; }
    [[nodiscard]] int max_combo() const noexcept { return MAX_COMBO; }
    [[nodiscard]] int board_size() const noexcept { return BOARD_SIZE; }
    [[nodiscard]] const game_board& board() const noexcept { return BOARD; }
};
}  // namespace pazusoba

#endif
