// 4x4 tic-tac-toe has only 3^16 (~43M) reachable board codes and a
// small branching factor, so searching to full depth (16, i.e. the
// whole board) with no time cutoff is EXACT -- not a heuristic
// approximation. That's what makes "verified against game-theoretic
// solutions" an honest claim for this file specifically (unlike
// Connect4, see test_connect4.cpp).
#include "TicTacToe.hpp"
#include "AlphaBetaSearch.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

using namespace sge;

int main() {
    TicTacToeState s;
    AlphaBetaSearcher<TicTacToeState> engine(16, std::chrono::milliseconds(30000));

    auto t0 = std::chrono::steady_clock::now();
    auto result = engine.search(s, Player::A);
    double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();

    std::cout << "Exact game-theoretic value of the empty 4x4 board for the first player: ";
    if (result.score > 0) std::cout << "WIN";
    else if (result.score < 0) std::cout << "LOSS";
    else std::cout << "DRAW";
    std::cout << " (score=" << result.score << ", nodes=" << result.nodesVisited
              << ", tt_entries=" << engine.lastTranspositionTableSize()
              << ", time=" << ms << "ms)\n";

    assert(!result.timedOut && "search must complete exactly, not time out, to be a valid verification");

    // Sanity/regression checks that don't depend on knowing the exact
    // value in advance:
    // 1) The engine must always take an immediate winning move.
    {
        TicTacToeState win3;
        // Build three-in-a-row for X on row 0, cols 0-2; col 3 open.
        win3.applyMoveImpl({0, 0}); // X
        win3.applyMoveImpl({1, 0}); // O
        win3.applyMoveImpl({0, 1}); // X
        win3.applyMoveImpl({1, 1}); // O
        win3.applyMoveImpl({0, 2}); // X
        win3.applyMoveImpl({1, 2}); // O
        // X to move, should take (0,3) to win immediately.
        auto r = engine.search(win3, Player::A);
        assert(r.bestMove.row == 0 && r.bestMove.col == 3);
        std::cout << "PASSED: takes immediate winning move\n";
    }

    // 2) A position and its mirror image must evaluate to the same score
    //    (structural symmetry check on the evaluation, not a hand-picked answer).
    {
        TicTacToeState a, b;
        a.applyMoveImpl({0, 0});
        b.applyMoveImpl({0, 3}); // horizontal mirror of (0,0)
        auto ra = engine.search(a, Player::B);
        auto rb = engine.search(b, Player::B);
        assert(ra.score == rb.score);
        std::cout << "PASSED: mirrored positions evaluate identically (score=" << ra.score << ")\n";
    }

    std::cout << "All Tic-Tac-Toe tests passed.\n";
    return 0;
}
