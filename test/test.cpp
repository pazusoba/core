#include <assert.h>
#include <fmt/core.h>
#include <pazusoba/core.h>

#define MILLION 1000000

void testAll();
void testTimer();
void testParser();
void testBoard();

int main() {
    pazusoba::timer timer("\n=> Test Completed");
    testAll();
}

void testAll() {
    testTimer();
    testParser();
    testBoard();
}

void testTimer() {
    fmt::print("--- Test Timer ---\n");
    // timer crashes with O2
    pazusoba::timer timer("=> Test Timer");
    int a = 0;
    int b = a + 99;
    int c = a + b + 1;
    for (int i = 0; i < 10; i++) {
        c++;
        fmt::print("c is now {}\n", c);
    }
    fmt::print("=> End timer manually\n");
    timer.end();
}

void testParser() {
    fmt::print("\n--- Test Parser ---\n");
    pazusoba::timer timer("=> Test Parser");

    fmt::print("=> one argument\n");
    // NOTE: pazusoba needs to placed because argv[0] is the program name
    const char* one[] = {"pazusoba", "RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB"};
    auto parser = pazusoba::parser(2, (char**)one);
    parser.parse();
    auto pazuboard = parser.Board();
    assert(pazuboard[29] == 2);
    assert(pazuboard(4, 5) == pazuboard[29]);
    assert(pazuboard(0, 0) == 1);
    assert(parser.Row() == 5);
    assert(parser.Column() == 6);

    fmt::print("=> two arguments\n");
    const char* two[] = {"pazusoba", "RHLBDGPRHDRJPJRHHJGR", "3"};
    parser = pazusoba::parser(3, (char**)two);
    parser.parse();
    pazuboard = parser.Board();
    assert(pazuboard[19] == 1);
    assert(pazuboard(3, 4) == pazuboard[19]);
    assert(parser.MinErase() == 3);
    assert(parser.Row() == 4);
    assert(parser.Column() == 5);

    fmt::print("=> three arguments\n");
    const char* three[] = {
        "pazusoba", "RHLBDGPRHDRJPJRHHJGRDRHLGLPHBBRHLBDGPRHDRJ", "4", "100"};
    parser = pazusoba::parser(4, (char**)three);
    parser.parse();
    pazuboard = parser.Board();
    assert(pazuboard[20] > 0);
    assert(pazuboard[41] == 7);
    fmt::print("=> modify board\n");
    pazuboard(5, 6) = 0;
    assert(pazuboard[41] == 0);
    assert(parser.Row() == 6);
    assert(parser.Column() == 7);
    assert(parser.MinErase() == 4);
    assert(parser.MaxSteps() == 100);

    fmt::print("=> four arguments\n");
    const char* four[] = {"pazusoba", "RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB", "5",
                          "200", "50000"};
    parser = pazusoba::parser(5, (char**)four);
    parser.parse();
    pazuboard = parser.Board();
    assert(parser.BeamSize() == 50000);
    assert(parser.MinErase() == 5);

    fmt::print("=> pass data, board string\n");
    parser = pazusoba::parser("RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB", 5, 200, 50000);
    parser.parse();
    pazuboard = parser.Board();
    assert(pazuboard[0] == 1);
    assert(pazuboard[5] == 3);
    assert(pazuboard[29] == 2);
    assert(parser.Row() == 5);
    assert(parser.Column() == 6);
    assert(parser.MinErase() == 5);
    assert(parser.MaxSteps() == 200);
    assert(parser.BeamSize() == 50000);

    // Assume this is running from `debug/` (need to go up one level)
    fmt::print("=> pass data, board path\n");
    parser = pazusoba::parser("../support/sample_board_65.txt", 3, 50, 10000);
    parser.parse();
    pazuboard = parser.Board();
    assert(pazuboard[0] == 1);
    assert(pazuboard[10] == 3);
    assert(pazuboard[20] == 4);
    assert(pazuboard[29] == 4);
    assert(parser.Row() == 5);
    assert(parser.Column() == 6);
}

void testBoard() {
    fmt::print("\n--- Test Board ---\n");
    pazusoba::timer timer("=> Test Board");

    auto parser =
        pazusoba::parser("RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB", 5, 200, 50000);
    parser.parse();
    auto board = parser.Board();

    fmt::print("=> print board\n");
    board.printBoard(pazusoba::PrintStyle::test);
    board.printBoard(pazusoba::PrintStyle::name);
    // very nice!
    board.printBoard(pazusoba::PrintStyle::colourful);

    auto output = board.getFormattedBoard(pazusoba::FormatStyle::dawnglare);
    fmt::print("=> test formatted board - dawnglare\n");
    assert(output == "RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB");
    fmt::print("=> test formatted board - serizawa\n");
    output = board.getFormattedBoard(pazusoba::FormatStyle::serizawa);
    assert(output == "053241705406760556104053137522");

    fmt::print("=> test board hash\n");
    // Make sure it is always the same, calculate 10 times
    assert(board.hash() == board.hash());
    assert(board.hash() == board.hash());
    assert(board.hash() == board.hash());
    assert(board.hash() == board.hash());
    assert(board.hash() == board.hash());

    fmt::print("=> test board copy\n");
    auto board2 = board.copy();
    assert(board2.hash() == board.hash());
    assert(board2.row == board.row);
    assert(board2.column == board.column);
    assert(board2.size == board.size);
    // Replace the last row with empty
    board[29] = 0;
    board[28] = 0;
    board[27] = 0;
    board[26] = 0;
    board[25] = 0;
    board[24] = 3;
    assert(board2[29] != board[29]);
    assert(board2[25] != board[25]);
    assert(board2[24] == board[24]);
    assert(board2.hash() != board.hash());
    fmt::print("=> print both board\n");
    board.printBoard(pazusoba::PrintStyle::colourful);
    board2.printBoard(pazusoba::PrintStyle::colourful);
    fmt::print("=> copy board one million times\n");
    // about 24ms (14ms in release), it is fast enough
    pazusoba::timer timerCopy("=> million copy");
    for (int i = 0; i < MILLION; i++) {
        auto board3 = board2.copy();
        board3[0] = 0;
    }
    timerCopy.end();
}