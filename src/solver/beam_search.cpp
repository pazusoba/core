#include <fmt/core.h>
#include <pazusoba/queue.h>
#include <pazusoba/solver/beam_search.h>
#include <pazusoba/state.h>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace pazusoba {
State BeamSearch::solve() {
    // setup multithreading
    std::deque<std::thread> threads;
    // Save one core for now
    pint processor_count = std::thread::hardware_concurrency();
    // processor_count = 1;
    fmt::print("Using {} threads\n", processor_count);

    const Board& board = _parser.board();
    SobaQueue<State*> pq(processor_count);
    std::unordered_map<size_t, bool> visited;
    std::mutex mtx;

    // setup all the initial states
    auto maxSteps = _parser.maxSteps();
    for (pint i = 0; i < board.size(); ++i) {
        // Need to add 0 for the initial state
        pq.insert(new State(board, maxSteps, i));
    }

    State bestState = *pq.next();
    // Use Beam Search starting from step one
    auto beamSize = _parser.beamSize() / processor_count + 1;
    fmt::print("Beam Size {}\n", beamSize);
    for (pint i = 0; i < maxSteps; ++i) {
        for (pint thread = 0; thread < processor_count; ++thread) {
            if (pq.empty()) {
                break;
            }

            threads.emplace_front(
                [thread, &bestState, &beamSize, &mtx, &pq, &visited] {
                    for (pint k = 0; k < beamSize; ++k) {
                        mtx.lock();
                        if (pq.empty()) {
                            mtx.unlock();
                            return;
                        }

                        auto current = pq.next();
                        pq.pop();
                        mtx.unlock();

                        if (current->shouldCutOff()) {
                            continue;
                        }

                        mtx.lock();
                        if (current->score() > bestState.score()) {
                            bestState = *current;
                        }
                        mtx.unlock();

                        auto hash = current->hash();
                        mtx.lock();
                        if (visited[hash]) {
                            mtx.unlock();
                            // check more since this one is already checked
                            // need to consider, this slows down
                            // k -= 1;
                            continue;
                        } else {
                            visited[hash] = true;
                        }
                        mtx.unlock();

                        for (const auto& child : current->children(false)) {
                            pq[thread].push_front(child);
                            // This is not safe yet
                            // pq.push(child, thread);
                        }
                        delete current;
                    }
                });
        }

        for (auto& t : threads)
            t.join();
        threads.clear();

        pq.group();
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
