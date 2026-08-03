#include "Connect4.hpp"
#include <random>
#include <sstream>

namespace sge {

namespace {

// Zobrist table: one random 64-bit number per (row, col, piece) triple,
// generated once with a fixed seed for reproducibility. A board's hash
// is the XOR of the entries for every occupied cell, plus a fixed
// constant when it's Player B's turn. XOR-ing/undoing a single cell's
// entry on applyMove/undoMove keeps the hash O(1) to maintain instead
// of O(board size) to recompute from scratch every node.
const std::array<std::array<std::array<uint64_t, 3>, Connect4State::COLS>, Connect4State::ROWS>&
zobristTable() {
    static const auto table = [] {
        std::array<std::array<std::array<uint64_t, 3>, Connect4State::COLS>, Connect4State::ROWS> t{};
        std::mt19937_64 rng(0xC0FFEEULL); // fixed seed -> reproducible hashes across runs
        for (auto& row : t)
            for (auto& cell : row)
                for (auto& piece : cell)
                    piece = rng();
        return t;
    }();
    return table;
}

uint64_t zobristSideToMove() {
    static const uint64_t v = std::mt19937_64(0xFACE)();
    return v;
}

} // namespace

Connect4State::Connect4State() {
    for (auto& row : board_) row.fill(0);
    zobristHash_ = 0; // Player::A to move, empty board
}

int Connect4State::dropRow(int col) const {
    for (int r = ROWS - 1; r >= 0; --r) {
        if (board_[r][col] == 0) return r;
    }
    return -1;
}

std::vector<Connect4Move> Connect4State::legalMovesImpl() const {
    std::vector<Connect4Move> moves;
    moves.reserve(COLS);
    for (int c = 0; c < COLS; ++c) {
        if (board_[0][c] == 0) moves.push_back(Connect4Move{c});
    }
    return moves;
}

void Connect4State::applyMoveImpl(const Connect4Move& m) {
    int r = dropRow(m.col);
    int8_t piece = (toMove_ == Player::A) ? 1 : 2;
    board_[r][m.col] = piece;
    zobristHash_ ^= zobristTable()[r][m.col][piece];
    zobristHash_ ^= zobristSideToMove();
    toMove_ = opponent(toMove_);
    ++movesPlayed_;
}

void Connect4State::undoMoveImpl(const Connect4Move& m) {
    for (int r = 0; r < ROWS; ++r) {
        if (board_[r][m.col] != 0) {
            int8_t piece = board_[r][m.col];
            zobristHash_ ^= zobristTable()[r][m.col][piece];
            zobristHash_ ^= zobristSideToMove();
            board_[r][m.col] = 0;
            break;
        }
    }
    toMove_ = opponent(toMove_);
    --movesPlayed_;
}

bool Connect4State::hasWinner(Player p) const {
    int8_t v = (p == Player::A) ? 1 : 2;
    static constexpr int DR[4] = {0, 1, 1, 1};
    static constexpr int DC[4] = {1, 0, 1, -1};
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (board_[r][c] != v) continue;
            for (int d = 0; d < 4; ++d) {
                int rr = r, cc = c, k = 0;
                for (; k < 4; ++k) {
                    if (rr < 0 || rr >= ROWS || cc < 0 || cc >= COLS || board_[rr][cc] != v) break;
                    rr += DR[d];
                    cc += DC[d];
                }
                if (k == 4) return true;
            }
        }
    }
    return false;
}

bool Connect4State::isFull() const {
    for (int c = 0; c < COLS; ++c)
        if (board_[0][c] == 0) return false;
    return true;
}

bool Connect4State::isTerminalImpl() const {
    return hasWinner(Player::A) || hasWinner(Player::B) || isFull();
}

int Connect4State::evaluateImpl(Player perspective) const {
    if (hasWinner(perspective)) return 100000 - movesPlayed_;      // prefer faster wins
    if (hasWinner(opponent(perspective))) return -100000 + movesPlayed_; // prefer slower losses
    return 0; // draw, or non-terminal depth cutoff (see README re: heuristic evaluation)
}

std::string Connect4State::toString() const {
    std::ostringstream out;
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            char ch = board_[r][c] == 1 ? 'X' : (board_[r][c] == 2 ? 'O' : '.');
            out << ch << ' ';
        }
        out << '\n';
    }
    return out.str();
}

} // namespace sge
