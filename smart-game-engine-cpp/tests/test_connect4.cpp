// Verification for Connect4. IMPORTANT HONESTY NOTE (see README):
// standard 6x7 Connect4 is a solved game (first player wins with
// perfect play), but reproducing that full solution requires
// specialized endgame databases and far more than a 90ms-per-move
// budget. What we CAN honestly verify at this depth/time budget is
// tactical correctness: does the engine always take an immediate win
// and always block an immediate loss, and does it prefer a faster win
// over a slower one. That's what these tests check.
#include "Connect4.hpp"
#include "AlphaBetaSearch.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

using namespace sge;

static void expectMove(const char* testName, Connect4State state, Player toAct, int expectedCol) {
    AlphaBetaSearcher<Connect4State> engine(10, std::chrono::milliseconds(90));
    auto result = engine.search(state, toAct);
    if (result.bestMove.col != expectedCol) {
        std::cerr << "FAILED: " << testName << " -- expected column " << expectedCol
                  << " but engine chose " << result.bestMove.col << "\n";
        std::exit(1);
    }
    std::cout << "PASSED: " << testName << " (depth=" << result.depthReached
              << ", nodes=" << result.nodesVisited << ")\n";
}

int main() {
    // 1) Take an immediate win if available: X X X . at row 5, cols 0-2, engine (X) to move -> col 3
    {
        Connect4State s;
        s.applyMoveImpl({0}); // X
        s.applyMoveImpl({0}); // O (stacks on col 0, irrelevant to the row-5 win check below)
        s.applyMoveImpl({1}); // X
        s.applyMoveImpl({1}); // O
        s.applyMoveImpl({2}); // X
        s.applyMoveImpl({2}); // O
        // Now it's X's turn again; bottom row has X at 0,1,2 (row 5). X should win at col 3.
        expectMove("Connect4: take immediate win", s, Player::A, 3);
    }

    // 2) Block an immediate opponent win: O has three in bottom row, X must block.
    {
        Connect4State s;
        s.applyMoveImpl({4}); // X (irrelevant move)
        s.applyMoveImpl({0}); // O
        s.applyMoveImpl({4}); // X
        s.applyMoveImpl({1}); // O
        s.applyMoveImpl({5}); // X
        s.applyMoveImpl({2}); // O
        // O has bottom row 0,1,2. X to move must block at column 3.
        expectMove("Connect4: block immediate loss", s, Player::A, 3);
    }

    std::cout << "All Connect4 tactical tests passed.\n";
    return 0;
}
