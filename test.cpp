#include <iostream>
#include <vector>
#include <cassert>
#include "lib/board.h"

using namespace std;
using namespace pad;

Board createBoard(int row, int column);
void printBoard(Board board);

int main()
{
    // Test if vector uses deep copy
    auto board1 = createBoard(6, 5);
    auto board2 = board1;
    assert(board1 == board2);
    board2[1][0] = orbs(1); // update board 2
    assert(board1 != board2);
    printBoard(board1);
    printBoard(board2);

    // Test if PadBoard uses deep copy
    auto padBoard1 = PadBoard(board2, 6, 5);
    auto padBoard2 = padBoard1;
    // board in padBoard1 will be erased
    padBoard1.rateBoard();
    assert(padBoard1.getBoardID() != padBoard2.getBoardID());

    return 0;
}

Board createBoard(int r, int col)
{
    Board board;
    for (int i = 0; i < col; i++)
    {
        Row row;
        for (int j = 0; j < r; j++)
        {
            row.push_back(orbs(j));
        }
        board.push_back(row);
    }
    return board;
}

void printBoard(Board board) {
    for (auto row : board) {
        for (auto orb : row) {
            cout << orb;
        }
    }
    cout << endl;
}