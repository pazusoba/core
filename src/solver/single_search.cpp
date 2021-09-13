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
    std::priority_queue<State*, std::vector<State*>, QueueCompare> pq;
    std::unordered_map<size_t, bool> visited;

    // setup all the initial states
    auto maxSteps = _parser.maxSteps();
    for (pint i = 0; i < board.size(); ++i) {
        // Need to add 0 for the initial state
        pq.push(new State(board, maxSteps, i));
    }

    State bestState = *pq.top();
    // Use Beam Search starting from step one
    auto beamSize = _parser.beamSize() / processor_count + 1;
    fmt::print("Beam Size {}\n", beamSize);
    for (pint i = 0; i < maxSteps; ++i) {
        // fmt::print("Step {}\n", i + 1);
        for (pint j = 0; j < beamSize; ++j) {
            if (pq.empty())
                break;

            auto current = pq.top();
            pq.pop();

            if (current->shouldCutOff()) {
                delete current;
                continue;
            }

            if (current->score() > bestState.score()) {
                bestState = *current;
            }

            auto hash = current->hash();
            if (visited[hash]) {
                // check more since this one is already checked
                // need to consider, this slows down
                // k -= 1;
                delete current;
                continue;
            } else {
                visited[hash] = true;
            }

            for (const auto child : current->children(false)) {
                pq.push(child);
            }
            delete current;
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
