#include <fmt/core.h>
#include <pazusoba/queue.h>
#include <pazusoba/solver/quick_search.h>
#include <pazusoba/state.h>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace pazusoba {
Route QuickSearch::solve() {
    std::unordered_map<size_t, bool> visited;
    std::vector<State> states;
    std::mutex mtx;
    // auto push = [&](const State& state) {
    //     mtx.lock();
    //     states.push_back(state);
    //     mtx.unlock();
    // };

    std::vector<std::thread> threads;
    // Save one core for now
    int processor_count = std::thread::hardware_concurrency() - 1;
    // processor_count = 0;
    if (processor_count == 0)
        processor_count = 1;
    fmt::print("Using {} threads\n", processor_count);
    threads.reserve(processor_count);

    // setup all the initial states
    const Board& board = _parser.board();
    for (pint i = 0; i < board.size(); ++i) {
        // Need to add 0 for the initial state
        states.emplace_back(board, _parser.maxSteps(), i);
    }

    State bestState = states[0];
    // Use Beam Search starting from step one
    while (!states.empty()) {
        fmt::print("array size: {}\n", states.size());

        for (int i = 0; i < processor_count; ++i) {
            threads.emplace_back([&] {
                if (states.empty() || states.size() > _parser.beamSize())
                    return;
                auto state = states.back();
                mtx.lock();
                states.pop_back();
                auto hash = state.hash();
                if (visited[hash]) {
                    mtx.unlock();
                    return;
                } else {
                    visited[hash] = true;
                }
                mtx.unlock();

                auto score = state.score();
                if (score > bestState.score()) {
                    bestState = state;
                }
                // state.children(push, false);
            });
        }

        for (auto& t : threads)
            t.join();
        threads.clear();
    }

    fmt::print("{}\n", bestState.score());

    /// TO BE UPDATED
    return Route(0);
}
}  // namespace pazusoba
