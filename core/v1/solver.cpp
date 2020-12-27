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
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "solver.h"
#include "queue.h"
#include "timer.h"
#include "profile.h"
#include "configuration.h"

// This is only for the priority queue
class PointerCompare
{
public:
    template <typename T>
    bool operator()(T *a, T *b)
    {
        return (*a) < (*b);
    }
};

// MARK: - Constrcutors

PSolver::PSolver(const std::string &filePath, int minErase, int steps, int size)
{
    this->minErase = minErase;
    this->steps = steps;
    this->size = size;

    if (filePath.find(".txt") != std::string::npos)
    {
        auto currBoard = readBoard(filePath);
        board = PBoard(currBoard);
    }
    else
    {
        setBoardFrom(filePath);
    }
}

// MARK: - Solve the board

std::vector<Route> PSolver::solve()
{
    // +
    // std::vector<Profile *> profiles{
    //     new ComboProfile,
    //     new PlusProfile({pad::fire, pad::water, pad::wood, pad::light, pad::dark}),
    //     new ColourProfile({pad::fire, pad::water, pad::wood, pad::light, pad::dark}),
    //     new ColourProfile({pad::light, pad::dark})};
    // paimon
    //    std::vector<Profile *> profiles{
    //        new ComboProfile,
    //        new PlusProfile({pad::light, pad::dark}),
    //        new ColourProfile({pad::light, pad::dark})};
    // Amen
    // std::vector<Profile *> profiles{
    //     new ComboProfile(7),
    //     new OrbProfile(2)};
    // Amen & +
    //  std::vector<Profile *>
    //      profiles{
    //          new ComboProfile(7),
    //          new OrbProfile(1),
    //          new PlusProfile({pad::fire, pad::water, pad::wood, pad::light, pad::dark}),
    //          new OneColumnProfile};
    // Two way
    // std::vector<Profile *> profiles{
    //     new ComboProfile,
    //     new TwoWayProfile({pad::fire, pad::water, pad::wood, pad::light, pad::dark})};
    // Combo
    // std::vector<Profile *> profiles{
    //     new ComboProfile,
    //     new TwoWayProfile({pad::light}),
    //     new ColourProfile};
    // Just combo
    std::vector<Profile *> profiles{
        new ComboProfile};
    // Laou
    // std::vector<Profile *> profiles{
    //     new ComboProfile,
    //     // new OneRowProfile({pad::fire}),
    //     new VoidPenProfile({pad::fire}),
    // };
    // 7x6, combo, 4 min erase
    // std::vector<Profile *> profiles{
    //     new ComboProfile,
    //     new TwoWayProfile({pad::light, pad::dark}),
    //     new ColourProfile({pad::light, pad::dark})};
    // 7x6 Plus
    // std::vector<Profile *> profiles{
    //     new ComboProfile,
    //     new PlusProfile({pad::light, pad::dark}),
    //     // new PlusProfile({pad::fire, pad::water, pad::wood, pad::light, pad::dark}),
    //     new ColourProfile({pad::light, pad::dark})};

    ProfileManager::shared().updateProfile(profiles);

    std::cout << "The board is " << column << " x " << row << ". Max step is " << steps << ".\n";
    board.printBoardForSimulation();

    // A queue that only saves top 100, 1000 based on the size
    // PPriorityQueue *toVisit = new PPriorityQueue(size);
    std::priority_queue<PState *, std::vector<PState *>, PointerCompare> toVisit;
    // This saves all chidren of states to visit
    std::vector<PState *> childrenStates;
    childrenStates.reserve(size * 4);
    // This saves the best score
    std::map<int, PState *> bestScore;
    Timer::shared().start(999);

    // This is the root state, 30 of them in a list to be deleted later
    std::vector<PState *> rootStates;
    rootStates.reserve(row * column);
    for (int i = 0; i < row; ++i)
    {
        for (int j = 0; j < column; ++j)
        {
            auto loc = OrbLocation(i, j);
            auto root = new PState(board, loc, loc, 0, steps);
            rootStates.emplace_back(root);
            toVisit.emplace(root);
        }
    }

    std::vector<std::thread> boardThreads;
    // This uses all your cores, make sure you don't make the size too large
    int processor_count = std::thread::hardware_concurrency();
    // int processor_count = 0;
    if (processor_count == 0)
    {
        processor_count = 1;
    }
    std::cout << "Using " << processor_count << " threads.\n";
    boardThreads.reserve(processor_count);
    // Cut into equal sizes
    int threadSize = size / processor_count;
    // This is important for queue and childrenStates because if you access them at the same time, the program will crash.
    // By locking and unlocking, it will make sure it is safe
    std::mutex mtx;

    srand(time(0));
    // Only take first 1000, reset for every step
    for (int i = 0; i < steps; ++i)
    {
        // int currSize = size * (100 + ((steps - i - 1) * 100 / steps)) / 200;
        // threadSize = currSize / processor_count;
        if (debug)
            Timer::shared().start(i);

        // Use multi threading
        for (int j = 0; j < processor_count; j++)
        {
            // Early break for early steps
            if (toVisit.empty())
                break;

            boardThreads.emplace_back([&] {
                for (int k = 0; k < threadSize; ++k)
                {
                    // Get the best state
                    mtx.lock();
                    if (toVisit.empty())
                    {
                        mtx.unlock();
                        break;
                    }
                    auto currentState = toVisit.top();
                    toVisit.pop();
                    mtx.unlock();

                    // Save current score for printing out later
                    int currentScore = currentState->score;
                    int currentStep = currentState->step;

                    // Save best scores
                    bool shouldAdd = false;
                    mtx.lock();
                    if (bestScore[currentScore] == nullptr)
                    {
                        bestScore[currentScore] = currentState;
                        shouldAdd = true;
                    }
                    else
                    {
                        auto saved = bestScore[currentScore];
                        shouldAdd = !(saved == currentState);

                        if (saved->step > currentStep)
                        {
                            // We found a better one
                            bestScore[currentScore] = currentState;
                        }
                    }
                    mtx.unlock();

                    if (shouldAdd && currentState != nullptr)
                    {
                        // All all possible children
                        for (auto &s : currentState->getChildren())
                        {
                            // Simply insert because states compete with each other
                            mtx.lock();
                            childrenStates.push_back(s);
                            mtx.unlock();
                        }
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
            // push randomly
            // int num = rand() % 30;
            // if (i < 10 && num < 25)
            //     toVisit.push(s);
            // else if (num < 15)
            toVisit.push(s);
        }
        childrenStates.clear();

        if (debug)
            Timer::shared().end(i);
    }

    if (debug)
        std::cout << "Search has been completed\n\n";

    // free all profiles once the search is completed
    ProfileManager::shared().clear();

    int routeSize = 3;
    if (debug)
        routeSize = 5;

    std::vector<Route> routes;
    routes.reserve(routeSize);
    // This gets routes for best 10
    int i = 0;
    PState *bestState = nullptr;
    for (auto it = bestScore.end(); it != bestScore.begin(); it--)
    {
        if (i > routeSize)
            break;
        else
            i++;

        if (i == 1)
            continue;
        // Save this state
        else if (i == 2)
            bestState = it->second;
        routes.emplace_back(it->second);
    }

    if (bestState != nullptr)
        bestState->saveToDisk();
    Timer::shared().end(999);

    // Print saved routes
    for (auto &r : routes)
    {
        r.printRoute();
    }

    // Free up memories
    for (auto const &s : rootStates)
    {
        delete s;
    }

    return routes;
}

// MARK: - Read the board from filePath or a string

Board PSolver::readBoard(const std::string &filePath)
{
    Board board;
    board.fill(pad::unknown);
    std::string lines;

    int currIndex = 0;
    std::ifstream boardFile(filePath);
    while (getline(boardFile, lines))
    {
        // Ignore lines that start with `//`
        if (lines.find("//") == 0)
            continue;

        // Remove trailing spaces by substr, +1 for substr (to include the char before space)
        int index = lines.find_last_not_of(" ") + 1;
        lines = lines.substr(0, index);

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
            board[currIndex] = Orb(a);
            currIndex++;
        }
        column++;
    }

    Configuration::shared().config(row, column, minErase);
    boardFile.close();
    return board;
}

void PSolver::setBoardFrom(const std::string &board)
{
    // This is just a string with the board
    int size = board.length();
    // It is just a string so must be fixed size
    if (size == 20) // 5x4
    {
        row = 4;
        column = 5;
    }
    else if (size == 30) // 6x5
    {
        row = 5;
        column = 6;
    }
    else if (size == 42) // 7x6
    {
        row = 6;
        column = 7;
    }

    Configuration::shared().config(row, column, minErase);

    // Read from a string
    Board currBoard;
    currBoard.fill(pad::unknown);

    for (int i = 0; i < size; i++)
    {
        char orb = board[i];

        // Check if it is a number between 1 and 9
        if (orb >= '0' && orb <= '9')
        {
            currBoard[i] = pad::orbs(orb - '0');
        }

        // Check if it is a letter (RBGLDH)
        for (int k = 0; k < pad::ORB_COUNT; k++)
        {
            if (pad::ORB_SIMULATION_NAMES[k].c_str()[0] == orb)
            {
                currBoard[i] = Orb(k);
                break;
            }
        }
    }

    this->board = PBoard(currBoard);
}

// MARK: - Setters
// Are they still used? Maybe remove later

void PSolver::setRandomBoard(int row, int column)
{
    // Set row and column
    this->row = row;
    this->column = column;

    // Update seed
    srand(time(NULL));
    Board currBoard;
    currBoard.fill(pad::unknown);
    for (int i = 0; i < row * column; i++)
    {
        currBoard[i] = pad::orbs(std::rand() % 6 + 1);
    }

    this->board = PBoard(currBoard);
}

void PSolver::setBeamSize(int size)
{
    this->size = size;
}

void PSolver::setStepLimit(int step)
{
    this->steps = step;
}
