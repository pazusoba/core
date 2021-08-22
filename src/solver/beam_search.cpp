#include <pazusoba/solver/beam_search.h>
#include <pazusoba/state.h>

namespace pazusoba {
Route BeamSearch::solve() {
    const Board& board = _parser.board();

    // setup all the initial states
    for (pint i = 0; i < board.size(); ++i) {
        // TODO: need to add this to a queue, NEED TO THINK ABOUT THIS
        auto state = State(board, _parser.maxSteps(), i);
    }

    // Use Beam Search starting from step one

    /// TO BE UPDATED
    return Route(0);
}
};  // namespace pazusoba
