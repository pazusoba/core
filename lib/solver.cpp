/**
 * solver.cpp
 * by Yiheng Quan
 */

#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <queue>
#include <thread>
#include <mutex>
#include "solver.hpp"
#include "queue.hpp"
#include "timer.hpp"

class PointerCompare
{
public:
    template <typename T>
    bool operator()(T *a, T *b)
    {
        return (*a) < (*b);
    }
};

/// Constrcutors

PSolver::PSolver(int minEraseCondition, int maxStep, int maxSize)
{
    this->minEraseCondition = minEraseCondition;
    this->steps = maxStep;
    this->size = maxSize;
}

PSolver::PSolver(std::string filePath, int minEraseCondition, int steps, int size) : PSolver(minEraseCondition, steps, size)
{
    if (filePath.find(".txt") != std::string::npos)
    {
        auto currBoard = readBoard(filePath);
        board = PBoard(currBoard, row, column, minEraseCondition);
    }
    else
    {
        // This is a board
        auto board = filePath;
        // This is just a string with the board
        int size = board.length();
        // It is just a string so must be fixed size
        if (size == 20) // 5x4
        {
            row = 5;
            column = 4;
        }
        else if (size == 30) // 6x5
        {
            row = 6;
            column = 5;
        }
        else if (size == 42) // 7x6
        {
            row = 7;
            column = 6;
        }

        // Read from a string
        Board currBoard;
        for (int i = 0; i < column; i++)
        {
            Row r;
            for (int j = 0; j < row; j++)
            {
                int index = j + i * row;
                char orb = board[index];
                for (int k = 0; k < pad::ORB_COUNT; k++)
                {
                    if (pad::ORB_SIMULATION_NAMES[k].c_str()[0] == orb)
                    {
                        r.push_back(Orb(k));
                        break;
                    }
                }
            }
            currBoard.push_back(r);
        }

        this->board = PBoard(currBoard, row, column, minEraseCondition);
    }
}

/// Solve the board

void PSolver::solveThread()
{
}

void PSolver::solve()
{
    timer::shared().start(9999);
    std::cout << "The board is " << row << " x " << column << ". Max step is " << steps << ".\n";
    board.printBoardForSimulation();

    // A queue that only saves top 100, 1000 based on the size
    // PPriorityQueue *toVisit = new PPriorityQueue(size);
    std::priority_queue<PState *, std::vector<PState *>, PointerCompare> toVisit;
    // This saves all chidren of states to visit
    std::vector<PState *> childrenStates;
    childrenStates.reserve(size * 3);
    // This saves the best score
    std::map<int, PState *> bestScore;

    // This is the root state, 30 of them in a list to be deleted later
    std::vector<PState *> rootStates;
    rootStates.reserve(row * column);
    for (int i = 0; i < column; ++i)
    {
        for (int j = 0; j < row; ++j)
        {
            auto loc = OrbLocation(i, j);
            auto root = new PState(board, loc, loc, 0, steps);
            rootStates.emplace_back(root);
            toVisit.emplace(root);
        }
    }

    std::vector<std::thread> boardThreads;
    // Don't use all cores, it may freeze your computer
    int processor_count = std::thread::hardware_concurrency();
    if (processor_count == 0)
    {
        processor_count = 1;
    }
    boardThreads.reserve(processor_count);
    int threadSize = size / processor_count;
    // This is important for queue and childrenStates because if you access them at the same time, the program will crash.
    // By locking and unlocking, it will make sure it is safe
    std::mutex mtx;

    // Only take first 1000, reset for every step
    for (int i = 0; i < steps; ++i)
    {
        timer::shared().start(i);
        // Use multi threading
        for (int j = 0; j < processor_count; j++)
        {
            boardThreads.emplace_back([&] {
                for (int k = 0; k < threadSize; ++k)
                {
                    // Early steps might not have enough size
                    if (toVisit.empty())
                        return;

                    mtx.lock();
                    // Get the best state
                    auto currentState = toVisit.top();
                    toVisit.pop();
                    mtx.unlock();

                    // Save current score for printing out later
                    int currentScore = currentState->score;
                    int currentStep = currentState->step;

                    // Save best scores
                    if (bestScore[currentScore] == nullptr)
                    {
                        bestScore[currentScore] = currentState;
                    }
                    else
                    {
                        auto saved = bestScore[currentScore];
                        if (saved->step > currentStep)
                        {
                            // We found a better one
                            bestScore[currentScore] = currentState;
                        }
                    }

                    // All all possible children
                    for (auto &s : currentState->getChildren())
                    {
                        mtx.lock();
                        // Simply insert because states compete with each other
                        childrenStates.emplace_back(s);
                        mtx.unlock();
                    }
                }
            });
        }
        
        // Make sure all threads are completed
        for (auto &t : boardThreads)
        {
            t.join();
        }
        // Clear for next round
        boardThreads.clear();

        toVisit = std::priority_queue<PState *, std::vector<PState *>, PointerCompare>();
        for (const auto &s : childrenStates)
        {
            toVisit.push(s);
        }
        childrenStates.clear();
        timer::shared().end(i);
    }
    timer::shared().end(9999);

    std::cout << "Search has been completed\n\n";
    // This prints top 5
    int i = 0;
    for (auto it = bestScore.end(); it != bestScore.begin(); it--)
    {
        if (i > 6)
            break;
        else
            i++;
        if (i == 1)
            continue;

        auto score = *it;
        score.second->printState();
    }

    // Free up memories
    for (auto const &s : rootStates)
    {
        delete s;
    }
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
