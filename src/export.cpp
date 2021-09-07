/// All functions which are callable from the dynamic library

#include <fmt/core.h>
#include <pazusoba/core.h>

// MAYBE add all C types here??

extern "C" {
namespace pazusoba {
///
/// ROUTES
///
CRoute* beamSearch(int argc, char* argv[]) {
    fmt::print("Calling solve from shared library\n");
    for (int i = 0; i < argc; i++) {
        fmt::print("argv[{}] = {}\n", i, argv[i]);
    }

    auto parser = Parser(argc, argv);
    parser.parse();
    auto search = BeamSearch(parser);
    auto route = search.solve();
    return route.exportedRoute();
}

void freeRoute(CRoute* list) {
    if (list != nullptr) {
        delete[] list;
        fmt::print("Freed all routes\n");
    }
}

///
/// COMBO
///
int findMaxCombo(std::string board, pint step) {
    auto parser = Parser(board, 3, step, 0);
    parser.parse();

    pint maxCombo = 0;
    for (pint i = 0; i < board.size(); ++i) {
        // Searching from every orb
        auto state = State(parser.board(), step, i);
        for (const auto& child : state.allChildren(step, false)) {
            auto combo = child.combo();
            if (combo > maxCombo)
                maxCombo = combo;
        }
    }
    return maxCombo;
}
}  // namespace pazusoba
}
