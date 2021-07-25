/*
main_v2.cpp
The entry point of pazusoba. It handles command line arguments

Created by Yiheng Quan on 12/11/2020
*/

#include <fmt/core.h>
#include <string>

inline void showUsage() {
    fmt::print(
        "\nusage: pazusoba [board string] [min erase] [max steps] [max "
        "beam size]\nboard string\t-- "
        "eg. RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB\nmin erase\t-- 3 to 5\nmax "
        "steps\t-- maximum steps before the program stops "
        "searching\nmax beam size\t-- the width of the search space, "
        "larger number means slower speed but better results\n\nMore "
        "at "
        "https://github.com/HenryQuan/pazusoba\n\n");
    exit(0);
}

int main(int argc, char* argv[]) {
    std::string filePath = "RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB";
    int minErase = 3;
    int maxStep = 30;
    int maxSize = 5000;

    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            showUsage();
        } else {
            filePath = argv[1];
        }
    }

    if (argc > 2) {
        minErase = atoi(argv[2]);
        // min 3, max 5 for now
        if (minErase < 3)
            minErase = 3;
        if (minErase > 5)
            minErase = 5;
    }

    if (argc > 3) {
        maxStep = atoi(argv[3]);
    }

    if (argc > 4) {
        maxSize = atoi(argv[4]);
    }

    return 0;
}
