#include "Sudoku.hpp"
#include <iostream>

using namespace sge;

int main() {
    std::array<std::array<int, 9>, 9> puzzle{};
    std::cout << "Enter 81 digits (0 for blank), row by row:\n";
    for (auto& row : puzzle)
        for (auto& cell : row)
            std::cin >> cell;

    try {
        SudokuSolver solver(puzzle);
        long long nodes = 0;
        auto solution = solver.solve(&nodes);

        if (!solution) {
            std::cout << "No solution exists.\n";
            return 0;
        }

        std::cout << solver.toString(*solution);
        std::cout << "Cells assigned during search: " << nodes << "\n";
        std::cout << "Valid solution: "
                  << (SudokuSolver::isValidSolution(*solution) ? "yes" : "no") << "\n";
    } catch (const std::invalid_argument& e) {
        std::cout << "Invalid puzzle: " << e.what() << "\n";
    }

    return 0;
}
