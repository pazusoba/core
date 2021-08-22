#include <pazusoba/queue.h>
#include <pazusoba/solver/beam_search.h>
#include <pazusoba/state.h>

namespace pazusoba {
Route BeamSearch::solve() {
    const Board& board = _parser.board();
    StatePriorityQueue pq;

    // setup all the initial states
    for (pint i = 0; i < board.size(); ++i) {
        pq.emplace(board, _parser.maxSteps(), i);
    }

    // Use Beam Search starting from step one

    /// TO BE UPDATED
    return Route(0);
}
};  // namespace pazusoba
