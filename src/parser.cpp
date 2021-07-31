#include <pazusoba/constant.h>
#include <pazusoba/parser.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace pazusoba {
parser::parser(int argc, char* argv[]) {
    // Nothing passed in
    if (argc <= 1) {
        showUsage();
    }

    // parse command line arguments
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            showUsage();
        } else {
            this->boardString = argv[1];
        }
    }

    if (argc > 2) {
        int minErase = atoi(argv[2]);
        // min 3, max 5 for now
        if (minErase < 3)
            minErase = 3;
        if (minErase > 5)
            minErase = 5;
        this->minErase = minErase;
    }

    if (argc > 3) {
        this->maxSteps = atoi(argv[3]);
    }

    if (argc > 4) {
        this->beamSize = atoi(argv[4]);
    }
}

parser::parser(const std::string& board,
               int minErase,
               int maxSteps,
               int beamSize) {
    this->boardString = board;
    this->minErase = minErase;
    this->maxSteps = maxSteps;
    this->beamSize = beamSize;
}

void parser::parse() {
    // the board can be the actually board or the path to a local file
    if (this->boardString.find(".txt") != std::string::npos) {
        readBoardFrom(this->boardString);
    } else {
        setBoardFrom(this->boardString);
    }
}

void parser::readBoardFrom(const std::string& path) {
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
            this->currentBoard[currIndex] = orb(a);
            currIndex++;
        }
        row++;
    }

    boardFile.close();
    this->currentBoard.row = row;
    this->currentBoard.column = column;
    this->currentBoard.size = row * column;
}

void parser::setBoardFrom(const std::string& boardString) {
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
            "parser::setBoardFrom - boardString has a invalid size");
    }

    // TODO: move this to a function instead
    this->currentBoard.size = size;
    this->currentBoard.row = row;
    this->currentBoard.column = column;

    for (int i = 0; i < size; i++) {
        char current = boardString[i];

        // Check if it is a number between 1 and 9
        if (current >= '0' && current <= '9') {
            this->currentBoard[i] = orb(current - '0');
        }

        // Check if it is a letter (RBGLDH)
        for (int j = 0; j < constant::ORB_COUNT; j++) {
            if (current == constant::ORB_WEB_NAME[j][0]) {
                this->currentBoard[i] = orb(j);
                break;
            }
        }
    }
}
}  // namespace pazusoba
