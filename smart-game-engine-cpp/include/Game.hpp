#pragma once
#include <vector>
#include <cstdint>

namespace sge {

// Player is intentionally +1 / -1 so a Negamax-style search can flip
// perspective with a simple sign flip instead of branching on identity.
enum class Player : int8_t { A = 1, B = -1 };

inline Player opponent(Player p) {
    return p == Player::A ? Player::B : Player::A;
}

// Generic two-player adversarial game-state interface, implemented via
// CRTP (Curiously Recurring Template Pattern) rather than virtual
// dispatch. Every concrete game (Connect4State, TicTacToeState, ...)
// derives from GameState<ConcreteGame, MoveType> and implements the
// *Impl methods below. This is what lets AlphaBetaSearch.hpp be a
// single reusable, modular search engine shared across every game
// instead of duplicating minimax per file, while keeping the search
// loop free of virtual-call overhead (important when you're trying to
// stay inside a tight per-move time budget).
template <typename Derived, typename MoveT>
class GameState {
public:
    using Move = MoveT;

    std::vector<Move> legalMoves() const {
        return static_cast<const Derived*>(this)->legalMovesImpl();
    }
    void applyMove(const Move& m) {
        static_cast<Derived*>(this)->applyMoveImpl(m);
    }
    void undoMove(const Move& m) {
        static_cast<Derived*>(this)->undoMoveImpl(m);
    }
    bool isTerminal() const {
        return static_cast<const Derived*>(this)->isTerminalImpl();
    }
    // Score from `perspective`'s point of view. Large positive = great
    // for `perspective`, large negative = terrible for `perspective`.
    int evaluate(Player perspective) const {
        return static_cast<const Derived*>(this)->evaluateImpl(perspective);
    }
    Player currentPlayer() const {
        return static_cast<const Derived*>(this)->currentPlayerImpl();
    }
    // Incremental Zobrist-style hash identifying this exact state, used
    // as the key into the transposition table (see TranspositionTable.hpp).
    uint64_t hash() const {
        return static_cast<const Derived*>(this)->hashImpl();
    }
};

} // namespace sge
