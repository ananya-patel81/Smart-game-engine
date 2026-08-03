#include "TicTacToe.hpp"
#include "AlphaBetaSearch.hpp"
#include <iostream>

using namespace sge;

int main() {
    TicTacToeState state;
    // No time budget needed: 4x4 (16 cells) is small enough to search
    // to full depth exactly, so this reproduces the true game-theoretic
    // value rather than a depth-limited approximation.
    AlphaBetaSearcher<TicTacToeState> engine(/*maxDepth=*/16, std::chrono::milliseconds(5000));

    std::cout << "You are X, engine is O.\n" << state.toString() << "\n";

    while (!state.isTerminalImpl()) {
        if (state.currentPlayerImpl() == Player::A) {
            int r, c;
            std::cout << "Your move (row col, 0-3 each): ";
            std::cin >> r >> c;
            state.applyMoveImpl(TicTacToeMove{r, c});
        } else {
            auto result = engine.search(state, Player::B);
            state.applyMoveImpl(result.bestMove);
            std::cout << "Engine plays (" << result.bestMove.row << "," << result.bestMove.col
                      << ") | full-depth score=" << result.score
                      << " | nodes=" << result.nodesVisited << "\n";
        }
        std::cout << state.toString() << "\n";
    }

    if (state.hasWinner(Player::A)) std::cout << "X wins!\n";
    else if (state.hasWinner(Player::B)) std::cout << "O wins!\n";
    else std::cout << "Draw.\n";

    return 0;
}
