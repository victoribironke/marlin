# 🐟 Project Marlin

A high-performance Connect 4 solver using C++ bitboards, inspired by Stockfish's architecture.

## Features

- **Bitboard representation** - 64-bit integer board storage for blazing fast operations
- **Negamax solver** - Recursive algorithm to find game-theoretic optimal moves
- **Alpha-Beta pruning** - Skips billions of unnecessary calculations
- **Transposition tables** - Caches positions to avoid redundant work
- **CLI interface** - Simple text-based interaction

## Building

```bash
# Generate build files
cmake -B build -S . -G "MinGW Makefiles"

# Build
cmake --build build
```

## Usage

```bash
./build/marlin
```

### Commands

| Command            | Description                      | Example         |
| ------------------ | -------------------------------- | --------------- |
| `position [moves]` | Set up position by playing moves | `position 4433` |
| `display`          | Show the board                   | `display`       |
| `go`               | Find the best move               | `go`            |
| `help`             | Show commands                    | `help`          |
| `quit`             | Exit                             | `quit`          |

### Move Format

Moves are digits 1-7 representing columns:

- `4` = Drop piece in column 4 (center)
- `4433` = P1→col4, P2→col4, P1→col3, P2→col3

### Example Session

```
> position 4433
Played 4 moves

> display
| . . . . . . . |
| . . . . . . . |
| . . . . . . . |
| . . . . . . . |
| . . X O . . . |
| . . X O . . . |
+---------------+
  1 2 3 4 5 6 7

> go
Analyzing...
  Column 3: score 18
bestmove 3 score 18 (WIN)
Nodes analyzed: 12847
```

### Understanding Scores

- **Positive** = Current player can force a win
- **Zero** = Draw with perfect play
- **Negative** = Opponent can force a win
- **Higher** = Faster win

## Key Concepts

| Technique                | What I Learned                                             |
| ------------------------ | ---------------------------------------------------------- |
| **Bitboards**            | Represent game state as bits for 100x faster operations    |
| **Negamax**              | "My opponent's loss = my gain" simplifies game tree search |
| **Alpha-Beta**           | Skip branches that can't affect the result                 |
| **Transposition Tables** | Cache results to avoid redundant computation               |
| **Move Ordering**        | Try center columns first for better pruning                |

## License

MIT
