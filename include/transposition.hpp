/**
 * transposition.hpp - Transposition Table for Caching Position Evaluations
 * 
 * WHAT IS A TRANSPOSITION TABLE?
 * =============================================================================
 * 
 * A transposition table is a cache (hash table) that stores positions we've
 * already evaluated. In Connect 4, the same position can be reached by
 * different move sequences (a "transposition"). For example:
 * 
 *   Sequence A: 1, 2, 3   →  [same position]  ←  Sequence B: 3, 2, 1
 * 
 * If we've already computed the value of this position via sequence A, we can
 * SKIP the entire subtree when we reach it via sequence B!
 * 
 * UNIQUE POSITION KEY:
 * =============================================================================
 * 
 * For Connect 4, we can create a unique key for each position using the
 * current player's stones and the mask (all stones).
 * 
 * The formula: key = position + mask + BOTTOM_MASK
 * 
 * This creates a unique 64-bit integer for each possible game state.
 * (No need for Zobrist hashing in Connect 4 - this is simpler and works!)
 * 
 * TABLE STRUCTURE:
 * =============================================================================
 * 
 * We use a simple array of entries. Each entry stores:
 *   - key: The position's unique identifier
 *   - value: The alpha-beta score for this position
 * 
 * We use key % table_size as the index. Collisions are handled by replacement
 * (newer entries overwrite older ones at the same index).
 */

#ifndef TRANSPOSITION_HPP
#define TRANSPOSITION_HPP

#include <cstdint>
#include <vector>

class TranspositionTable {
public:
    /**
     * Constructor - Creates a table with the given size.
     * 
     * @param size  Number of entries (should be a power of 2 for efficiency)
     *              Default is 8 million entries (~64 MB of memory)
     */
    explicit TranspositionTable(size_t size = 8388608);  // 2^23 = 8M entries

    /**
     * put - Store a position's value in the table.
     * 
     * @param key    Unique position key
     * @param value  The evaluated score
     */
    void put(uint64_t key, int8_t value);

    /**
     * get - Retrieve a position's value from the table.
     * 
     * @param key  Unique position key
     * @return     The stored value, or 0 if not found
     */
    int8_t get(uint64_t key) const;

    /**
     * reset - Clear all entries (for starting a new game).
     */
    void reset();

private:
    struct Entry {
        uint64_t key = 0;   // Position key (0 means empty)
        int8_t value = 0;   // Stored score
    };

    std::vector<Entry> table_;
    size_t size_;

    // Helper to compute index from key
    size_t index(uint64_t key) const { return key % size_; }
};

#endif // TRANSPOSITION_HPP
