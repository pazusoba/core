#include <fmt/core.h>
#include <pazusoba/constant.h>
#include <pazusoba/parser.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace pazusoba {
Parser::Parser(int argc, char* argv[]) {
    // Nothing passed in
    if (argc <= 1) {
        showUsage();
    }

    // parse command line arguments
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            showUsage();
        } else {
            _board_string = argv[1];
        }
    }

    if (argc > 2) {
        int minErase = atoi(argv[2]);
        // min 3, max 5 for now
        if (minErase < 3)
            minErase = 3;
        if (minErase > 5)
            minErase = 5;
        _min_erase = minErase;
    }

    if (argc > 3) {
        _max_steps = atoi(argv[3]);
    }

    if (argc > 4) {
        this->_beam_size = atoi(argv[4]);
    }
}

Parser::Parser(const std::string& boardString,
               pint minErase,
               pint maxSteps,
               pint beamSize) {
    _board_string = boardString;
    _min_erase = minErase;
    _max_steps = maxSteps;
    this->_beam_size = beamSize;
}

void Parser::showUsage() {
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

void Parser::parse() {
    // the board can be the actually board or the path to a local file
    if (_board_string.find(".txt") != std::string::npos) {
        readBoardFrom(_board_string);
    } else {
        setBoardFrom(_board_string);
    }

    if (DEBUG_MODE) {
        _board.printBoard(colourful);
    }
}

void Parser::readBoardFrom(const std::string& path) {
    std::string lines;

    int currIndex = 0;
    int row = 0;
    int column = 0;
    std::ifstream boardFile(path);
    while (getline(boardFile, lines)) {
        // Ignore lines that start with `//`
        if (lines.find("//") == 0)
            continue;

        // Remove trailing spaces by substr, +1 for substr
        // (to include the char before space)
        int index = lines.find_last_not_of(" ") + 1;
        lines = lines.substr(0, index);

        // Keep reading until error, it will get rid of spaces automatically
        std::stringstream ss(lines);
        while (ss.good()) {
            // Only add one to row if we are in the first column,
            // the size is fixed so there won't be a row with a different number
            // of orbs
            if (row == 0)
                column++;
            // Read it out as a number
            int a = 0;
            ss >> a;

            // Convert int into orbs
            _board[currIndex] = orb(a);
            currIndex++;
        }
        row++;
    }

    boardFile.close();
    _board.set(row, column, _min_erase);
}

void Parser::setBoardFrom(const std::string& boardString) {
    int size = boardString.length();
    int row = 0;
    int column = 0;

    // only support 3 fixed size for now, more can be added later
    if (size == 20) {
        row = 4;
        column = 5;
    } else if (size == 30) {
        row = 5;
        column = 6;
    } else if (size == 42) {
        row = 6;
        column = 7;
    } else {
        fmt::print("Unsupported board size - {}\n", size);
        throw std::logic_error(
            "Parser::setBoardFrom - boardString has a invalid size");
    }

    _board.set(row, column, _min_erase);

    for (int i = 0; i < size; i++) {
        char current = boardString[i];

        // Check if it is a number between 1 and 9
        if (current >= '0' && current <= '9') {
            _board[i] = orb(current - '0');
        }

        // Check if it is a letter (RBGLDH)
        for (int j = 0; j < pad::ORB_COUNT; j++) {
            if (current == pad::ORB_WEB_NAME[j][0]) {
                _board[i] = orb(j);
                break;
            }
        }
    }
}
}  // namespace pazusoba
