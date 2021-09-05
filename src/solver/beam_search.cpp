#include <fmt/core.h>
#include <pazusoba/queue.h>
#include <pazusoba/solver/beam_search.h>
#include <pazusoba/state.h>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace pazusoba {
Route BeamSearch::solve() {
    // setup multithreading
    std::deque<std::thread> threads;
    // Save one core for now
    pint processor_count = std::thread::hardware_concurrency();
    // processor_count = 1;
    fmt::print("Using {} threads\n", processor_count);

    const Board& board = _parser.board();
    SobaQueue<State> pq(processor_count);
    std::unordered_map<size_t, bool> visited;
    std::mutex mtx;

    // setup all the initial states
    auto maxSteps = _parser.maxSteps();
    for (pint i = 0; i < board.size(); ++i) {
        // Need to add 0 for the initial state
        pq.insert(State(board, maxSteps, i));
    }

    State bestState = pq.next();
    // Use Beam Search starting from step one
    auto beamSize = _parser.beamSize() / processor_count + 1;
    fmt::print("Beam Size {}\n", beamSize);
    for (pint i = 0; i < maxSteps; ++i) {
        fmt::print("Step {}\n", i);
        for (pint j = 0; j < processor_count; ++j) {
            if (pq.empty()) {
                break;
            }

            const auto thread_id = j;
            threads.emplace_front(
                [thread_id, &bestState, &beamSize, &mtx, &pq, &visited] {
                    for (pint k = 0; k < beamSize; ++k) {
                        mtx.lock();
                        if (pq.empty()) {
                            mtx.unlock();
                            return;
                        }

                        auto current = pq.next();
                        pq.pop();
                        mtx.unlock();

                        if (current.shouldCutOff()) {
                            continue;
                        }

                        if (current.score() > bestState.score()) {
                            mtx.lock();
                            bestState = current;
                            mtx.unlock();
                        }

                        auto hash = current.hash();
                        if (visited[hash]) {
                            // check more since this one is already checked
                            // need to consider, this slows down
                            // k -= 1;
                            continue;
                        } else {
                            mtx.lock();
                            visited[hash] = true;
                            mtx.unlock();
                        }

                        auto children = current.children(false);
                        for (const auto& child : children) {
                            pq[thread_id].push_front(child);
                        }
                    }
                });
        }

        for (auto& t : threads)
            t.join();
        threads.clear();

        pq.group();
        // fmt::print("Step {}, join\n", i);
    }

    auto b = bestState.board();
    fmt::print("Best Score {}\n", bestState.score());
    fmt::print("{}\n", b.getFormattedBoard(dawnglare));
    b.printBoard(colourful);
    /// TO BE UPDATED
    return Route(0);
}
}  // namespace pazusoba
