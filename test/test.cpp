#include <assert.h>
#include <fmt/core.h>
#include <pazusoba/core.h>
#include <future>

#define LOOPS 1000000
typedef unsigned long long int int64;

void testAll();
void testTimer();
void testParser();
void testBoard();
void testState();
void testBoardErase();
void testSearch();
void testQueue();

int main() {
    pazusoba::Timer timer("\n=> Test Completed");
    testAll();
    fmt::print("\n--- Test All ---");

    // TODO: fix the timeout assert
    // auto timeout = std::async(std::launch::async, [] {
    //     fmt::print("\n--- Timeout ---");
    //     assert(false);
    // });
    // timeout.wait_for(std::chrono::seconds(10));
}

void testAll() {
    testTimer();
    testParser();
    testBoard();
    testState();
    testBoardErase();
    testSearch();
    testQueue();
}

void testTimer() {
    fmt::print("--- Test Timer ---\n");
    // timer crashes with O2
    pazusoba::Timer timer("=> Test Timer");
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
    pazusoba::Timer timer("=> Test Parser");

    fmt::print("=> one argument\n");
    // NOTE: pazusoba needs to placed because argv[0] is the program name
    const char* one[] = {"pazusoba", "RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB"};
    auto parser = pazusoba::Parser(2, (char**)one);
    parser.parse();
    auto pazuboard = parser.board();
    assert(pazuboard[29] == 2);
    assert(pazuboard(4, 5) == pazuboard[29]);
    assert(pazuboard(0, 0) == 1);
    assert(parser.row() == 5);
    assert(parser.column() == 6);

    fmt::print("=> two arguments\n");
    const char* two[] = {"pazusoba", "RHLBDGPRHDRJPJRHHJGR", "3"};
    parser = pazusoba::Parser(3, (char**)two);
    parser.parse();
    pazuboard = parser.board();
    assert(pazuboard[19] == 1);
    assert(pazuboard(3, 4) == pazuboard[19]);
    assert(parser.minErase() == 3);
    assert(parser.row() == 4);
    assert(parser.column() == 5);

    fmt::print("=> three arguments\n");
    const char* three[] = {
        "pazusoba", "RHLBDGPRHDRJPJRHHJGRDRHLGLPHBBRHLBDGPRHDRJ", "4", "100"};
    parser = pazusoba::Parser(4, (char**)three);
    parser.parse();
    pazuboard = parser.board();
    assert(pazuboard[20] > 0);
    assert(pazuboard[41] == 7);
    fmt::print("=> modify board\n");
    pazuboard(5, 6) = 0;
    assert(pazuboard[41] == 0);
    assert(parser.row() == 6);
    assert(parser.column() == 7);
    assert(parser.minErase() == 4);
    assert(parser.maxSteps() == 100);

    fmt::print("=> four arguments\n");
    const char* four[] = {"pazusoba", "RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB", "5",
                          "200", "50000"};
    parser = pazusoba::Parser(5, (char**)four);
    parser.parse();
    pazuboard = parser.board();
    assert(parser.beamSize() == 50000);
    assert(parser.minErase() == 5);

    fmt::print("=> pass data, board string\n");
    parser = pazusoba::Parser("RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB", 5, 200, 50000);
    parser.parse();
    pazuboard = parser.board();
    assert(pazuboard[0] == 1);
    assert(pazuboard[5] == 3);
    assert(pazuboard[29] == 2);
    assert(parser.row() == 5);
    assert(parser.column() == 6);
    assert(parser.minErase() == 5);
    assert(parser.maxSteps() == 200);
    assert(parser.beamSize() == 50000);

    // Assume this is running from `debug/` (need to go up one level)
    fmt::print("=> pass data, board path\n");
    parser = pazusoba::Parser("../support/sample_board_65.txt", 3, 50, 10000);
    parser.parse();
    pazuboard = parser.board();
    assert(pazuboard[0] == 1);
    assert(pazuboard[10] == 3);
    assert(pazuboard[20] == 4);
    assert(pazuboard[29] == 4);
    assert(parser.row() == 5);
    assert(parser.column() == 6);
}

void testBoard() {
    fmt::print("\n--- Test Board ---\n");
    pazusoba::Timer timer("=> Test Board");

    auto parser =
        pazusoba::Parser("RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB", 5, 200, 50000);
    parser.parse();
    auto board = parser.board();

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
    assert(board2.row() == board.row());
    assert(board2.column() == board.column());
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
    // about 26ms (14ms in release), it is fast enough
    pazusoba::Timer timerCopy("=> copy multiple times");
    for (int64 i = 0; i < LOOPS; i++) {
        auto board3 = board2.copy();
        board3[0] = 0;
    }
    timerCopy.end();

    fmt::print("=> test orb swap\n");
    board.swap(0, 1);
    assert(board[0] == 6);
    assert(board[1] == 1);
    board.printBoard(pazusoba::PrintStyle::colourful);

    board.swap(0, 0, 0, 1);
    assert(board[0] == 1);
    assert(board[1] == 6);
    board.printBoard(pazusoba::PrintStyle::colourful);

    // invalid swaps
    board.swap(100, 200);
    board.swap(29, 30);
    assert(board[29] == 0);
    // swap with out of range
    board.swap(0, 30);
    assert(board[0] == 1);
    // about 40 seconds (much less in relase)
    pazusoba::Timer timerSwap("=> swap multiple times");
    for (int64 i = 0; i < LOOPS; i++) {
        board.swap(0, 1);
    }
    timerSwap.end();

    fmt::print("=> test move orbs down\n");
    // get a clean board
    parser.parse();
    auto combo_board = parser.board();
    combo_board[0] = 0;
    combo_board[8] = 0;
    combo_board[9] = 0;
    combo_board[11] = 0;
    combo_board[12] = 0;
    combo_board[16] = 0;
    combo_board[19] = 0;
    combo_board[22] = 0;
    combo_board[24] = 0;
    combo_board[25] = 0;
    combo_board[26] = 0;
    combo_board[27] = 0;
    combo_board[28] = 0;
    combo_board[29] = 0;
    fmt::print("=> before moving\n");
    combo_board.printBoard(pazusoba::PrintStyle::colourful);
    combo_board.moveOrbsDown();
    fmt::print("=> after moving\n");
    combo_board.printBoard(pazusoba::PrintStyle::colourful);
    // First two rows should be empty
    assert(combo_board[1] == 0);
    assert(combo_board[2] == 0);
    assert(combo_board[3] == 0);
    assert(combo_board[4] == 0);
    assert(combo_board[5] == 0);
    assert(combo_board[6] == 0);
    assert(combo_board[7] == 0);
    assert(combo_board[8] == 0);
    assert(combo_board[9] == 0);
    assert(combo_board[10] == 0);
    assert(combo_board[11] == 0);
    assert(combo_board[12] == 0);
    assert(combo_board[16] == 0);
    assert(combo_board[18] != 0);
    assert(combo_board[19] != 0);
    assert(combo_board[21] != 0);
    assert(combo_board[23] != 0);
    assert(combo_board[24] != 0);
    assert(combo_board[26] != 0);
    assert(combo_board[28] != 0);
    assert(combo_board[29] != 0);
    auto combo_board_string =
        combo_board.getFormattedBoard(pazusoba::FormatStyle::dawnglare);
    // Match with the dawnglare format
    assert(combo_board_string == "             HLB GPRRHDJGJDRRL");

    pazusoba::Timer moveOrbs("=> move orbs down multiple times");
    for (int64 i = 0; i < LOOPS; i++) {
        combo_board.moveOrbsDown();
    }
    moveOrbs.end();

    fmt::print("=> test erase orbs\n");
    // There are lots of edge cases so need to test it properly
    parser = pazusoba::Parser("../support/sample_board_65_1.txt", 3, 30, 5000);
    // TODO: maybe parse should be called automatically???
    parser.parse();
    pazusoba::ComboList comboList;
    board = parser.board();
    board.eraseOrbs(comboList);
    output = board.getFormattedBoard(pazusoba::FormatStyle::dawnglare);
    // Should erase all orbs
    assert(output == "                              ");
}

void testState() {
    fmt::print("\n--- Test State ---\n");
    pazusoba::Timer timer("=> Test State");
    auto parser =
        pazusoba::Parser("RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB", 3, 50, 10000);
    parser.parse();
    auto board = parser.board();

    std::deque<pazusoba::State> children;
    fmt::print("=> test children states (0, 0)\n");
    // (0, 0)
    auto state1 = pazusoba::State(board, parser.maxSteps(), 0);
    // digonal moves
    children = state1.children(true);
    assert(children.size() == 3);
    assert(children[0].currentStep() == 1);
    assert(children[0].currIndex() == 6);
    assert(children[0].prevIndex() == 0);
    assert(children[0].maxStep() == parser.maxSteps());
    children.clear();
    // only down and right here
    children = state1.children(false);
    assert(children.size() == 2);
    assert(children[1].currentStep() == 1);
    assert(children[1].currIndex() == 1);
    assert(children[1].prevIndex() == 0);
    assert(children[1].maxStep() == parser.maxSteps());
    children.clear();

    fmt::print("=> test children states (3, 3)\n");
    // (3, 3) in the middle with max 8 valid children states
    state1 = pazusoba::State(board, parser.maxSteps(), 15);
    children = state1.children(true);
    assert(children.size() == 8);
    fmt::print("=> validate (3, 3) -> (2, 3)\n");
    // moving up
    assert(children[0].currentStep() == 1);
    assert(children[0].currIndex() == 9);
    assert(children[0].prevIndex() == 15);
    assert(children[0].maxStep() == parser.maxSteps());
    fmt::print("=> validate new board\n");
    // make sure the board is swapped
    auto up_board = children[0].board();
    auto org_board = state1.board();
    up_board[9] = org_board[15];
    up_board[15] = org_board[9];
    up_board[0] = 0;
    assert(org_board[0] != 0);
    children.clear();

    children = state1.children(false);
    assert(children.size() == 4);
    fmt::print("=> validate (3, 3) -> (3, 2)\n");
    // should be left here
    assert(children[2].currentStep() == 1);
    assert(children[2].currIndex() == 14);
    assert(children[2].prevIndex() == 15);
    assert(children[2].maxStep() == parser.maxSteps());

    fmt::print("=> test max step\n");
    pazusoba::State maxStepState = children[0];
    children.clear();
    // start from step 1 here
    for (int i = 0; i < int(parser.maxSteps()) - 1; ++i) {
        auto children = maxStepState.children(false);
        maxStepState = children[0];
        children.clear();
    }
    assert(maxStepState.currentStep() == parser.maxSteps());
    assert(maxStepState.maxStep() == maxStepState.currentStep());
    children = maxStepState.children(false);
    assert(children.size() == 0);
}

void testBoardErase() {
    fmt::print("\n--- Test Board Erase ---\n");
    pazusoba::Timer timer("=> Test Board Erase");

    // This board should be only one combo not 5
    auto parser = pazusoba::Parser("PHHLBGPRDDRJGJRRHJGRDRHLLPHBHB", 3, 1, 1);
    parser.parse();
    auto state = pazusoba::State(parser.board(), 0, parser.maxSteps(), 0, 0);
    // assert(state.score() == 20);
}

void testSearch() {
    fmt::print("\n--- Test Search ---\n");
    pazusoba::Timer timer("=> Test Search");
    auto parser =
        pazusoba::Parser("RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB", 3, 50, 5000);
    parser.parse();

    pazusoba::Timer quick("=> Test Quick Search");
    // auto quickSearch = pazusoba::QuickSearch(parser);
    // quickSearch.solve();
    quick.end();

    pazusoba::Timer beam("=> Test Beam Search");
    // auto beamSearch = pazusoba::BeamSearch(parser);
    // beamSearch.solve();
    beam.end();
}

void testQueue() {
    const int SIZE = 10000;
    fmt::print("\n--- Test Queue ---\n");
    pazusoba::Timer timer("=> Test Queue");
    pazusoba::pint processor_count = std::thread::hardware_concurrency();
    auto queue = pazusoba::SobaQueue(processor_count);
    assert(queue.threadSize() == processor_count);
    for (const auto& l : queue.list()) {
        assert(l.size() == 0);
    }

    // Prepare for states
    auto parser = pazusoba::Parser("PHHLBGPRDDRJGJRRHJGRDRHLLPHBHB", 3, 1, 1);
    parser.parse();
    auto board = parser.board();

    pazusoba::Timer insert("=> Test Insert");
    std::vector<std::thread> threads;
    for (pazusoba::pint i = 0; i < processor_count; i++) {
        // Copy threadNum to the callback here
        const auto threadNum = i;
        threads.emplace_back([threadNum, &SIZE, &queue, &board] {
            for (int j = 0; j < SIZE; j++) {
                // Test insert without mutex lock
                queue[threadNum].emplace_front(board, 10, 0);
            }
        });
    }

    // Wait for all threads to finish
    for (auto& t : threads)
        t.join();
    threads.clear();
    insert.end();

    // make sure all threads inserted the right number of elements
    for (const auto& l : queue.list()) {
        assert(l.size() == SIZE);
    }
    auto targetSize = SIZE * processor_count;

    queue.group();
    assert(queue.size() == (int)targetSize);

    pazusoba::pint counter = 0;
    pazusoba::pint score = 0;
    // This is very slow to complete when there are 60000 states
    while (!queue.empty()) {
        auto state = queue.next();
        score = state.score();
        queue.pop();
        // fmt::print("counter = {}, size = {}, score = {}\n", counter,
        //            queue.size(), score);
        counter++;
    }
    fmt::print("counter = {}, score = {}\n", counter, score);
    assert(counter == targetSize);
}
