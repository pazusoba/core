/**
 * solver.cpp
 * by Yiheng Quan
 */

#include <fstream>
#include <sstream>
#include <map>
#include "solver.hpp"
#include "queue.hpp"

/// Constrcutors

PSolver::PSolver(std::string filePath, int minEraseCondition, int steps, int size)
{
    this->size = size;
    this->steps = steps;
    auto currBoard = readBoard(filePath);
    board = PBoard(currBoard, row, column, minEraseCondition);
}

/// Solve the board

std::string PSolver::solve()
{
    std::stringstream ss;
    ss << "The board is " << row << " x " << column << ". Max step is " << steps << ".\n";
    board.printBoardForSimulation();

    PPriorityQueue *toVisit = new PPriorityQueue(size);
    std::map<int, PState *> bestScore;

    // Start and end should be the same for step 0
    auto start = OrbLocation(2, 0);
    // Basically, the start state is like holding the orb so start and end locations are the same
    auto root = new PState(board, start, start, 0, steps, board.estimatedBestScore());
    toVisit->insert(root);

    while (toVisit->size > 0)
    {
        // Get the best state
        auto currentState = toVisit->pop();

        // Save current score for printing out later
        int currentScore = currentState->score;
        if (bestScore[currentScore] == NULL)
            bestScore[currentScore] = currentState;

        // All all possible children
        auto children = currentState->getChildren();
        for (auto s : children)
        {
            // Simply insert because states compete with each other
            toVisit->insert(s);
        }
    }

    ss << "Search has been completed\n";

    // This prints top ten
    int i = 0;
    for (auto it = bestScore.end(); it != bestScore.begin(); it--)
    {
        if (i > 10) 
            break;
        else 
            i++;
        if (i == 1) 
            continue;

        auto score = *it;
        score.second->printState();
    }

    // Free up memories
    delete toVisit;
    delete root;

    return ss.str();
}

/// Read the board from filePath

Board PSolver::readBoard(std::string filePath)
{
    Board board;
    std::string lines;

    std::ifstream boardFile(filePath);
    while (getline(boardFile, lines))
    {
        // Ignore lines that start with `//`
        if (lines.find("//") == 0)
            continue;

        // Remove trailing spaces by substr, +1 for substr (to include the char before space)
        int index = lines.find_last_not_of(" ") + 1;
        lines = lines.substr(0, index);

        // This is for storing this new row
        Row boardRow;
        // Keep reading until error, it will get rid of spaces automatically
        std::stringstream ss(lines);
        while (ss.good())
        {
            // Only add one to row if we are in the first column,
            // the size is fixed so there won't be a row with a different number of orbs
            if (column == 0)
                row++;
            // Read it out as a number
            int a = 0;
            ss >> a;

            // Convert int into orbs
            boardRow.push_back(Orb(a));
        }

        // Add this row to the board
        board.push_back(boardRow);

        column++;
    }

    boardFile.close();
    return board;
}
