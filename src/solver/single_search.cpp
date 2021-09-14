#include <fmt/core.h>
#include <pazusoba/queue.h>
#include <pazusoba/solver/single_search.h>
#include <pazusoba/state.h>
#include <unordered_map>

namespace pazusoba {
State SingleSearch::solve() {
    // Save one core for now
    pint processor_count = 1;
    fmt::print("Using {} threads\n", processor_count);

    const Board& board = _parser.board();
    typedef std::priority_queue<State, std::vector<State>, QueueCompare>
        SingleQueue;
    // use two queues to swap around
    SingleQueue first;
    SingleQueue second;
    std::unordered_map<size_t, bool> visited;

    // setup all the initial states
    auto maxSteps = _parser.maxSteps();
    for (pint i = 0; i < board.size(); ++i) {
        // Need to add 0 for the initial state
        first.push(State(board, maxSteps, i));
    }

    State bestState = first.top();
    // Use Beam Search starting from step one
    auto beamSize = _parser.beamSize() / processor_count + 1;
    fmt::print("Beam Size {}\n", beamSize);
    for (pint i = 0; i < maxSteps; ++i) {
        auto& curr = i % 2 == 0 ? first : second;
        auto& next = i % 2 == 0 ? second : first;
        for (pint j = 0; j < beamSize; ++j) {
            if (curr.empty())
                break;

            auto current = curr.top();
            curr.pop();

            if (current.shouldCutOff()) {
                continue;
            }

            if (current.combo() > bestState.combo()) {
                bestState = current;
            }

            auto hash = current.hash();
            if (visited[hash]) {
                // check more since this one is already checked
                // need to consider, this slows down
                // k -= 1;
                continue;
            } else {
                visited[hash] = true;
            }

            auto children = current.children(false);
            for (const auto child : children) {
                next.push(*child);
            }
        }

        // reset
        while (!curr.empty()) {
            auto state = curr.top();
            curr.pop();
        }
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
