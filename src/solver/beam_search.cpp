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
    // processor_count = 0;
    if (processor_count == 0)
        processor_count = 1;
    fmt::print("Using {} threads\n", processor_count);

    // Use Beam Search starting from step one
    State bestState = pq.top();
    auto beamSize = _parser.beamSize() / processor_count + 1;
    for (pint i = 0; i < maxSteps; i++) {
        fmt::print("step {}\n", i + 1);
        for (pint j = 0; j < processor_count; j++) {
            threads.emplace_back([&] {
                for (pint k = 0; k < beamSize; k++) {
                    if (pq.empty())
                        return;

                    mtx.lock();
                    auto current = pq.top();
                    pq.pop();
                    mtx.unlock();

                    auto hash = current.hash();
                    if (visited[hash]) {
                        // check more since this one is already checked
                        // j -= 1;
                        continue;
                    } else {
                        visited[hash] = true;
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
            if (score > 0)
                fmt::print("score {}", score);
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
