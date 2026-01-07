/**
 * position.hpp - Connect 4 Bitboard Position Representation
 * 
 * =============================================================================
 * HOW BITBOARDS WORK (Beginner Explanation):
 * =============================================================================
 * 
 * Instead of using a 2D array like board[6][7], we use a single 64-bit number
 * (called a "bitboard") where each bit represents one cell on the board.
 * 
 * Why? Because computers are EXTREMELY fast at bitwise operations (AND, OR, 
 * XOR, shifts). This lets us check for wins, make moves, etc. in just a few
 * CPU instructions instead of loops.
 * 
 * =============================================================================
 * THE BIT LAYOUT:
 * =============================================================================
 * 
 * We map the 6x7 board to bits 0-48 of a 64-bit integer like this:
 * 
 *   Col 0    Col 1    Col 2    Col 3    Col 4    Col 5    Col 6
 * +--------+--------+--------+--------+--------+--------+--------+
 * |  (6)   |  (13)  |  (20)  |  (27)  |  (34)  |  (41)  |  (48)  | <- Buffer row (always 0)
 * +--------+--------+--------+--------+--------+--------+--------+
 * |   5    |   12   |   19   |   26   |   33   |   40   |   47   | <- Row 5 (top)
 * |   4    |   11   |   18   |   25   |   32   |   39   |   46   |
 * |   3    |   10   |   17   |   24   |   31   |   38   |   45   |
 * |   2    |    9   |   16   |   23   |   30   |   37   |   44   |
 * |   1    |    8   |   15   |   22   |   29   |   36   |   43   |
 * |   0    |    7   |   14   |   21   |   28   |   35   |   42   | <- Row 0 (bottom)
 * +--------+--------+--------+--------+--------+--------+--------+
 * 
 * Each column uses 7 bits: 6 for actual rows + 1 "buffer" bit at the top.
 * The buffer prevents false diagonal wins from wrapping around.
 * 
 * Formula: bit_index = col * 7 + row
 *   - Cell (row=0, col=0) -> bit 0
 *   - Cell (row=5, col=0) -> bit 5
 *   - Cell (row=0, col=1) -> bit 7
 *   - Cell (row=2, col=3) -> bit 23
 * 
 * =============================================================================
 */

#ifndef POSITION_HPP
#define POSITION_HPP

#include <cstdint>  // For uint64_t (a 64-bit unsigned integer)
#include <string>

class Position {
public:
    // Board dimensions
    static constexpr int WIDTH = 7;   // Number of columns
    static constexpr int HEIGHT = 6;  // Number of rows (playable)

    // Constructor - creates an empty board
    Position();

    // -------------------------------------------------------------------------
    // Helper Mask Functions (explained below)
    // -------------------------------------------------------------------------

    /**
     * bottom_mask(col) - Returns a bitboard with a single 1 at the BOTTOM of
     * the given column.
     * 
     * Example: bottom_mask(0) returns:
     *   Bit 0 is set (binary: 0000...0001)
     *   This represents the bottom cell of column 0.
     * 
     * Example: bottom_mask(3) returns:
     *   Bit 21 is set (binary: 0000...0010000000000000000000)
     *   This represents the bottom cell of column 3.
     */
    static constexpr uint64_t bottom_mask(int col) {
        return 1ULL << (col * (HEIGHT + 1));
    }

    /**
     * column_mask(col) - Returns a bitboard with 1s in ALL playable cells of
     * the given column (6 bits set, not including the buffer bit).
     * 
     * Example: column_mask(0) returns bits 0-5 set.
     * Example: column_mask(1) returns bits 7-12 set.
     */
    static constexpr uint64_t column_mask(int col) {
        return ((1ULL << HEIGHT) - 1) << (col * (HEIGHT + 1));
    }

    /**
     * top_mask(col) - Returns a bitboard with a single 1 at the TOP PLAYABLE
     * cell of the given column (row 5, NOT the buffer).
     * 
     * This is useful for checking if a column is full.
     */
    static constexpr uint64_t top_mask(int col) {
        return 1ULL << ((HEIGHT - 1) + col * (HEIGHT + 1));
    }

    /**
     * bottom() - Returns a bitboard with 1s at the bottom of EVERY column.
     * Used for the "gravity trick" when making moves.
     */
    static constexpr uint64_t bottom() {
        // This creates: bit 0 | bit 7 | bit 14 | bit 21 | bit 28 | bit 35 | bit 42
        uint64_t result = 0;
        for (int col = 0; col < WIDTH; ++col) {
            result |= bottom_mask(col);
        }
        return result;
    }

    // Pre-computed bottom mask for efficiency
    static constexpr uint64_t BOTTOM_MASK = 
        (1ULL << 0) | (1ULL << 7) | (1ULL << 14) | (1ULL << 21) | 
        (1ULL << 28) | (1ULL << 35) | (1ULL << 42);

    // Full board mask (all playable cells)
    static constexpr uint64_t BOARD_MASK = BOTTOM_MASK * ((1ULL << HEIGHT) - 1);

private:
    // -------------------------------------------------------------------------
    // THE TWO CORE BITBOARDS
    // -------------------------------------------------------------------------
    
    /**
     * mask - A bitboard where every occupied cell (by either player) has a 1.
     * 
     * If Red is at (0,0) and Yellow is at (0,1):
     *   mask = 0000...10000001 (bits 0 and 7 are set)
     */
    uint64_t mask_;

    /**
     * position - A bitboard showing only the CURRENT PLAYER's stones.
     * 
     * We alternate players each turn. By XORing mask and position,
     * we can get the opponent's stones: opponent = mask ^ position
     */
    uint64_t position_;

    /**
     * moves - How many moves have been played (0 to 42).
     * Used to determine whose turn it is (even = player 1, odd = player 2)
     * and to detect draws (42 moves = full board).
     */
    int moves_;
};

#endif // POSITION_HPP
