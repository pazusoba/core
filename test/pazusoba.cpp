/*
main_v2.cpp
The entry point of pazusoba. It handles command line arguments

Created by Yiheng Quan on 12/11/2020
*/

#include <pazusoba/core.h>

int main(int argc, char* argv[]) {
    pazusoba::Timer t("=> pazusoba");
    // this should find 8 combo
     auto parser =
         pazusoba::Parser("RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB", 3, 50, 5000);
//    auto parser = pazusoba::Parser(argc, argv);
    parser.parse();
    auto solver = pazusoba::SingleSearch(parser);
    solver.solve();
    return 0;
}
