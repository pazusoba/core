/// All functions which are callable from the dynamic library

#include <fmt/core.h>
#include <pazusoba/core.h>

// MAYBE add all C types here??

extern "C" {
namespace pazusoba {
CRoute* beamSearch(int argc, char* argv[]) {
    fmt::print("Calling solve from shared library\n");
    for (int i = 0; i < argc; i++) {
        fmt::print("argv[{}] = {}\n", i, argv[i]);
    }

    auto parser = pazusoba::Parser(argc, argv);
    parser.parse();
    auto search = pazusoba::BeamSearch(parser);
    auto route = search.solve();
    return route.exportedRoute();
}

void freeRoute(CRoute* list) {
    if (list != nullptr) {
        delete[] list;
        fmt::print("Freed all routes\n");
    }
}
}  // namespace pazusoba
}
