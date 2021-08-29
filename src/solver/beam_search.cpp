#include <fmt/core.h>
#include <pazusoba/queue.h>
#include <pazusoba/solver/beam_search.h>
#include <pazusoba/state.h>
#include <unordered_map>

namespace pazusoba {
Route BeamSearch::solve() {
    const Board& board = _parser.board();
    StatePriorityQueue pq;
    std::unordered_map<size_t, bool> visited;

    auto push = [&](const State& state) { pq.push(state); };

    // setup all the initial states
    auto maxSteps = _parser.maxSteps();
    for (pint i = 0; i < board.size(); ++i) {
        // Need to add 0 for the initial state
        pq.emplace(board, maxSteps, i);
    }

    // Use Beam Search starting from step one
    State bestState = pq.top();
    auto beamSize = _parser.beamSize();
    for (pint i = 0; i < maxSteps; i++) {
        for (pint j = 0; j < beamSize; j++) {
            if (pq.empty())
                break;

            auto current = pq.top();
            pq.pop();

            auto hash = current.hash();
            if (visited[hash]) {
                continue;
            } else {
                visited[hash] = true;
            }

            auto score = current.score();
            if (score > bestState.score()) {
                bestState = current;
            }

            current.children(push, false);
        }
    }

    fmt::print("best score: {}\n", bestState.score());
    /// TO BE UPDATED
    return Route(0);
}
}  // namespace pazusoba
