#pragma once
#include <unordered_map>
#include <optional>
#include <cstdint>

namespace sge {

// The state space explored by minimax is a graph: nodes are board
// states, edges are legal moves. The recursion in AlphaBetaSearch.hpp
// walks that graph implicitly (each recursive call = following one
// edge), but two different move orders can reach the *same* node
// (transposition) — e.g. dropping in column 2 then 4 reaches the same
// board as dropping in 4 then 2. TranspositionTable is the explicit
// data structure that turns the implicit tree traversal into a proper
// graph traversal by memoizing results per unique node (state hash),
// so a transposed state is looked up in O(1) instead of re-searched.
template <typename ValueT>
class TranspositionTable {
public:
    struct Entry {
        int depth;      // search depth this value was computed at
        ValueT value;
    };

    std::optional<ValueT> lookup(uint64_t nodeKey, int minDepth) const {
        auto it = table_.find(nodeKey);
        if (it != table_.end() && it->second.depth >= minDepth) {
            return it->second.value;
        }
        return std::nullopt;
    }

    void store(uint64_t nodeKey, int depth, ValueT value) {
        auto it = table_.find(nodeKey);
        if (it == table_.end() || it->second.depth <= depth) {
            table_[nodeKey] = Entry{depth, value};
        }
    }

    size_t nodeCount() const { return table_.size(); }
    void clear() { table_.clear(); }

private:
    std::unordered_map<uint64_t, Entry> table_;
};

} // namespace sge
