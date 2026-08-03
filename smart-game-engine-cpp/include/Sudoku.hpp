#pragma once
#include <array>
#include <bitset>
#include <vector>
#include <string>
#include <optional>

namespace sge {

// NOTE ON DESIGN: Sudoku is a single-agent Constraint Satisfaction
// Problem, not a two-player adversarial game, so it deliberately does
// NOT use AlphaBetaSearcher/GameState -- there is no opponent to
// minimize against, and "alpha-beta pruning" has no meaning without
// one. What it *does* share with the adversarial engines is the same
// underlying search paradigm: depth-first search over a state-space
// graph with pruning -- here the pruning is constraint propagation
// (ruling out branches that violate row/col/box constraints) rather
// than a minimax bound. See README.md for how to talk about this
// distinction in an interview.
class SudokuSolver {
public:
    static constexpr int N = 9;
    static constexpr int BOX = 3;

    // 0 = empty cell. Throws std::invalid_argument if the puzzle is
    // already inconsistent (e.g. duplicate digit in a row) before any
    // search even begins.
    explicit SudokuSolver(const std::array<std::array<int, N>, N>& puzzle);

    // Returns the solved board, or std::nullopt if unsolvable.
    // `nodesExplored` (optional out-param) reports how many cells
    // were assigned during search, for benchmarking/verification.
    std::optional<std::array<std::array<int, N>, N>> solve(long long* nodesExplored = nullptr);

    // Verifies a completed board fully satisfies every Sudoku constraint --
    // this is the "verified against the known-correct solution" step.
    static bool isValidSolution(const std::array<std::array<int, N>, N>& board);

    std::string toString(const std::array<std::array<int, N>, N>& board) const;

private:
    std::array<std::array<int, N>, N> board_;

    // Bitset-per-unit constraint tracking: bit d set means digit (d+1)
    // is already used in that row/column/box. This turns the legality
    // check from an O(N) linear scan (as in the original triple-loop
    // `ok()`) into an O(1) bit test, and the "un-assign on backtrack"
    // step into an O(1) bit clear instead of nothing to undo at all.
    std::array<std::bitset<N>, N> rowUsed_;
    std::array<std::bitset<N>, N> colUsed_;
    std::array<std::bitset<N>, N> boxUsed_;

    static int boxIndex(int row, int col) { return (row / BOX) * BOX + (col / BOX); }

    bool place(int row, int col, int digit);   // returns false if it violates a constraint
    void remove(int row, int col, int digit);  // backtrack/undo

    // Finds the empty cell with the fewest legal candidates remaining
    // ("minimum remaining values" heuristic) instead of always
    // scanning left-to-right/top-to-bottom -- this is the single
    // biggest practical speedup over naive backtracking, since it
    // fails fast on the most constrained cells first.
    bool findMostConstrainedCell(int& outRow, int& outCol) const;

    bool backtrack(long long& nodesExplored);
};

} // namespace sge
