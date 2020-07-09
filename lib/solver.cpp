/**
 * solver.cpp
 * by Yiheng Quan
 */

#include <fstream>
#include <sstream>
#include <map>
#include "solver.h"
#include "queue.h"

/// Constrcutors

PadSolver::PadSolver(std::string filePath, int minEraseCondition, int steps, int size)
{
    this->size = size;
    this->steps = steps;
    auto currBoard = readBoard(filePath);
    board = PadBoard(currBoard, row, column, minEraseCondition);
}

/// Solve the board

std::string PadSolver::solve()
{
    std::stringstream ss;
    ss << "The board is " << row << " x " << column << ". Max step is " << steps << ".\n";
    board.printBoardForSimulation();

    std::map<std::string, int> visited;
    PadPriorityQueue *toVisit = new PadPriorityQueue(size);
    std::vector<int> bestScorePerStep(steps + 2);
    std::map<int, State *> bestScore;

    // Start and end should be the same for step 0
    auto start = OrbLocation(3, 2);
    // Basically, the start state is like holding the orb so start and end locations are the same
    auto rooState = new State(board, start, start, 0, steps, board.estimatedBestScore());
    toVisit->insert(rooState);

    int counter = 0;
    while (toVisit->size > 0)
    {
        // if (counter > 50000)
        //     break;

        // Update current state
        auto currentState = toVisit->pop();

        int currentScore = currentState->score;
        // int currStep = currentState -> step;

        // // Not good enough
        // if (bestScorePerStep[currStep] < currentScore)
        //     bestScorePerStep[currStep] = currentScore;
        // else if (bestScorePerStep[currStep] / currentScore > 4)
        //     continue;

        if (bestScore[currentScore] == NULL)
            bestScore[currentScore] = currentState;

        // if (currentState->step > 10)
        // {
        //     // DEBUG only
        //     int a = 0;
        // }

        // Prevent duplcate boards
        // auto boardID = currentState->boardID;
        // int lastStepCount = visited[boardID];
        // if (lastStepCount > 0 && currStep > lastStepCount)
        //     continue;
        // else
        //     visited[boardID] = currStep;

        counter++;
        auto children = currentState->getChildren();
        for (auto s : children)
        {
            // The number here will affect the result and also speed
            // if (toVisit.size() > 100)
            // {
            //     // Must be better than the best state
            //     auto topState = toVisit.top();
            //     if (*s < *topState)
            //         continue;
            // } else {
            //     toVisit.push(s);
            //     continue;
            // }

            // auto avg = currentState->averageScore;
            toVisit->insert(s);
            // It means that children is not as good as the parent
            // if (avg == 0 || currentState->score >= avg)
            // {
            // }
            // else
            // {
            // }
        }
    }

    ss << "Search has been completed\n";

    int i = 0;
    for (auto it = bestScore.end(); it != bestScore.begin(); it--)
    {
        if (i > 3)
            break;
        else
            i++;

        if (i == 1)
            continue;
        auto score = *it;
        score.second->printState();
    }

    delete toVisit;
    delete rooState;
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
