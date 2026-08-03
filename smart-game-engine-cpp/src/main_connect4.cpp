#include "Connect4.hpp"
#include "AlphaBetaSearch.hpp"
#include <iostream>
#include <chrono>

using namespace sge;

int main() {
    Connect4State state;
    AlphaBetaSearcher<Connect4State> engine(/*maxDepth=*/12, std::chrono::milliseconds(90));

    char input;
    std::cout << "Which color should the engine play? (X/O): ";
    std::cin >> input;
    Player enginePlayer = (input == 'X' || input == 'x') ? Player::A : Player::B;

    std::cout << state.toString() << "\n";

    while (!state.isTerminalImpl()) {
        if (state.currentPlayerImpl() == enginePlayer) {
            auto t0 = std::chrono::steady_clock::now();
            auto result = engine.search(state, enginePlayer);
            auto elapsedMs = std::chrono::duration<double, std::milli>(
                                  std::chrono::steady_clock::now() - t0)
                                  .count();

            state.applyMoveImpl(result.bestMove);
            std::cout << "Engine plays column " << result.bestMove.col
                      << " | depth=" << result.depthReached
                      << " | nodes=" << result.nodesVisited
                      << " | tt_entries=" << engine.lastTranspositionTableSize()
                      << " | time=" << elapsedMs << "ms\n";
            std::cout << state.toString() << "\n";
        } else {
            int col;
            std::cout << "Your move (column 0-6): ";
            std::cin >> col;
            state.applyMoveImpl(Connect4Move{col});
            std::cout << state.toString() << "\n";
        }
    }

    if (state.hasWinner(Player::A)) std::cout << "X wins!\n";
    else if (state.hasWinner(Player::B)) std::cout << "O wins!\n";
    else std::cout << "Draw.\n";

    return 0;
}
