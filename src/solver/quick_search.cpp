#include <fmt/core.h>
#include <pazusoba/queue.h>
#include <pazusoba/solver/quick_search.h>
#include <pazusoba/state.h>
namespace pazusoba {
Route QuickSearch::solve() {
    const Board& board = _parser.board();
    StatePriorityQueue pq;
    auto push = [&pq](const State& state) { pq.push(state); };

    // setup all the initial states
    for (pint i = 0; i < board.size(); ++i) {
        // Need to add 0 for the initial state
        pq.emplace(board, _parser.maxSteps(), i);
    }

    // Use Beam Search starting from step one
    while (!pq.empty()) {
        auto state = pq.top();
        auto score = state.score();
        if (score > 10)
            fmt::print("{}\n", score);
        if (score > 180)
            break;
        state.children(push, false);
    }

    /// TO BE UPDATED
    return Route(0);
}
}  // namespace pazusoba
