#include <iostream>
#include <vector>
#include <cassert>
#include "lib/board.h"
#include "lib/queue.h"

using namespace std;
using namespace pad;

void testBoard();
Board createBoard(int row, int column);
void printBoard(Board board);

void testQueue();

int main()
{
    testBoard();
    testQueue();

    return 0;
}

void testBoard()
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

void printBoard(Board board)
{
    for (auto row : board)
    {
        for (auto orb : row)
        {
            cout << orb;
        }
    }
    cout << endl;
}

void testQueue()
{
    // Test queue
    PadPriorityQueue *pq = new PadPriorityQueue(10);
    // Nothing inside so returns NULL
    assert(pq->pop() == NULL);

    State *testState = new State(999);
    State *testState2 = new State(10000);
    State *testState3 = new State(1);
    State *testState4 = new State(100);
    // Insert and pop, reduce size and get testState
    pq->insert(testState);
    assert(pq->size == 1);
    assert(pq->pop() == testState);
    assert(pq->size == 0);

    pq->insert(testState);
    assert(pq->size == 1);
    pq->insert(testState3);
    assert(pq->size == 2);
    pq->insert(testState4);
    assert(pq->size == 3);
    pq->insert(testState);
    assert(pq->size == 4);
    pq->insert(testState3);
    assert(pq->size == 5);
    pq->insert(testState4);
    assert(pq->size == 6);
    pq->insert(testState);
    assert(pq->size == 7);
    pq->insert(testState3);
    assert(pq->size == 8);
    pq->insert(testState4);
    assert(pq->size == 9);
    pq->insert(testState);
    assert(pq->size == 10);
    pq->insert(testState4);
    assert(pq->size == 10);
    pq->insert(testState);
    pq->insert(testState);
    pq->insert(testState);
    pq->insert(testState);
    pq->insert(testState);
    pq->insert(testState2);
    // Max size must be 10
    assert(pq->size == 10);
    pq->printQueue();

    // Now, size is 9
    assert(pq->pop() == testState2);
    assert(pq->size == 9);
    pq->printQueue();

    // Clean up
    delete pq;
    delete testState;
}
