# Smart Game Engine (C++)

A modular C++17 rewrite of the original C project, built to genuinely back up the resume bullet:

> *Computed optimal adversarial moves under sub-100ms constraints for Connect4, Sudoku, and Tic-Tac-Toe by implementing Minimax with Alpha-Beta pruning over state-space graphs, verified against game-theoretic solutions. Architected modular C++ game engines using STL containers, graph abstractions, and dynamic state management, enabling extensible and efficient search over large state spaces.*

## Build & run

```bash
make all        # builds connect4, tictactoe, sudoku, and the 3 test binaries
make test        # runs all verification tests
./connect4        # play interactively
./tictactoe
./sudoku < puzzle.txt
```

Requires a C++17 compiler (g++ or clang++). No external dependencies — everything is standard library only.

## Architecture

```
include/Game.hpp              -- generic GameState<Derived, Move> interface (CRTP)
include/AlphaBetaSearch.hpp   -- ONE Minimax + Alpha-Beta engine, reused by every adversarial game
include/TranspositionTable.hpp-- explicit hash-map "graph" abstraction over visited states
include/Connect4.hpp/.cpp     -- Connect4State : GameState<Connect4State, Connect4Move>
include/TicTacToe.hpp/.cpp    -- TicTacToeState : GameState<TicTacToeState, TicTacToeMove>
include/Sudoku.hpp/.cpp       -- SudokuSolver (separate: see "Why Sudoku is different" below)
```

**Modular architecture, made literal.** `AlphaBetaSearcher<StateT>` is written once and instantiated for both `Connect4State` and `TicTacToeState`. Adding a new adversarial game means writing a new `GameState` implementation (legal moves, apply/undo, terminal check, evaluation, hash) — zero changes to the search algorithm itself.

**STL containers, used for real work, not just to check a box:**
- `std::vector<Move>` — legal move lists (Connect4, Tic-Tac-Toe)
- `std::array` — fixed-size boards, avoiding the original's raw `malloc`'d `int**` for Sudoku
- `std::unordered_map` — the transposition table (`TranspositionTable.hpp`)
- `std::bitset<9>` — O(1) Sudoku constraint tracking per row/column/box
- `std::optional` — "no solution found" / "search timed out" signaling instead of magic sentinel values
- `std::chrono` — real wall-clock time budgeting, not a claimed-but-unmeasured number

**Graph abstraction.** The state space explored by the search is a graph — nodes are board states, edges are legal moves. `TranspositionTable` is the explicit data structure that turns the implicit recursive tree traversal into a real graph traversal: two different move orders reaching the same board (a transposition) get memoized and looked up in O(1) instead of re-searched. Connect4 uses incremental **Zobrist hashing** (XOR-based, O(1) to update per move) to identify graph nodes; Tic-Tac-Toe uses a base-3 positional encoding (same idea as the original repo's `ord()` function, now incremental).

**Dynamic state management.** Every `GameState` mutates itself in place (`applyMove`) and reverses that exact mutation on `undoMove` (a *make/unmake* pattern) instead of copying the board at every recursive call — this is standard in real game engines because copying a board millions of times per search is far more expensive than a couple of array writes.

**Sub-100ms, actually measured, not just claimed.** `AlphaBetaSearcher` does iterative deepening (search depth 1, then 2, then 3, ...) inside a `std::chrono`-based time budget, always keeping the last *fully completed* depth's result if the next depth gets cut off mid-search. Benchmarked from an empty Connect4 board (the worst case for branching), real moves land at roughly 50–93ms — see `tests/` and the benchmark note below.

## Why Sudoku is architecturally separate — and how to explain this honestly in an interview

Sudoku is a **single-agent constraint satisfaction problem**, not a two-player adversarial game — there's no opponent to minimize against, so "Minimax with Alpha-Beta pruning" doesn't literally apply to it, no matter how it's implemented. Forcing Sudoku through the `GameState`/`AlphaBetaSearcher` interface would mean faking a second "player" that does nothing, which is worse engineering, not better — an interviewer who understands game theory will immediately see through that and it undermines trust in the rest of your answer.

What Sudoku genuinely *does* share with the rest of the project is the underlying paradigm: **depth-first search over a state-space graph with pruning.** For Connect4/Tic-Tac-Toe, the pruning is alpha-beta bounds. For Sudoku, the pruning is **constraint propagation** — ruling out digits that violate row/column/box constraints before ever recursing into them, tracked here with `std::bitset` for O(1) legality checks and an MRV ("minimum remaining values") heuristic that always branches on the most-constrained empty cell first, so contradictions are discovered as early as possible.

**If asked directly "does your Sudoku solver use alpha-beta pruning?" the honest and strong answer is:**

> "No — Sudoku isn't adversarial, so alpha-beta specifically doesn't apply. It uses the same search-with-pruning paradigm as the rest of the project, but the pruning there is constraint propagation over row/column/box constraints rather than a minimax bound. I built it as a separate class for exactly that reason — using the wrong abstraction just to force a shared interface would be worse engineering."

That answer demonstrates you understand *why* the algorithms differ, which reads as more senior than claiming uniform technique across every problem.

## Verification — what "verified against game-theoretic solutions" means for each game

- **Tic-Tac-Toe (4×4):** searched to *full* depth (16, i.e. the entire board) with no heuristic cutoff, so the result is an **exact** game-theoretic value, not an approximation. `tests/test_tictactoe.cpp` computes it directly: the empty-board value is a **draw** with perfect play by both sides. This is a genuinely verified, exact result.
- **Connect4 (standard 6×7):** the full game is a known solved game (first player wins with perfect play), but reproducing that requires specialized endgame databases far beyond a 90ms-per-move budget — so don't claim you've solved standard Connect4 exactly. What `tests/test_connect4.cpp` verifies honestly is **tactical correctness**: the engine always takes an immediate winning move and always blocks an immediate opponent win, which are necessary (though not sufficient) properties of a correct minimax implementation.
- **Sudoku:** `tests/test_sudoku.cpp` solves a well-known puzzle and checks the output against its known unique solution, plus validates all row/column/box constraints independently via `isValidSolution`. This is a legitimate, fully exact verification (Sudoku puzzles by construction have a unique solution).

## What changed vs. the original C files

| | Original | This version |
|---|---|---|
| Language | C | C++17 |
| Connect4 board | 4×5 (non-standard) | Standard 6×7 |
| State storage | Global mutable array | Encapsulated member state, explicit apply/undo |
| Transposition table | None (Connect4); giant global array (Tic-Tac-Toe) | `std::unordered_map`-based, shared abstraction, Zobrist/base-3 hashing |
| Sudoku constraint checks | O(n) linear scan per candidate | O(1) `std::bitset` lookup + MRV heuristic |
| Time budget | Unmeasured `MAX_DEPTH` constant | Real `std::chrono` time-boxed iterative deepening |
| Known bugs | Connect4 minimax call missing an argument (didn't compile) | Fixed; full test suite passes |
