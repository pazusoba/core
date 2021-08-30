#include <fmt/core.h>
#include <pazusoba/queue.h>
#include <pazusoba/solver/beam_search.h>
#include <pazusoba/state.h>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace pazusoba {
Route BeamSearch::solve() {
    const Board& board = _parser.board();
    StatePriorityQueue pq;
    std::unordered_map<size_t, bool> visited;
    std::mutex mtx;
    std::deque<State> toVisit;
    auto push = [&](const State& state) {
        mtx.lock();
        toVisit.push_back(state);
        mtx.unlock();
    };

    // setup all the initial states
    auto maxSteps = _parser.maxSteps();
    for (pint i = 0; i < board.size(); ++i) {
        // Need to add 0 for the initial state
        pq.emplace(board, maxSteps, i);
    }

    // setup multithreading
    std::deque<std::thread> threads;
    // Save one core for now
    pint processor_count = std::thread::hardware_concurrency();
    // processor_count = 1;
    fmt::print("Using {} threads\n", processor_count);

    // Use Beam Search starting from step one
    State bestState = pq.top();
    auto beamSize = _parser.beamSize() / processor_count + 1;
    fmt::print("Beam Size {}\n", beamSize);
    for (pint i = 0; i < maxSteps; ++i) {
        for (pint j = 0; j < processor_count; ++j) {
            threads.emplace_back([&] {
                for (pint k = 0; k < beamSize; ++k) {
                    mtx.lock();
                    if (pq.empty()) {
                        mtx.unlock();
                        return;
                    }

                    auto current = pq.top();
                    pq.pop();
                    mtx.unlock();

                    mtx.lock();
                    auto hash = current.hash();
                    if (visited[hash]) {
                        mtx.unlock();
                        // check more since this one is already checked
                        // need to consider, this slows down
                        // j -= 1;
                        continue;
                    } else {
                        visited[hash] = true;
                        mtx.unlock();
                    }

                    current.children(push, false);
                }
            });
        }

        for (auto& t : threads)
            t.join();
        threads.clear();

        for (const auto& state : toVisit) {
            pq.push(state);
            auto score = state.score();
            if (score > bestState.score()) {
                bestState = state;
            }
        }
        toVisit.clear();
    }

    fmt::print("best score: {}\n", bestState.score());
    /// TO BE UPDATED
    return Route(0);
}
}  // namespace pazusoba
