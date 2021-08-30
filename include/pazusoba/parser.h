//
// parser.h
// Parse arguments and options.
// This is an interface for solver.
//
// Created by Yiheng Quan on 25/07/2021
//

#ifndef _PARSER_H_
#define _PARSER_H_

#include <string>
#include "board.h"

namespace pazusoba {
class Parser {
    pint _min_erase = 3;
    pint _max_steps = 50;
    pint _beam_size = 5000;
    std::string _board_string;
    Board _board;

    void showUsage();

    /// Read the text from the path and convert it to a board.
    /// This is mainly used for TEST, sample_board_65.txt
    void readBoardFrom(const std::string&);
    /// Set the board from a board string, RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB
    void setBoardFrom(const std::string&);

public:
    Parser(int argc, char* argv[]);
    Parser(const std::string&, pint, pint, pint);
    void parse();

    // Getters
    const pint& row() const { return _board.row(); }
    const pint& column() const { return _board.column(); }
    const pint& minErase() const { return _min_erase; }
    const pint& maxSteps() const { return _max_steps; }
    const pint& beamSize() const { return _beam_size; }
    const Board& board() const { return _board; }
};
}  // namespace pazusoba

#endif
