#include <fmt/core.h>
#include <pazusoba/constant.h>
#include <pazusoba/queue.h>
#include <pazusoba/solver/quick_search.h>
#include <pazusoba/state.h>
#include <algorithm>
#include <unordered_map>

namespace pazusoba {

pint QuickSearch::scan() {
    pint info[pad::ORB_COUNT] = {0};
    auto board = _parser.board();
    for (pint i = 0; i < board.size(); i++) {
        info[board[i]]++;
    }

    pint combo = 0;
    for (int i = 0; i < pad::ORB_COUNT; i++) {
        combo += info[i] / 3;
    }
    return combo;
}

State QuickSearch::solve() {
    typedef std::priority_queue<State, std::vector<State>, ValueCompare>
        QuickQueue;
    std::unordered_map<size_t, bool> visited;
    std::vector<State> states;
    QuickQueue queue;

    // reserve enough space for all states
    auto beamSize = _parser.beamSize();
    states.reserve(beamSize * 4);

    pint maxCombo = scan();
    fmt::print("Max combo - {}\n", maxCombo);

    // setup all the initial states
    const Board& board = _parser.board();
    for (pint i = 0; i < board.size(); ++i) {
        // Need to add 0 for the initial state
        queue.emplace(board, _parser.maxSteps(), i);
    }

    State bestState = states[0];
    auto maxStep = _parser.maxSteps();
    pint lastImprovement = 0;
#pragma omp declare reduction (merge : std::vector<State> : omp_out.insert(omp_out.end(), omp_in.begin(), omp_in.end()))
    // Use Beam Search starting from step one
#pragma omp parallel for reduction(merge : states) shared(queue)
    for (pint i = 0; i < maxStep; i++) {
        // No improvement for a while, stop
        for (pint j = 0; j < beamSize; j++) {
            // Simply insert everything
            if (queue.empty())
                j = beamSize;

            auto current = queue.top();
            queue.pop();

            if (current.combo() >= maxCombo) {
                auto b = current.board();
                fmt::print("Best Score - {}\n", current.score());
                fmt::print("Steps - {}\n", current.currentStep());
                fmt::print("Combo - {}\n", current.combo());
                fmt::print("{}\n", b.getFormattedBoard(dawnglare));
                current.route().printRoute();
                current.route().writeToDisk();
                b.printBoard(colourful);
            }

            auto hash = current.hash();
            if (visited[hash]) {
                continue;
            } else {
                visited[hash] = true;
            }

            if (current.combo() > bestState.combo()) {
                bestState = current;
                lastImprovement = i;
            }

            // Generate all the next states
            for (const auto state : current.children(false)) {
                states.push_back(*state);
            }
        }

        std::sort(states.begin(), states.end(), greater());

        for (pint j = 0; j < beamSize; j++) {
            queue.emplace(states[j]);
        }

        states.clear();
    }

    auto b = bestState.board();
    fmt::print("Best Score - {}\n", bestState.score());
    fmt::print("Steps - {}\n", bestState.currentStep());
    fmt::print("Combo - {}\n", bestState.combo());
    fmt::print("{}\n", b.getFormattedBoard(dawnglare));
    bestState.route().printRoute();
    bestState.route().writeToDisk();
    b.printBoard(colourful);
    return bestState;
}
}  // namespace pazusoba
