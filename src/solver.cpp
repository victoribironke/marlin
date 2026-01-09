/**
 * solver.cpp - Negamax Solver with Alpha-Beta Pruning
 */

#include "solver.hpp"

// Define the static member (required for linking)
constexpr int Solver::COLUMN_ORDER[7];

/**
 * solve - Public entry point for the solver.
 * 
 * We initialize alpha to the worst possible score (we're losing as fast as possible)
 * and beta to the best possible score (we're winning as fast as possible).
 * These bounds will tighten as we explore the game tree.
 */
int Solver::solve(const Position& pos) {
    reset_node_count();
    
    // Initial bounds:
    // alpha = can't be worse than losing on the very last possible move
    // beta = can't be better than winning on the next move
    int alpha = -(Position::WIDTH * Position::HEIGHT) / 2;
    int beta = (Position::WIDTH * Position::HEIGHT + 1) / 2;
    
    return negamax(pos, alpha, beta);
}

/**
 * negamax - The core recursive algorithm with alpha-beta pruning.
 * 
 * Alpha-beta pruning dramatically reduces the number of positions we need to
 * examine. When we find a move that's "too good" (would be rejected by the
 * opponent), we can stop searching that branch entirely.
 */
int Solver::negamax(Position pos, int alpha, int beta) {
    node_count_++;

    // -------------------------------------------------------------------------
    // BASE CASE 1: Check if current player can win immediately
    // -------------------------------------------------------------------------
    for (int i = 0; i < Position::WIDTH; i++) {
        int col = COLUMN_ORDER[i];  // Check center columns first
        if (pos.can_play(col) && pos.is_winning_move(col)) {
            // Current player wins! Score based on how fast we win.
            return (Position::WIDTH * Position::HEIGHT + 1 - pos.nb_moves()) / 2;
        }
    }

    // -------------------------------------------------------------------------
    // BASE CASE 2: Check for draw (no moves left = all 42 cells filled)
    // -------------------------------------------------------------------------
    if (pos.nb_moves() >= Position::WIDTH * Position::HEIGHT - 1) {
        return 0;  // Draw (or about to be)
    }

    // -------------------------------------------------------------------------
    // OPTIMIZATION: Tighten the upper bound (beta)
    // -------------------------------------------------------------------------
    // We can't do better than winning in 2 moves (since we already checked
    // for immediate wins above). This helps prune more branches.
    int max_possible = (Position::WIDTH * Position::HEIGHT - 1 - pos.nb_moves()) / 2;
    if (beta > max_possible) {
        beta = max_possible;
        if (alpha >= beta) return beta;  // Prune!
    }

    // -------------------------------------------------------------------------
    // RECURSIVE CASE: Try all moves with alpha-beta pruning
    // -------------------------------------------------------------------------
    for (int i = 0; i < Position::WIDTH; i++) {
        int col = COLUMN_ORDER[i];  // Try center columns first (move ordering)
        
        if (pos.can_play(col)) {
            // Create a copy and make the move
            Position next = pos;
            next.make_move(col);

            // Recursively evaluate from opponent's perspective
            // Note: We negate and swap alpha/beta bounds
            int score = -negamax(next, -beta, -alpha);

            // ALPHA-BETA PRUNING CHECK
            if (score >= beta) {
                // This move is "too good" - opponent won't allow this line
                // We can stop searching this branch entirely!
                return score;  // Fail-high (beta cutoff)
            }

            // Update alpha (our guaranteed minimum score)
            if (score > alpha) {
                alpha = score;
            }
        }
    }

    return alpha;
}
