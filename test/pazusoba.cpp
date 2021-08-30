/*
main_v2.cpp
The entry point of pazusoba. It handles command line arguments

Created by Yiheng Quan on 12/11/2020
*/

#include <pazusoba/core.h>

int main(int argc, char* argv[]) {
    pazusoba::Timer timer("=> pazusoba");
    auto parser = pazusoba::Parser("RHGLBJDHDHRJDHDHRJDRRHLLGGDBRB", 3, 2, 10);
    // auto parser = pazusoba::Parser(argc, argv);
    parser.parse();
    auto solver = pazusoba::BeamSearch(parser);
    solver.solve();
    return 0;
}
