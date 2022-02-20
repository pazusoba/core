// pazusoba.cpp
// Try to improve the performance of the solver while being flexible enough

// Compile with
// g++ pazusoba.cpp -std=c++11 -O2 -o pazusoba

#include <array>
#include <iostream>

#define MAX_DEPTH 150
#define MIN_BEAM_SIZE 1000
#define MAX_BOARD_LENGTH 42
#define ORB_COUNT 14

/// Match names https://pad.dawnglare.com/ use (not all orbs are supported)
const char ORB_WEB_NAME[ORB_COUNT] = {' ', 'R', 'B', 'G', 'L', 'D', 'H',
                                      'J', ' ', 'P', ' ', ' ', ' ', ' '};

typedef unsigned char orb;

// global variables, they shouldn't be changed once set
int MIN_ERASE = 3;
int SEARCH_DEPTH = 100;
int BEAM_SIZE = 5000;
int ROW, COLUMN;
int MAX_COMBO;
int BOARD_SIZE;
std::array<orb, MAX_BOARD_LENGTH> BOARD;
// count the number of each orb to calculate the max combo (not 100% correct)
std::array<orb, ORB_COUNT> ORB_COUNTER;

void print_board(const std::array<orb, MAX_BOARD_LENGTH>&);
// A naive way to approach max combo, mostly accurate unless it is two colour
int calc_max_combo(const std::array<orb, ORB_COUNT>&, int, int);
void parse_args(int argc, char* argv[]);
void usage();

int main(int argc, char* argv[]) {
    parse_args(argc, argv);
    return 0;
}

void print_board(const std::array<orb, MAX_BOARD_LENGTH>& board) {
    printf("board: ");
    for (int i = 0; i < MAX_BOARD_LENGTH; i++) {
        printf("%c", ORB_WEB_NAME[board[i]]);
    }
    printf("\n");
}

int calc_max_combo(const std::array<orb, ORB_COUNT>& counter,
                   int size,
                   int min_erase) {
    // at least one combo when the board has only one orb
    int max_combo = 0;
    int threshold = size / 2;
    for (const auto& count : counter) {
        int combo = count / min_erase;
        // based on my experience, it is not possible to do more combo
        // if one colour has more than half the board
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
            printf("min_erase is too small, set to 3\n");
        } else if (min_erase > 5) {
            min_erase = 5;
            printf("min_erase is too large, set to 5\n");
        }
        MIN_ERASE = min_erase;
    }

    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            usage();
        } else {
            printf("=============== INFO ===============\n");
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
    printf("board size: %d\n", BOARD_SIZE);
    printf("row x column: %d x %d\n", ROW, COLUMN);
    printf("min_erase: %d\n", MIN_ERASE);
    printf("max_combo: %d\n", MAX_COMBO);
    printf("search_depth: %d\n", SEARCH_DEPTH);
    printf("beam_size: %d\n", BEAM_SIZE);
    printf("====================================\n");
}

void usage() {
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
