# 🐟 Project Marlin

A high-performance Connect 4 solver using C++ bitboards, inspired by Stockfish's architecture.

**[▶ Play Online](https://marlin-connect4.vercel.app)** · **[GitHub](https://github.com/victoribironke/marlin)**

## Features

- **Bitboard representation** - 64-bit integer board storage for blazing fast operations
- **Negamax solver** - Recursive algorithm to find game-theoretic optimal moves
- **Alpha-Beta pruning** - Skips billions of unnecessary calculations
- **Transposition tables** - Caches positions to avoid redundant work
- **Web UI** - Play against the AI in your browser (compiled to WebAssembly)
- **Difficulty levels** - Easy, Medium, and Hard (perfect play)
- **CLI interface** - Simple text-based interaction

## Play Online

The solver is compiled to WebAssembly and runs entirely in your browser — no server needed.

Visit **[marlin-connect4.vercel.app](https://marlin-connect4.vercel.app)** to play.

| Difficulty | Search Depth | Description                          |
| ---------- | ------------ | ------------------------------------ |
| Easy       | 4 moves      | Makes mistakes, great for beginners  |
| Medium     | 8 moves      | Solid play, misses deep tactics      |
| Hard       | Unlimited    | Perfect play — good luck!            |

## Project Structure

```
marlin/
├── include/              # C++ headers
│   ├── position.hpp      # Bitboard position representation
│   ├── solver.hpp        # Negamax solver with alpha-beta pruning
│   └── transposition.hpp # Transposition table for caching
├── src/                  # C++ source files
│   ├── main.cpp          # CLI interface
│   ├── position.cpp      # Position implementation
│   ├── solver.cpp        # Solver implementation
│   ├── transposition.cpp # Transposition table implementation
│   └── bindings.cpp      # Emscripten WASM bindings
├── web/                  # Web UI (static site)
│   ├── index.html        # Page structure
│   ├── style.css         # Styling
│   ├── app.js            # Game logic
│   ├── marlin.js         # Emscripten glue (generated)
│   └── marlin.wasm       # WebAssembly binary (generated)
├── tests/                # Test files
├── build_wasm.bat        # WASM build script (Windows)
├── vercel.json           # Vercel deployment config
└── CMakeLists.txt        # CMake build config (native CLI)
```

## Building

### Native CLI

```bash
# Generate build files
cmake -B build -S . -G "MinGW Makefiles"

# Build
cmake --build build
```

### WebAssembly (for the web UI)

Requires [Emscripten SDK](https://emscripten.org/docs/getting_started/).

```bash
# Activate Emscripten (run once per terminal session)
D:\emsdk\emsdk_env.bat

# Build WASM
build_wasm.bat
```

This generates `web/marlin.js` and `web/marlin.wasm`.

To test locally:

```bash
npx serve web
```

## CLI Usage

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
| . . O O . . . |
| . . X X . . . |
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
| **WebAssembly**          | Compile C++ to run natively in the browser                 |

## Deployment

The web UI is deployed on [Vercel](https://vercel.com) as a static site.

```bash
# Install Vercel CLI
npm i -g vercel

# Deploy
vercel --prod
```

## License

MIT
