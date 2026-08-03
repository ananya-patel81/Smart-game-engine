#pragma once
#include "Game.hpp"
#include <array>
#include <vector>
#include <cstdint>
#include <string>

namespace sge {

struct TicTacToeMove {
    int row = 0, col = 0;
    bool operator==(const TicTacToeMove& o) const { return row == o.row && col == o.col; }
};

// 4x4 variant (matches the original repo): win = 4 in a row/col/diagonal.
class TicTacToeState : public GameState<TicTacToeState, TicTacToeMove> {
public:
    static constexpr int SIZE = 4;

    TicTacToeState();

    std::vector<TicTacToeMove> legalMovesImpl() const;
    void applyMoveImpl(const TicTacToeMove& m);
    void undoMoveImpl(const TicTacToeMove& m);
    bool isTerminalImpl() const;
    int evaluateImpl(Player perspective) const;
    Player currentPlayerImpl() const { return toMove_; }
    // Base-3 encoding of the board (each cell in {empty, A, B}) -- same
    // idea as the original tictactoe.c's `ord()`, just as a member here
    // instead of a free function keyed into a giant global array.
    uint64_t hashImpl() const { return code_; }

    bool hasWinner(Player p) const;
    bool isFull() const;
    int movesPlayed() const { return movesPlayed_; }
    std::string toString() const;

private:
    std::array<std::array<int8_t, SIZE>, SIZE> board_; // 0 empty, 1 = A, 2 = B
    Player toMove_ = Player::A;
    int movesPlayed_ = 0;
    uint64_t code_ = 0; // incremental base-3 board encoding
};

} // namespace sge
