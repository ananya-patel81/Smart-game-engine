#include "Sudoku.hpp"
#include <stdexcept>
#include <sstream>
#include <limits>

namespace sge {

SudokuSolver::SudokuSolver(const std::array<std::array<int, SudokuSolver::N>, SudokuSolver::N>& puzzle) : board_{} {
    for (auto& r : rowUsed_) r.reset();
    for (auto& c : colUsed_) c.reset();
    for (auto& b : boxUsed_) b.reset();

    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            int d = puzzle[r][c];
            if (d == 0) continue;
            if (d < 1 || d > 9 || !place(r, c, d)) {
                throw std::invalid_argument("Inconsistent Sudoku puzzle at cell (" +
                                             std::to_string(r) + "," + std::to_string(c) + ")");
            }
        }
    }
}

bool SudokuSolver::place(int row, int col, int digit) {
    int bit = digit - 1;
    int box = boxIndex(row, col);
    if (rowUsed_[row][bit] || colUsed_[col][bit] || boxUsed_[box][bit]) return false;
    rowUsed_[row].set(bit);
    colUsed_[col].set(bit);
    boxUsed_[box].set(bit);
    board_[row][col] = digit;
    return true;
}

void SudokuSolver::remove(int row, int col, int digit) {
    int bit = digit - 1;
    int box = boxIndex(row, col);
    rowUsed_[row].reset(bit);
    colUsed_[col].reset(bit);
    boxUsed_[box].reset(bit);
    board_[row][col] = 0;
}

bool SudokuSolver::findMostConstrainedCell(int& outRow, int& outCol) const {
    int bestCount = std::numeric_limits<int>::max();
    bool found = false;
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            if (board_[r][c] != 0) continue;
            int box = boxIndex(r, c);
            auto used = rowUsed_[r] | colUsed_[c] | boxUsed_[box];
            int candidates = N - static_cast<int>(used.count());
            if (candidates < bestCount) {
                bestCount = candidates;
                outRow = r;
                outCol = c;
                found = true;
                if (bestCount == 0) return true; // dead end, bail out immediately
            }
        }
    }
    return found;
}

bool SudokuSolver::backtrack(long long& nodesExplored) {
    int row, col;
    if (!findMostConstrainedCell(row, col)) {
        return true; // no empty cells left -> solved
    }

    int box = boxIndex(row, col);
    auto used = rowUsed_[row] | colUsed_[col] | boxUsed_[box];

    for (int digit = 1; digit <= N; ++digit) {
        if (used[digit - 1]) continue; // constraint-propagation prune: skip known-illegal digits
        ++nodesExplored;
        place(row, col, digit);
        if (backtrack(nodesExplored)) return true;
        remove(row, col, digit);
    }
    return false;
}

std::optional<std::array<std::array<int, SudokuSolver::N>, SudokuSolver::N>> SudokuSolver::solve(long long* nodesExplored) {
    long long nodes = 0;
    bool solved = backtrack(nodes);
    if (nodesExplored) *nodesExplored = nodes;
    if (!solved) return std::nullopt;
    return board_;
}

bool SudokuSolver::isValidSolution(const std::array<std::array<int, SudokuSolver::N>, SudokuSolver::N>& board) {
    for (int r = 0; r < N; ++r) {
        std::bitset<N> seen;
        for (int c = 0; c < N; ++c) {
            int d = board[r][c];
            if (d < 1 || d > 9 || seen[d - 1]) return false;
            seen.set(d - 1);
        }
    }
    for (int c = 0; c < N; ++c) {
        std::bitset<N> seen;
        for (int r = 0; r < N; ++r) {
            int d = board[r][c];
            if (seen[d - 1]) return false;
            seen.set(d - 1);
        }
    }
    for (int br = 0; br < N; br += BOX) {
        for (int bc = 0; bc < N; bc += BOX) {
            std::bitset<N> seen;
            for (int r = br; r < br + BOX; ++r)
                for (int c = bc; c < bc + BOX; ++c) {
                    int d = board[r][c];
                    if (seen[d - 1]) return false;
                    seen.set(d - 1);
                }
        }
    }
    return true;
}

std::string SudokuSolver::toString(const std::array<std::array<int, SudokuSolver::N>, SudokuSolver::N>& board) const {
    std::ostringstream out;
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) out << board[r][c] << ' ';
        out << '\n';
    }
    return out.str();
}

} // namespace sge
