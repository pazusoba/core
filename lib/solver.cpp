/**
 * solver.cpp
 * by Yiheng Quan
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include "solver.h"

PadSolver::PadSolver(std::string filePath) {
    readBoard(filePath);
}

PadSolver::~PadSolver() {
    // Just clear the board
    board.clear();
}

void PadSolver::readBoard(std::string filePath) {
    std::string lines;

    std::ifstream boardFile(filePath);
    while (getline(boardFile, lines)) {
        // Ignore lines that start with `//`
        if (lines.find("//") == 0) continue;

        // Remove trailing spaces by substr, +1 for substr (to include the char before space)
        int index = lines.find_last_not_of(" ") + 1;
        lines = lines.substr(0, index);

        // Keep reading until error, it will get rid of spaces automatically
        std::stringstream ss(lines);
        while (ss.good()) {
            // Only add one to row if we are in the first column, 
            // the size is fixed so there won't be a row with a different number of orbs
            if (column == 0) row++;
            // Read it out as a number
            int a = 0; ss >> a;

            // Convert int into orbs
            board.push_back(pad::orbs(a));
        }

        column++;
    }
    boardFile.close();
}

void PadSolver::printBoard() {
    if (isEmptyFile()) {
        std::cout << "- empty -\n";
        return;
    }

    // Print everything out nicely
    std::cout << row << " x " << column << std::endl;
    int counter = 0;
    for (int orb : board) {
        counter++;
        std::cout << pad::ORB_NAMES[orb] << "\t";
        if (counter % row == 0) std::cout << std::endl;
    }
}

void PadSolver::printBoardInfo() {
    if (isEmptyFile()) {
        std::cout << "no info\n";
        return;
    }

    // Collect orb info
    int *counter = new int[pad::ORB_COUNT] {0};
    for (auto orb : board) counter[orb]++;

    // Print out some board info
    for (int i = 0; i < pad::ORB_COUNT; i++) {
        // It is just like fire x 5 wood x 6
        std::cout << counter[i] << " x " << pad::ORB_NAMES[i];
        if (i != pad::ORB_COUNT - 1) std::cout << " || ";
    }
    std::cout << std::endl;

    delete[] counter;
}

int PadSolver::getMaxCombo() {
    if (isEmptyFile()) return 0;

    int combo = 0;

    int *counter = new int[pad::ORB_COUNT] {0};
    for (auto orb : board) {
        counter[orb]++;
        // TODO: this is a naive way of getting max combo. If the entire board only has one type, it only has one combo
        // You can make one combo with 3 orbs
        if (counter[orb] == 3) {
            combo++;
            // Reset current counter
            counter[orb] = 0;
        }
    }
    delete[] counter;

    return combo;
}

bool PadSolver::isEmptyFile() {
    return column == 0 && row == 0;
}
