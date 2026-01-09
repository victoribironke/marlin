/**
 * solver.hpp - Negamax Solver for Connect 4
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
 * THE ALGORITHM:
 * =============================================================================
 * 
 * negamax(position):
 *     if game is over:
 *         return score (positive = current player wins)
 *     
 *     best = -infinity
 *     for each possible move:
 *         make_move()
 *         score = -negamax(new_position)  // Negate! Opponent's loss = our gain
 *         best = max(best, score)
 *     return best
 * 
 * SCORING CONVENTION:
 * =============================================================================
 * 
 * We use a scoring system based on "how many moves until win":
 *   - Win in 1 move:  +21 (best possible - win immediately)
 *   - Win in 3 moves: +20
 *   - Win in N moves: +(22 - N)
 *   - Draw:           0
 *   - Lose in N moves: -(22 - N)
 * 
 * This way, the solver prefers FASTER wins and SLOWER losses.
 */

#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "position.hpp"

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

private:
    /**
     * negamax - The core recursive search algorithm.
     * 
     * @param pos   Current position (passed by value so we can modify it)
     * @return      Score from current player's perspective
     */
    int negamax(Position pos);

    // Counter for positions analyzed
    unsigned long long node_count_ = 0;
};

#endif // SOLVER_HPP
