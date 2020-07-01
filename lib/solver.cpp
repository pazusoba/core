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

void PadSolver::readBoard(std::string filePath) {
    std::string lines;

    std::ifstream boardFile(filePath);
    while (getline(boardFile, lines)) {
        // Ignore lines that start with `//`
        if (lines.find("//") == 0) continue;

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
    std::cout << column << " x " << row << std::endl;
    int counter = 0;
    for (auto orb : board) {
        counter++;
        std::cout << pad::ORB_NAMES[orb] << "\t";
        if (counter % row == 0) std::cout << std::endl;
    }

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
