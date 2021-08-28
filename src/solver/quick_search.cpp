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
    StatePriorityQueue pq;
    std::mutex mtx;
    auto push = [&pq, &mtx](const State& state) {
        mtx.lock();
        pq.push(state);
        mtx.unlock();
    };

    std::vector<std::thread> threads;
    // Save one core for now
    int processor_count = std::thread::hardware_concurrency() - 1;
    // int processor_count = 0;
    if (processor_count == 0)
        processor_count = 1;
    fmt::print("Using {} threads\n", processor_count);
    threads.reserve(processor_count);

    // setup all the initial states
    const Board& board = _parser.board();
    for (pint i = 0; i < board.size(); ++i) {
        // Need to add 0 for the initial state
        pq.emplace(board, _parser.maxSteps(), i);
    }

    State bestState = pq.top();
    bool found = false;
    // Use Beam Search starting from step one
    while (!pq.empty()) {
        fmt::print("queue size: {}\n", pq.size());
        for (int i = 0; i < processor_count; ++i) {
            if (found)
                break;
            threads.emplace_back([&] {
                if (pq.empty())
                    return;
                auto state = pq.top();
                mtx.lock();
                pq.pop();
                mtx.unlock();
                auto hash = state.hash();
                if (visited[hash])
                    return;
                else
                    visited[hash] = true;

                auto score = state.score();
                if (score > bestState.score()) {
                    mtx.lock();
                    bestState = state;
                    mtx.unlock();
                }
                if (score > 180)
                    found = true;
                state.children(push, false);
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
