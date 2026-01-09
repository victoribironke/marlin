/**
 * solver.cpp - Negamax Solver Implementation
 */

#include "solver.hpp"

/**
 * solve - Public entry point for the solver.
 * 
 * Resets the node counter and calls negamax.
 */
int Solver::solve(const Position& pos) {
    reset_node_count();
    return negamax(pos);
}

/**
 * negamax - The core recursive algorithm.
 * 
 * This explores ALL possible game continuations (brute force for now).
 * Later we'll add alpha-beta pruning to make it much faster.
 */
int Solver::negamax(Position pos) {
    node_count_++;

    // -------------------------------------------------------------------------
    // BASE CASE 1: Check if current player can win immediately
    // -------------------------------------------------------------------------
    for (int col = 0; col < Position::WIDTH; col++) {
        if (pos.can_play(col) && pos.is_winning_move(col)) {
            // Current player wins! Return a high positive score.
            // The score is based on how many moves are left in the game.
            // More moves left = faster win = higher score.
            // Formula: (total_cells - moves_played + 1) / 2
            // This gives us a value that decreases as the game goes on.
            return (Position::WIDTH * Position::HEIGHT + 1 - pos.nb_moves()) / 2;
        }
    }

    // -------------------------------------------------------------------------
    // BASE CASE 2: Check for draw (all columns full)
    // -------------------------------------------------------------------------
    if (pos.nb_moves() == Position::WIDTH * Position::HEIGHT) {
        return 0;  // Draw
    }

    // -------------------------------------------------------------------------
    // RECURSIVE CASE: Try all possible moves
    // -------------------------------------------------------------------------
    int best_score = -1000;  // Start with worst possible score

    for (int col = 0; col < Position::WIDTH; col++) {
        if (pos.can_play(col)) {
            // Create a copy of the position and make the move
            Position next = pos;
            next.make_move(col);

            // Recursively evaluate the position from opponent's perspective
            // The key insight: opponent's score = -our score
            int score = -negamax(next);

            // Keep track of the best score we can achieve
            if (score > best_score) {
                best_score = score;
            }
        }
    }

    return best_score;
}
