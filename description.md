# 🐟 Project Marlin: High-Performance Connect 4 Engine

**Goal:** Build a "Stockfish-class" Connect 4 solver that plays perfectly (always wins or draws).
**Language:** C++ (Targeting C++17 or C++20 for performance).
**Architecture:** 64-bit Bitboards.

---

## 1. The Core Architecture: Bitboards

We are abandoning standard 2D arrays (`board[6][7]`) in favor of bitwise operations. The board is represented by two 64-bit integers (`unsigned long long`).

### The Mapping ( Layout)

The board is treated as **Column-Major**. Each column has **7 bits** (6 rows + 1 buffer row at the top). The buffer prevents false positives when checking for diagonal wins.

```text
  Col 0    Col 1    Col 2    Col 3    Col 4    Col 5    Col 6
+--------+--------+--------+--------+--------+--------+--------+
|  (6)   |  (13)  |  (20)  |  (27)  |  (34)  |  (41)  |  (48)  | <-- Buffer (Always 0)
+--------+--------+--------+--------+--------+--------+--------+
|   5    |   12   |   19   |   26   |   33   |   40   |   47   | <-- Row 5 (Top)
|   4    |   11   |   18   |   25   |   32   |   39   |   46   |
|   3    |   10   |   17   |   24   |   31   |   38   |   45   |
|   2    |    9   |   16   |   23   |   30   |   37   |   44   |
|   1    |    8   |   15   |   22   |   29   |   36   |   43   |
|   0    |    7   |   14   |   21   |   28   |   35   |   42   | <-- Row 0 (Bottom)
+--------+--------+--------+--------+--------+--------+--------+

```

- **Total bits used:** 49.
- **Unused bits:** 50–63 (Garbage).

### The Data Structure

The engine tracks the game state using just two variables:

1. `mask`: A bitboard showing **every** occupied cell (Red + Yellow).
2. `position`: A bitboard showing only the **current player's** stones.

- _(The opponent's stones can be calculated by `mask ^ position`)_.

---

## 2. Core Mechanics (The Math)

### A. Gravity (The "Binary Trick")

We do not iterate to find the first empty slot. We use the properties of binary addition.

- `bottom_mask(col)`: Returns a bitboard with a `1` at the bottom of the column.
- **Formula:** `mask | (mask + bottom_mask)`
- **Result:** This operation automatically "carries" the bit up to the first empty row in that column.

### B. Win Detection (Bitshifts)

We detect wins by shifting the entire board against itself. If a bit overlaps with its neighbor, we have a connection.

- **Vertical:** Shift `>> 1`
- **Horizontal:** Shift `>> 7`
- **Diagonal (/):** Shift `>> 8`
- **Diagonal ():** Shift `>> 6`

**Algorithm:**

```cpp
// Example: Horizontal check
uint64_t m = position & (position >> 7); // Find 2-in-a-row
if (m & (m >> 14)) { return true; }      // Find 4-in-a-row (2 pairs of 2)

```

---

## 3. Input & Interface

### A. The Move Standard

We do not use complex notations like PGN. We use a **Move String**.

- **Format:** A string of digits `1` through `7`.
- **Example:** `"4433221"`
- Red drops in 4.
- Yellow drops in 4.
- Red drops in 3...

- **Conversion:** When parsing, convert ASCII `'1'-'7'` to integer `0-6`.

### B. The Protocol (CLI)

Your `main.cpp` will run a loop listening for text commands (Standard Input):

1. `position [move_string]` -> Resets board and replays the string.
2. `go [time_in_ms]` -> Starts the AI search.
3. `quit` -> Exits.

---

## 4. Implementation Roadmap (Checklist)

1. **Bitboard Class:** Implement `Position` class with `make_move()` and `can_play()`.
2. **Move Parsing:** Write a helper to read a string `"4433"` and call `make_move()` sequentially.
3. **Solver (v1):** Implement a `Negamax` search (basic recursion) to find wins.
4. **Solver (v2):** Add Alpha-Beta Pruning to speed it up.
5. **Solver (v3):** Add Transposition Tables (caching) using Zobrist Hashing to reach "solved" depth.

**Ready to code?** Start by setting up your `Position` class in C++ using the bit logic above. Good luck!
