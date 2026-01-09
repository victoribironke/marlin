/**
 * solver.hpp - Negamax Solver with Alpha-Beta Pruning
 * 
 * WHAT IS NEGAMAX?
 * =============================================================================
 * 
 * Negamax is a variant of the Minimax algorithm. In a two-player zero-sum game:
 *   - What's good for me is bad for my opponent (and vice versa)
 *   - The value of a position for player A = -(value for player B)
 * 
 * Instead of alternating between MAX and MIN players, Negamax always maximizes
 * from the current player's perspective. We just negate the score when we
 * recurse to the opponent's turn.
 * 
 * WHAT IS ALPHA-BETA PRUNING?
 * =============================================================================
 * 
 * Alpha-Beta is an optimization that SKIPS branches we don't need to explore.
 * 
 * Imagine you're evaluating move A, and you find it gives you a score of +5.
 * Now you're evaluating move B. You discover that no matter what you do in B,
 * your opponent can force a score of +3 (worse than A).
 * 
 * You can STOP exploring B immediately! You already have a better option (A).
 * 
 * - Alpha: The BEST score the current player is guaranteed (lower bound)
 * - Beta:  The WORST score the opponent will allow (upper bound)
 * 
 * If alpha >= beta, we can "prune" (skip) the rest of this branch.
 * 
 * MOVE ORDERING:
 * =============================================================================
 * 
 * Alpha-Beta works best when we find good moves FIRST. In Connect 4, center
 * columns are usually better (more winning opportunities), so we try them first.
 * 
 * Order: 4, 3, 5, 2, 6, 1, 7 (center to edges)
 * =============================================================================
 */

#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "position.hpp"
#include "transposition.hpp"

class Solver {
public:
    /**
     * solve - Find the game-theoretic value of a position.
     * 
     * @param pos   The position to analyze
     * @return      Score from current player's perspective
     *              Positive = current player can force a win
     *              Zero = draw with perfect play
     *              Negative = opponent can force a win
     */
    int solve(const Position& pos);

    /**
     * get_node_count - Return how many positions were analyzed.
     * Useful for benchmarking and understanding search depth.
     */
    unsigned long long get_node_count() const { return node_count_; }

    /**
     * reset_node_count - Reset the counter (call before each solve).
     */
    void reset_node_count() { node_count_ = 0; }

    // Move ordering: center columns first (better for alpha-beta pruning)
    // Column indices: 3, 2, 4, 1, 5, 0, 6 (center to edges, 0-indexed)
    static constexpr int COLUMN_ORDER[7] = {3, 2, 4, 1, 5, 0, 6};

private:
    /**
     * negamax - The core recursive search algorithm with alpha-beta pruning.
     * 
     * @param pos   Current position (passed by value so we can modify it)
     * @param alpha Lower bound - best score we're guaranteed
     * @param beta  Upper bound - worst score opponent will allow
     * @return      Score from current player's perspective
     */
    int negamax(Position pos, int alpha, int beta);

    // Counter for positions analyzed
    unsigned long long node_count_ = 0;

    // Transposition table for caching evaluated positions
    TranspositionTable tt_;
};

#endif // SOLVER_HPP
