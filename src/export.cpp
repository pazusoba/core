/// All functions which are callable from the dynamic library

#include <fmt/core.h>
#include <pazusoba/core.h>

extern "C" {
pazusoba::Route solve(int argc, char* argv[]) {
    fmt::print("Calling solve from shared library");
    for (int i = 0; i < argc; i++) {
        fmt::print("argv[{}] = {}\n", i, argv[i]);
    }

    auto parser = pazusoba::Parser(argc, argv);
    auto search = pazusoba::BeamSearch(parser);
    return search.solve();
}
