#pragma once
#include "Game.hpp"
#include "TranspositionTable.hpp"
#include <algorithm>
#include <chrono>
#include <limits>

namespace sge {

// One reusable Minimax + Alpha-Beta engine, templated on any GameState
// implementation. Connect4State and TicTacToeState both plug into this
// exact same class -- this is the "modular architecture" and "dynamic
// state management" from the resume bullet made literal: adding a new
// game means writing a new GameState, not a new search algorithm.
//
// Implemented as Negamax: instead of separate maximizing/minimizing
// branches, every recursive call scores the position from the current
// mover's perspective and negates the child's score before combining
// (score(node) = -score(node, from opponent's view)). This halves the
// amount of near-duplicate code a hand-written maximizing/minimizing
// pair would otherwise need.
template <typename StateT>
class AlphaBetaSearcher {
public:
    using Move = typename StateT::Move;

    struct Result {
        Move bestMove{};
        int score = 0;
        int depthReached = 0;
        long long nodesVisited = 0;
        bool timedOut = false;
    };

    // maxDepth: hard depth ceiling (safety net / exactness knob).
    // timeBudget: wall-clock budget per top-level move decision -- this
    // is what actually enforces the "sub-100ms" resume claim instead of
    // just asserting it. Iterative deepening means we always have a
    // *complete* depth-N result ready even if depth N+1 gets cut off
    // mid-search.
    explicit AlphaBetaSearcher(int maxDepth = 12,
                                std::chrono::milliseconds timeBudget = std::chrono::milliseconds(90))
        : maxDepth_(maxDepth), timeBudget_(timeBudget) {}

    Result search(StateT& state, Player perspective) {
        Result best{};
        start_ = std::chrono::steady_clock::now();
        timedOut_ = false;
        nodes_ = 0;
        tt_.clear();

        for (int depth = 1; depth <= maxDepth_; ++depth) {
            Result candidate = searchToDepth(state, perspective, depth);
            if (timedOut_) {
                best.timedOut = true;
                break; // discard the incomplete deeper search, keep last full one
            }
            best = candidate;
            best.depthReached = depth;

            // Already found a proven forced win/loss -- no need to search deeper.
            if (candidate.score > 90000 || candidate.score < -90000) break;
        }
        return best;
    }

    long long lastTranspositionTableSize() const { return static_cast<long long>(tt_.nodeCount()); }

private:
    int maxDepth_;
    std::chrono::milliseconds timeBudget_;
    std::chrono::steady_clock::time_point start_;
    bool timedOut_ = false;
    long long nodes_ = 0;
    TranspositionTable<int> tt_;

    bool outOfTime() const {
        return std::chrono::steady_clock::now() - start_ > timeBudget_;
    }

    Result searchToDepth(StateT& state, Player perspective, int depth) {
        Result r{};
        int alpha = std::numeric_limits<int>::min() + 1;
        int beta  = std::numeric_limits<int>::max() - 1;
        int bestScore = std::numeric_limits<int>::min() + 1;
        Move bestMove{};
        bool any = false;

        for (const auto& m : state.legalMoves()) {
            state.applyMove(m);
            int score = -negamax(state, opponent(perspective), depth - 1, -beta, -alpha, perspective);
            state.undoMove(m);

            if (timedOut_) break;
            if (!any || score > bestScore) {
                bestScore = score;
                bestMove = m;
                any = true;
            }
            alpha = std::max(alpha, bestScore);
        }
        r.bestMove = bestMove;
        r.score = bestScore;
        r.nodesVisited = nodes_;
        return r;
    }

    int negamax(StateT& state, Player toMove, int depth, int alpha, int beta, Player perspective) {
        if ((++nodes_ & 0x7FF) == 0 && outOfTime()) { // check clock every 2048 nodes, not every node
            timedOut_ = true;
            return 0;
        }

        if (state.isTerminal() || depth == 0) {
            int s = state.evaluate(perspective);
            return (toMove == perspective) ? s : -s;
        }

        uint64_t node = state.hash();
        if (auto cached = tt_.lookup(node, depth)) {
            return *cached;
        }

        int best = std::numeric_limits<int>::min() + 1;
        for (const auto& m : state.legalMoves()) {
            state.applyMove(m);
            int score = -negamax(state, opponent(toMove), depth - 1, -beta, -alpha, perspective);
            state.undoMove(m);
            if (timedOut_) return 0;

            best = std::max(best, score);
            alpha = std::max(alpha, score);
            if (alpha >= beta) break; // *** the alpha-beta cutoff ***
        }
        tt_.store(node, depth, best);
        return best;
    }
};

} // namespace sge
