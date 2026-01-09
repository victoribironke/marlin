/**
 * solver.cpp - Negamax Solver with Alpha-Beta Pruning and Transposition Table
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
 * negamax - The core recursive algorithm with alpha-beta pruning and TT.
 * 
 * TRANSPOSITION TABLE INTEGRATION:
 * At the start, we check if this position was already evaluated.
 * If so, we can use the cached value to narrow our bounds.
 * At the end, we store the result for future lookups.
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
    // TRANSPOSITION TABLE LOOKUP
    // -------------------------------------------------------------------------
    // Check if we've seen this position before. If the cached value gives us
    // useful information, we can narrow our search bounds or return early.
    uint64_t key = pos.key();
    int8_t cached_value = tt_.get(key);
    
    if (cached_value != 0) {
        // We found a cached upper bound for this position
        // The cached value is an upper bound on the true score
        int max_from_cache = cached_value + Position::WIDTH * Position::HEIGHT / 2 - 1;
        if (beta > max_from_cache) {
            beta = max_from_cache;
            if (alpha >= beta) return beta;  // Prune with cached info!
        }
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

    // -------------------------------------------------------------------------
    // TRANSPOSITION TABLE STORE
    // -------------------------------------------------------------------------
    // Store the result. We convert alpha to a relative value.
    // This helps future searches prune earlier.
    tt_.put(key, static_cast<int8_t>(alpha - Position::WIDTH * Position::HEIGHT / 2 + 1));

    return alpha;
}
