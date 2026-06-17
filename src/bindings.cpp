/**
 * bindings.cpp - Emscripten WASM bindings for the Marlin Connect 4 solver.
 *
 * Exposes a C-style API that JavaScript can call via Module.cwrap().
 * Uses a global Position and Solver with a 1M-entry transposition table.
 */

#include "position.hpp"
#include "solver.hpp"
#include <emscripten.h>

static Position g_pos;
static Solver g_solver(1048576);  // 1M entries (~16 MB) for browser
static int g_last_best_score = 0;

extern "C" {

EMSCRIPTEN_KEEPALIVE
void marlin_reset() {
    g_pos = Position();
    g_solver.reset();
}

EMSCRIPTEN_KEEPALIVE
int marlin_make_move(int col) {
    if (col < 0 || col >= Position::WIDTH || !g_pos.can_play(col)) return 0;
    g_pos.make_move(col);
    return 1;
}

EMSCRIPTEN_KEEPALIVE
int marlin_can_play(int col) {
    return g_pos.can_play(col) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int marlin_is_winning_move(int col) {
    if (col < 0 || col >= Position::WIDTH || !g_pos.can_play(col)) return 0;
    return g_pos.is_winning_move(col) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int marlin_get_cell(int col, int row) {
    // Returns: 0 = empty, 1 = player 1 (first mover), 2 = player 2
    int bit = col * (Position::HEIGHT + 1) + row;
    uint64_t cell = 1ULL << bit;

    if (!(g_pos.get_mask() & cell)) return 0;

    // position_ stores the CURRENT player's pieces.
    // When nb_moves() is even, current player is P1, so position_ = P1's pieces.
    // When nb_moves() is odd, current player is P2, so position_ = P2's pieces.
    uint64_t p1_pieces, p2_pieces;
    if (g_pos.nb_moves() % 2 == 0) {
        p1_pieces = g_pos.get_position();
        p2_pieces = g_pos.get_mask() ^ g_pos.get_position();
    } else {
        p2_pieces = g_pos.get_position();
        p1_pieces = g_pos.get_mask() ^ g_pos.get_position();
    }

    if (p1_pieces & cell) return 1;
    if (p2_pieces & cell) return 2;
    return 0;
}

EMSCRIPTEN_KEEPALIVE
int marlin_nb_moves() {
    return g_pos.nb_moves();
}

EMSCRIPTEN_KEEPALIVE
int marlin_get_best_move(int max_depth) {
    // Check for immediate wins first (any difficulty)
    for (int i = 0; i < Position::WIDTH; i++) {
        int col = Solver::COLUMN_ORDER[i];
        if (g_pos.can_play(col) && g_pos.is_winning_move(col)) {
            g_last_best_score = (Position::WIDTH * Position::HEIGHT + 1 - g_pos.nb_moves()) / 2;
            return col;
        }
    }

    // Check if game is over (draw)
    if (g_pos.nb_moves() >= Position::WIDTH * Position::HEIGHT) {
        g_last_best_score = 0;
        return -1;
    }

    int best_col = -1;
    int best_score = -1000;

    for (int i = 0; i < Position::WIDTH; i++) {
        int col = Solver::COLUMN_ORDER[i];
        if (g_pos.can_play(col)) {
            Position next = g_pos;
            next.make_move(col);

            int score;
            if (max_depth > 0) {
                score = -g_solver.solve(next, max_depth - 1);
            } else {
                score = -g_solver.solve(next);
            }

            if (score > best_score) {
                best_score = score;
                best_col = col;
            }
        }
    }

    g_last_best_score = best_score;
    return best_col;
}

EMSCRIPTEN_KEEPALIVE
int marlin_get_best_score() {
    return g_last_best_score;
}

EMSCRIPTEN_KEEPALIVE
int marlin_is_draw() {
    return g_pos.nb_moves() >= Position::WIDTH * Position::HEIGHT ? 1 : 0;
}

} // extern "C"
