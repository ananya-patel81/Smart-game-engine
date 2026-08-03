#pragma once
#include "Game.hpp"
#include <array>
#include <vector>
#include <cstdint>
#include <string>

namespace sge {

struct Connect4Move {
    int col = 0;
    bool operator==(const Connect4Move& o) const { return col == o.col; }
};

// Standard 6-row x 7-column Connect4.
class Connect4State : public GameState<Connect4State, Connect4Move> {
public:
    static constexpr int ROWS = 6;
    static constexpr int COLS = 7;

    Connect4State();

    std::vector<Connect4Move> legalMovesImpl() const;
    void applyMoveImpl(const Connect4Move& m);
    void undoMoveImpl(const Connect4Move& m);
    bool isTerminalImpl() const;
    int evaluateImpl(Player perspective) const;
    Player currentPlayerImpl() const { return toMove_; }
    uint64_t hashImpl() const { return zobristHash_; }

    bool hasWinner(Player p) const;
    bool isFull() const;
    int movesPlayed() const { return movesPlayed_; }
    std::string toString() const;

private:
    // 0 = empty, 1 = Player::A, 2 = Player::B (index into the Zobrist table)
    std::array<std::array<int8_t, COLS>, ROWS> board_;
    Player toMove_ = Player::A;
    int movesPlayed_ = 0;
    uint64_t zobristHash_ = 0;

    int dropRow(int col) const; // lowest empty row in a column, -1 if full
};

} // namespace sge
