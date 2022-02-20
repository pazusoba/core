// pazusoba.cpp
// Try to improve the performance of the solver while being flexible enough

// Compile with
// g++ pazusoba.cpp -std=c++11 -O2 -o pazusoba

#include <iostream>

#define MAX_DEPTH 150
#define MIN_BEAM_SIZE 1000

// global variables, they shouldn't be changed once set
int MIN_ERASE = 3;
int SEARCH_DEPTH = 100;
int BEAM_SIZE = 5000;

void usage();
void parse_args(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    parse_args(argc, argv);
    return 0;
}

void parse_args(int argc, char* argv[]) {
    if (argc <= 1)
        usage();

    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            usage();
        } else {
            // _board_string = argv[1];
        }
    }

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

    printf("min_erase: %d\n", MIN_ERASE);
    printf("search_depth: %d\n", SEARCH_DEPTH);
    printf("beam_size: %d\n", BEAM_SIZE);
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