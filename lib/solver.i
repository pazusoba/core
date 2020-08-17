%module pazusoba

%{
#include "solver.h"
#include "board.h"
#include "pad.h"
#include "profile.h"
#include "queue.h"
#include "route.h"
#include "route.h"
#include "state.h"
#include "timer.h"
%}

%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "std_set.i"
%include "std_deque.i"

class PSolver
{
    int minEraseCondition = 3;
    int steps = 25;
    int size = 1000;

    // Read board from filePath, return the board
    Board readBoard(std::string &filePath);

public:
    // Row and column are used to count board size
    int row = 0;
    int column = 0;
    // This is the original board
    PBoard board;

    PSolver() {}
    PSolver(int minEraseCondition, int maxStep, int maxSize);
    PSolver(std::string &filePath, int minEraseCondition, int maxStep, int maxSize);

    // Solve the current board
    std::vector<Route> solve();

    // Read a board from a string
    void setBoardFrom(std::string &board);

    // A random board that is row x column
    void setRandomBoard(int row, int column);

    // Update beam size, mainly for Qt
    void setBeamSize(int size);

    // Update step limit
    void setStepLimit(int step);
};
