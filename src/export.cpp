/// All functions which are callable from the dynamic library

#include <pazusoba/core.h>

extern "C" {
pazusoba::Route solve(int argc, char* argv[]) {
    auto parser = pazusoba::Parser(argc, argv);
    auto search = pazusoba::BeamSearch(parser);
    return search.solve();
}
}
