#include <iostream>
#include <vector>
#include <cassert>
#include "core/board.h"
#include "core/queue.h"

using namespace std;
using namespace pad;

void testBoard();
Board createBoard(int row, int column);
void printBoard(Board board);

void testQueue();
void testSolver();

int main()
{
    testBoard();
    testQueue();
    testSolver();

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

    // Test if PBoard uses deep copy
    auto padBoard1 = PBoard(board2, 6, 5);
    auto padBoard2 = padBoard1;
    // board in padBoard1 will be erased
    padBoard1.rateBoard(0);
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
    for (auto const &row : board)
    {
        for (auto const &orb : row)
        {
            cout << orb;
        }
    }
    cout << endl;
}

void testQueue()
{
    // Test queue
    PPriorityQueue *pq = new PPriorityQueue(20);
    // Nothing inside so returns nullptr
    assert(pq->pop() == nullptr);

    PState *testState = new PState(999);
    PState *testState2 = new PState(10000);
    PState *testState3 = new PState(1);
    PState *testState4 = new PState(100);
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
    assert(pq->size == 11);
    pq->insert(testState);
    pq->insert(testState);
    pq->insert(testState);
    pq->insert(testState);
    pq->insert(testState2);
    pq->insert(testState);
    assert(pq->size == 17);
    pq->printQueue();

    // Now, size is 16
    assert(pq->pop() == testState2);
    assert(pq->size == 16);
    pq->printQueue();

    for (int i = 0; i < 20000; i++)
    {
        pq->insert(testState2);
    }
    pq->printQueue();

    // Clean up
    delete pq;
    delete testState, delete testState2, delete testState3, delete testState4;
}

void testSolver() {

}
