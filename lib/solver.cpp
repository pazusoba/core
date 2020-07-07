/**
 * solver.cpp
 * by Yiheng Quan
 */

#include <fstream>
#include <sstream>
#include <queue>
#include <map>
#include "solver.h"

/// Constrcutors

PadSolver::PadSolver(std::string filePath, int minEraseCondition)
{
    auto currBoard = readBoard(filePath);
    board = PadBoard(currBoard, row, column, minEraseCondition);
}

/// Solve the board

std::string PadSolver::solve(int steps)
{
    std::stringstream ss;
    ss << "The board is " << row << " x " << column << ". Max step is " << steps << ".\n";
    board.printBoardForSimulation();

    std::map<int, State*> visited;
    std::priority_queue<State> toVisit;

    // Start and end should be the same for step 0
    auto start = OrbLocation(2, 0);
    // Basically, the start state is like holding the orb so start and end locations are the same
    State currentState(board, start, start, 0, steps, board.estimatedBestScore());
    toVisit.push(currentState);

    while (toVisit.size() > 0)
    {
        // Update current state
        currentState = toVisit.top();
        toVisit.pop();

        int step = currentState.step;
        int minScore = 0;
        if (step > 5) {
            minScore = step * 100;
        }

        if (currentState.score < minScore) 
            continue;

        if (currentState.step > 10) 
        {
            // DEBUG only
            int a = 0;
        }

        auto lastVisited = visited[currentState.score];
        if (lastVisited != NULL && lastVisited -> step < step)
            continue;

        auto children = currentState.getChildren();
        for (const State s : children)
        {
            if (toVisit.size() > 100)
            {
                // Must be better than the best state
                const auto topState = toVisit.top();
                if (s < topState)
                    continue;
            }
            toVisit.push(s);
        }
        visited[currentState.score] = &currentState;
    }

    ss << "Search has been completed\n";

    int counter = 0;
    while (counter < 5)
    {
        ss << toVisit.top().score << "\n";
        toVisit.pop();
        counter++;
    }

    return ss.str();
}

/// Read the board from filePath

Board PadSolver::readBoard(std::string filePath)
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
