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

PadSolver::PadSolver(std::string filePath, int minEraseCondition): PadSolver(filePath) {
    this -> minEraseCondition = minEraseCondition;
}

PadSolver::~PadSolver() {
    for (auto row : board) {
        // Clean all rows inside
        row.clear();
    }
    // Clean the board
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

        // This is for storing this new row
        std::vector<pad::orbs> boardRow;
        // Keep reading until error, it will get rid of spaces automatically
        std::stringstream ss(lines);
        while (ss.good()) {
            // Only add one to row if we are in the first column, 
            // the size is fixed so there won't be a row with a different number of orbs
            if (column == 0) row++;
            // Read it out as a number
            int a = 0; ss >> a;

            // Convert int into orbs
            boardRow.push_back(pad::orbs(a));
        }

        // Add this row to the board
        board.push_back(boardRow);

        column++;
    }

    boardFile.close();
}

void PadSolver::solveBoard() {

}

void PadSolver::updateBoard() {
    // row -1 because there is no need to update the bottom row
    for (int i = 0 ; i < row - 1; i++) {
        for (int j = 0; j < column; j++) {
            auto current = board[i][j];
            if (pad::empty == board[i + 1][j]) {
                board[i + 1][j] = current;
                board[i][j] = pad::empty;
            }
        }
    }
}

void PadSolver::printBoard() {
    if (isEmptyFile()) {
        std::cout << "- empty -\n";
        return;
    }

    // Print everything out nicely
    std::cout << row << " x " << column << std::endl;
    for (auto row : board) {
        for (auto orb : row) {
            std::cout << pad::ORB_NAMES[orb] << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void PadSolver::printBoardInfo() {
    if (isEmptyFile()) {
        std::cout << "no info\n";
        return;
    }

    // Collect orb info
    int *counter = collectOrbCount();

    // Print out some board info
    for (int i = 0; i < pad::ORB_COUNT; i++) {
        int count = counter[i];
        if (count == 0) continue;
        // It is just like fire x 5 wood x 6
        std::cout << count << " x " << pad::ORB_NAMES[i];
        if (i != pad::ORB_COUNT - 1) std::cout << " | ";
    }
    std::cout << std::endl;

    std::cout << "Board max combo: " << getBoardMaxCombo() << std::endl;
    std::cout << "Current max combo: " << getMaxCombo(counter) << std::endl;

    delete[] counter;
}

int PadSolver::getMaxCombo(int *counter) {
    if (isEmptyFile()) return 0;

    int comboCounter = 0;

    int moreComboCount;
    do {
        // Reset this flag everytime
        moreComboCount = 0;
        // This tracks how many orbs are left
        int orbLeft = 0;
        int maxOrbCounter = 0;

        for (int i = 0 ; i < pad::ORB_COUNT; i++) {
            // Keep -3 or other minEraseCondition (4, 5) until all orbs are less than 2
            int curr = counter[i];
            if (curr >= minEraseCondition) {
                moreComboCount += 1;
                comboCounter++;
                counter[i] -= minEraseCondition;
                if (curr > maxOrbCounter) maxOrbCounter = curr;
            } else {
                orbLeft += curr;
            }
        }

        // Only one colour can do combo now
        if (moreComboCount == 1) {
            // Orbs left are in different colour but they can still seperate other colours
            int maxComboPossible = orbLeft / minEraseCondition;
            int maxCombo = maxOrbCounter / minEraseCondition;
            // Make sure there are enough orbs
            comboCounter += maxCombo > maxComboPossible ? maxComboPossible : maxCombo;

            // No orbs left means one colour, -1 here because we count one more but doesn't apply to a single colour board
            if (orbLeft > 0) comboCounter--;
            break;
        }
    } while (moreComboCount > 0);

    return comboCounter;
}

/// Utils

int PadSolver::getBoardMaxCombo() {
    return row * column / minEraseCondition;
}

bool PadSolver::isEmptyFile() {
    return column == 0 && row == 0;
}

int* PadSolver::collectOrbCount() {
    int *counter = new int[pad::ORB_COUNT] {0};
    for (auto row : board) {
        for (auto orb : row) {
            counter[orb]++;
        }
    }
    return counter;
}
