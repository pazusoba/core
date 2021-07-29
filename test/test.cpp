#include <assert.h>
#include <fmt/core.h>
#include <pazusoba/core.h>

void testAll();
void testTimer();
void testParser();

int main() {
    testAll();
    fmt::print("\nALL GOOD!\n");
}

void testAll() {
    testTimer();
    testParser();
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
    assert(pazuboard[30] == -1);
    assert(parser.Row() == 5);
    assert(parser.Column() == 6);

    fmt::print("=> two arguments\n");
    const char* two[] = {"pazusoba", "RHLBDGPRHDRJPJRHHJGR", "3"};
    parser = pazusoba::parser(3, (char**)two);
    parser.parse();
    pazuboard = parser.Board();
    assert(pazuboard[19] == 1);
    assert(pazuboard[20] == -1);
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

    fmt::print("=> pass data in\n");
    parser = pazusoba::parser("RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB", 5, 200, 50000);
    parser.parse();
    pazuboard = parser.Board();
    assert(pazuboard[0] == 1);
    assert(pazuboard[5] == 3);
    assert(pazuboard[29] == 2);
    assert(pazuboard[30] == -1);
    assert(parser.Row() == 5);
    assert(parser.Column() == 6);
    assert(parser.MinErase() == 5);
    assert(parser.MaxSteps() == 200);
    assert(parser.BeamSize() == 50000);
}