#include "TicTacToe.hpp"
#include <sstream>
#include <cmath>

namespace sge {

namespace {
// 3^(row*SIZE + col): the "place value" of a cell in the base-3 encoding.
uint64_t placeValue(int row, int col) {
    static const auto powers = [] {
        std::array<uint64_t, TicTacToeState::SIZE * TicTacToeState::SIZE> p{};
        uint64_t v = 1;
        for (auto& x : p) { x = v; v *= 3; }
        return p;
    }();
    return powers[row * TicTacToeState::SIZE + col];
}
} // namespace

TicTacToeState::TicTacToeState() {
    for (auto& row : board_) row.fill(0);
}

std::vector<TicTacToeMove> TicTacToeState::legalMovesImpl() const {
    std::vector<TicTacToeMove> moves;
    moves.reserve(SIZE * SIZE);
    for (int r = 0; r < SIZE; ++r)
        for (int c = 0; c < SIZE; ++c)
            if (board_[r][c] == 0) moves.push_back(TicTacToeMove{r, c});
    return moves;
}

void TicTacToeState::applyMoveImpl(const TicTacToeMove& m) {
    int8_t piece = (toMove_ == Player::A) ? 1 : 2;
    board_[m.row][m.col] = piece;
    code_ += static_cast<uint64_t>(piece) * placeValue(m.row, m.col);
    toMove_ = opponent(toMove_);
    ++movesPlayed_;
}

void TicTacToeState::undoMoveImpl(const TicTacToeMove& m) {
    int8_t piece = board_[m.row][m.col];
    code_ -= static_cast<uint64_t>(piece) * placeValue(m.row, m.col);
    board_[m.row][m.col] = 0;
    toMove_ = opponent(toMove_);
    --movesPlayed_;
}

bool TicTacToeState::hasWinner(Player p) const {
    int8_t v = (p == Player::A) ? 1 : 2;

    for (int r = 0; r < SIZE; ++r) {
        bool win = true;
        for (int c = 0; c < SIZE; ++c) win &= (board_[r][c] == v);
        if (win) return true;
    }
    for (int c = 0; c < SIZE; ++c) {
        bool win = true;
        for (int r = 0; r < SIZE; ++r) win &= (board_[r][c] == v);
        if (win) return true;
    }
    bool diag1 = true, diag2 = true;
    for (int i = 0; i < SIZE; ++i) {
        diag1 &= (board_[i][i] == v);
        diag2 &= (board_[i][SIZE - 1 - i] == v);
    }
    return diag1 || diag2;
}

bool TicTacToeState::isFull() const {
    for (auto& row : board_)
        for (auto cell : row)
            if (cell == 0) return false;
    return true;
}

bool TicTacToeState::isTerminalImpl() const {
    return hasWinner(Player::A) || hasWinner(Player::B) || isFull();
}

int TicTacToeState::evaluateImpl(Player perspective) const {
    if (hasWinner(perspective)) return 1000 - movesPlayed_;
    if (hasWinner(opponent(perspective))) return -1000 + movesPlayed_;
    return 0;
}

std::string TicTacToeState::toString() const {
    std::ostringstream out;
    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            char ch = board_[r][c] == 1 ? 'X' : (board_[r][c] == 2 ? 'O' : '.');
            out << ch << ' ';
        }
        out << '\n';
    }
    return out.str();
}

} // namespace sge
