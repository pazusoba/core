#include <iostream>
#include <vector>
#include <cassert>

using namespace std;
typedef vector<int> IntVector;
typedef vector<IntVector> TestBoard;

TestBoard createBoard(int row, int column);
void printBoard(TestBoard board);

int main()
{
    // Test if vector uses deep copy
    auto board1 = createBoard(6, 5);
    auto board2 = board1;
    assert(board1 == board2);
    board2[1][0] = 1; // update board 2
    assert(board1 != board2);
    printBoard(board1);
    printBoard(board2);

    return 0;
}

TestBoard createBoard(int r, int col)
{
    TestBoard board;
    for (int i = 0; i < col; i++)
    {
        IntVector row;
        for (int j = 0; j < r; j++)
        {
            row.push_back(0);
        }
        board.push_back(row);
    }
    return board;
}

void printBoard(TestBoard board) {
    for (auto row : board) {
        for (auto e : row) {
            cout << e;
        }
    }
    cout << endl;
}