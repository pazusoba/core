//
// parser.h
// Parse arguments and options
//
// Created by Yiheng Quan on 25/07/2021
//

#ifndef _PARSER_H_
#define _PARSER_H_

#include <fmt/core.h>
#include <string>
#include "board.h"

namespace pazusoba {
class parser {
    int row;
    int column;
    int minErase = 3;
    int maxSteps = 30;
    int beamSize = 5000;
    std::string boardString;
    board currentBoard;

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

    /// Read the text from the path and convert it to a board,
    /// sample_board_65.txt
    void readBoardFrom(const std::string&);
    /// Set the board from a board string, RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB
    void setBoardFrom(const std::string&);

public:
    parser(int argc, char* argv[]);
    parser(const std::string& board, int minErase, int maxSteps, int beamSize);
    void parse();

    // Getters
    const int& Row() const { return row; }
    const int& Column() const { return column; }
    const int& MinErase() const { return minErase; }
    const int& MaxSteps() const { return maxSteps; }
    const int& BeamSize() const { return beamSize; }
    const board& Board() const { return currentBoard; }
};
}  // namespace pazusoba

#endif
