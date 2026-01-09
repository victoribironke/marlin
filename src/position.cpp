/**
 * position.cpp - Position class implementation
 */

#include "position.hpp"
#include <iostream>

// Constructor - initializes an empty board
Position::Position() : mask_(0), position_(0), moves_(0) {
    // mask_ = 0 means no cells are occupied
    // position_ = 0 means current player has no stones yet
    // moves_ = 0 means no moves have been made
}

/**
 * can_play - Check if there's room in this column.
 * 
 * We check if the TOP cell of the column is empty by looking at whether
 * that bit is set in the mask. If it's set, the column is full.
 */
bool Position::can_play(int col) const {
    // top_mask(col) has a 1 at the top playable cell of the column
    // If (mask & top_mask) is non-zero, that cell is occupied = column full
    return (mask_ & top_mask(col)) == 0;
}

/**
 * make_move - Place a piece in the given column using the gravity trick.
 * 
 * This function does 3 things:
 * 1. Switches perspective (XOR trick) so the NEW current player's view is stored
 * 2. Finds where the piece lands using binary addition (gravity trick)
 * 3. Updates the mask and increments the move counter
 */
void Position::make_move(int col) {
    // STEP 1: Switch perspective
    // -------------------------------------------------------------------------
    // Before adding the new piece, we XOR position with mask.
    // This converts "current player's stones" to "OTHER player's stones".
    // 
    // Why? After this move, the OTHER player becomes the current player.
    // So we store THEIR position, then add OUR new piece to the mask.
    // 
    // Think of it like: position_ always shows the CURRENT player's stones.
    // When players switch, we flip whose stones are stored.
    position_ ^= mask_;

    // STEP 2: Find where the piece lands (gravity trick)
    // -------------------------------------------------------------------------
    // (mask & column_mask(col)) isolates just this column's occupied cells.
    // Adding bottom_mask(col) causes the 1 to "carry up" to the first empty cell.
    // 
    // We then OR this with the current mask to add the new piece.
    mask_ |= mask_ + bottom_mask(col);

    // STEP 3: Increment move counter
    moves_++;
}

/**
 * alignment - Helper function to check if a position has 4-in-a-row.
 * 
 * This uses the bitshift technique to detect alignments:
 * 1. Shift the position by the direction offset
 * 2. AND with original - now we have bits where 2-in-a-row exist
 * 3. Shift that result by 2x the offset
 * 4. AND again - now we have bits where 4-in-a-row exist
 */
static bool alignment(uint64_t pos) {
    // Check HORIZONTAL (columns are 7 bits apart)
    uint64_t m = pos & (pos >> 7);
    if (m & (m >> 14)) return true;

    // Check VERTICAL (rows are 1 bit apart)
    m = pos & (pos >> 1);
    if (m & (m >> 2)) return true;

    // Check DIAGONAL / (going up-right = row + column = 8 bits)
    m = pos & (pos >> 8);
    if (m & (m >> 16)) return true;

    // Check DIAGONAL \ (going up-left = row - column = 6 bits)
    m = pos & (pos >> 6);
    if (m & (m >> 12)) return true;

    return false;
}

/**
 * is_winning_move - Check if playing in a column would create a 4-in-a-row.
 * 
 * We simulate adding the piece to the current player's position and check
 * if that creates a winning alignment.
 */
bool Position::is_winning_move(int col) const {
    // Calculate where the new piece would land
    // The piece lands at the first empty cell in this column
    uint64_t new_piece = (mask_ + bottom_mask(col)) & column_mask(col);
    
    // Create a hypothetical position with the new piece added
    uint64_t new_position = position_ | new_piece;
    
    // Check if this creates a 4-in-a-row
    return alignment(new_position);
}
