#include "Sudoku.hpp"
#include <cassert>
#include <iostream>

using namespace sge;

int main() {
    // Well-known easy puzzle with a well-known unique solution.
    std::array<std::array<int, 9>, 9> puzzle = {{
        {5,3,0, 0,7,0, 0,0,0},
        {6,0,0, 1,9,5, 0,0,0},
        {0,9,8, 0,0,0, 0,6,0},

        {8,0,0, 0,6,0, 0,0,3},
        {4,0,0, 8,0,3, 0,0,1},
        {7,0,0, 0,2,0, 0,0,6},

        {0,6,0, 0,0,0, 2,8,0},
        {0,0,0, 4,1,9, 0,0,5},
        {0,0,0, 0,8,0, 0,7,9}
    }};

    std::array<std::array<int, 9>, 9> expected = {{
        {5,3,4, 6,7,8, 9,1,2},
        {6,7,2, 1,9,5, 3,4,8},
        {1,9,8, 3,4,2, 5,6,7},

        {8,5,9, 7,6,1, 4,2,3},
        {4,2,6, 8,5,3, 7,9,1},
        {7,1,3, 9,2,4, 8,5,6},

        {9,6,1, 5,3,7, 2,8,4},
        {2,8,7, 4,1,9, 6,3,5},
        {3,4,5, 2,8,6, 1,7,9}
    }};

    SudokuSolver solver(puzzle);
    long long nodes = 0;
    auto solution = solver.solve(&nodes);

    assert(solution.has_value());
    assert(SudokuSolver::isValidSolution(*solution));
    assert(*solution == expected);

    std::cout << "PASSED: Sudoku solver matches the known unique solution\n";
    std::cout << "Cells assigned during search: " << nodes << "\n";

    // Unsolvable puzzle: two 5s in the same row -> constructor should throw.
    bool threw = false;
    try {
        std::array<std::array<int, 9>, 9> bad{};
        bad[0][0] = 5;
        bad[0][1] = 5;
        SudokuSolver badSolver(bad);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
    std::cout << "PASSED: inconsistent puzzle correctly rejected\n";

    std::cout << "All Sudoku tests passed.\n";
    return 0;
}
