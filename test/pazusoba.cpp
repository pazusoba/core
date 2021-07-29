/*
main_v2.cpp
The entry point of pazusoba. It handles command line arguments

Created by Yiheng Quan on 12/11/2020
*/

#include <pazusoba/core.h>

int main(int argc, char* argv[]) {
    pazusoba::timer timer("=> pazusoba");
    auto parser = pazusoba::parser(argc, argv);
    parser.parse();
    return 0;
}
