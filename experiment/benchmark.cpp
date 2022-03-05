#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include "pazusoba.h"


#define ROUND 10
const char ORB_NAME[] = {'R', 'B', 'G', 'L', 'D', 'H'};

char* random_board() {
    srand(time(0));
    char* board = new char[31];
    for (int i = 0; i < 30; i++) {
        board[i] = ORB_NAME[rand() % 6];
    }
    board[30] = '\0';
    return board;
}

int main() {
    printf("Testing %d rounds\n", ROUND);
    for (int i = 0; i < ROUND; i++) {
        char* board = random_board();
        printf("%d: %s\n", i + 1, board);
        pazusoba::solver solver;
        solver.set_board(board);
        auto state = solver.adventure();
        delete[] board;
    }
    return 0;
}