/* ========================================
   MARLIN — Connect 4 AI
   Game Controller
   ======================================== */

// ---- Constants ----
const DIFFICULTY = {
  easy: 4,
  medium: 8,
  hard: 12, // capped at 12 to prevent browser thread from hanging
};

const ROWS = 6;
const COLS = 7;

// ---- WASM API (populated after runtime init) ----
let api = null;

// ---- Game State ----
let gameState = {
  humanPlayer: 1,     // 1 = red/first, 2 = yellow/second
  difficulty: 'hard',
  isHumanTurn: true,
  isGameOver: false,
  isAiThinking: false,
  moveHistory: [],
  previousBoard: null, // for diffing renders
};

// ---- DOM References ----
const dom = {};

function cacheDom() {
  dom.setupScreen = document.getElementById('setup-screen');
  dom.gameScreen = document.getElementById('game-screen');
  dom.board = document.getElementById('board');
  dom.statusBar = document.getElementById('status-bar');
  dom.turnIndicator = document.getElementById('turn-indicator');
  dom.turnDot = dom.turnIndicator.querySelector('.turn-dot');
  dom.statusText = document.getElementById('status-text');
  dom.thinkingIndicator = document.getElementById('thinking-indicator');
  dom.resultText = document.getElementById('result-text');
  dom.newGameBtn = document.getElementById('new-game-btn');
  dom.startBtn = document.getElementById('start-btn');
  dom.overlay = document.getElementById('game-over-overlay');
  dom.overlayIcon = document.getElementById('overlay-icon');
  dom.overlayTitle = document.getElementById('overlay-title');
  dom.overlaySubtitle = document.getElementById('overlay-subtitle');
  dom.overlayNewGameBtn = document.getElementById('overlay-new-game-btn');

  dom.sideRedBtn = document.getElementById('side-red-btn');
  dom.sideYellowBtn = document.getElementById('side-yellow-btn');
  dom.diffEasyBtn = document.getElementById('diff-easy-btn');
  dom.diffMediumBtn = document.getElementById('diff-medium-btn');
  dom.diffHardBtn = document.getElementById('diff-hard-btn');

  dom.columns = dom.board.querySelectorAll('.column');
  dom.cells = {};
  dom.board.querySelectorAll('.cell').forEach((cell) => {
    const col = parseInt(cell.dataset.col);
    const row = parseInt(cell.dataset.row);
    dom.cells[`${col},${row}`] = cell;
  });
}

// ---- Setup Screen Logic ----

function initSetupListeners() {
  // Side selection
  dom.sideRedBtn.addEventListener('click', () => selectSide(1));
  dom.sideYellowBtn.addEventListener('click', () => selectSide(2));

  // Difficulty selection
  dom.diffEasyBtn.addEventListener('click', () => selectDifficulty('easy'));
  dom.diffMediumBtn.addEventListener('click', () => selectDifficulty('medium'));
  dom.diffHardBtn.addEventListener('click', () => selectDifficulty('hard'));

  // Start game
  dom.startBtn.addEventListener('click', initGame);

  // New game buttons
  dom.newGameBtn.addEventListener('click', resetToSetup);
  dom.overlayNewGameBtn.addEventListener('click', resetToSetup);
}

function selectSide(side) {
  gameState.humanPlayer = side;
  dom.sideRedBtn.classList.toggle('selected', side === 1);
  dom.sideYellowBtn.classList.toggle('selected', side === 2);
}

function selectDifficulty(diff) {
  gameState.difficulty = diff;
  document.querySelectorAll('.diff-btn').forEach((btn) => {
    btn.classList.toggle('selected', btn.dataset.diff === diff);
  });
}

// ---- Game Init ----

function initGame() {
  if (!api) return;

  api.reset();

  gameState.isGameOver = false;
  gameState.isAiThinking = false;
  gameState.moveHistory = [];
  gameState.previousBoard = null;

  // Determine who goes first
  if (gameState.humanPlayer === 1) {
    gameState.isHumanTurn = true;
  } else {
    gameState.isHumanTurn = false;
  }

  // Track game start
  if (typeof posthog !== 'undefined') {
    posthog.capture('game_started', {
      difficulty: gameState.difficulty,
      human_player: gameState.humanPlayer === 1 ? 'red' : 'yellow'
    });
  }

  // Switch screens
  dom.setupScreen.classList.add('hidden');
  dom.gameScreen.classList.remove('hidden');
  dom.overlay.classList.add('hidden');

  // Reset board DOM
  clearBoardClasses();
  renderBoard();
  updateStatus();

  // If AI goes first, trigger AI move
  if (!gameState.isHumanTurn) {
    triggerAiMove();
  }
}

function clearBoardClasses() {
  Object.values(dom.cells).forEach((cell) => {
    cell.className = 'cell';
  });
  dom.board.classList.remove('disabled');
}

// ---- Board Rendering ----

function renderBoard() {
  const currentBoard = [];

  for (let col = 0; col < COLS; col++) {
    for (let row = 0; row < ROWS; row++) {
      const val = api.getCell(col, row);
      const key = `${col},${row}`;
      const cell = dom.cells[key];

      currentBoard.push(val);

      // Skip if unchanged (unless we haven't rendered before)
      if (
        gameState.previousBoard &&
        gameState.previousBoard[col * ROWS + row] === val
      ) {
        continue;
      }

      // Update class — but don't remove dropping or winning classes
      if (val === 0) {
        cell.classList.remove('red', 'yellow');
      } else if (val === 1) {
        cell.classList.remove('yellow');
        cell.classList.add('red');
      } else if (val === 2) {
        cell.classList.remove('red');
        cell.classList.add('yellow');
      }
    }
  }

  gameState.previousBoard = currentBoard;
}

// ---- Cell Lookup ----

function getCell(col, row) {
  return dom.cells[`${col},${row}`];
}

function getLowestEmptyRow(col) {
  for (let row = 0; row < ROWS; row++) {
    if (api.getCell(col, row) === 0) {
      return row;
    }
  }
  return -1;
}

// ---- Column Click ----

function handleColumnClick(col) {
  if (!gameState.isHumanTurn || gameState.isGameOver || gameState.isAiThinking) {
    return;
  }

  if (!api.canPlay(col)) {
    return;
  }

  const row = getLowestEmptyRow(col);
  if (row === -1) return;

  const currentPlayer = gameState.humanPlayer;
  const isWin = api.isWinningMove(col) === 1;

  api.makeMove(col);
  gameState.moveHistory.push({ col, row, player: currentPlayer });

  // Animate and render
  animateDrop(col, row, currentPlayer);

  if (isWin) {
    gameState.isGameOver = true;
    gameState.isHumanTurn = false;
    setTimeout(() => {
      highlightWinningCells(currentPlayer);
      showResult('win', currentPlayer);
    }, 550);
    return;
  }

  if (api.isDraw()) {
    gameState.isGameOver = true;
    gameState.isHumanTurn = false;
    setTimeout(() => {
      showResult('draw', 0);
    }, 550);
    return;
  }

  // AI's turn
  gameState.isHumanTurn = false;
  clearHoverPreview();
  triggerAiMove();
}

// ---- AI Move ----

function triggerAiMove() {
  gameState.isAiThinking = true;
  updateStatus();
  dom.board.classList.add('disabled');

  // Use setTimeout to let the UI update (show thinking indicator)
  setTimeout(() => {
    const depth = DIFFICULTY[gameState.difficulty];
    const bestCol = api.getBestMove(depth);
    const aiPlayer = gameState.humanPlayer === 1 ? 2 : 1;

    const row = getLowestEmptyRow(bestCol);
    const isWin = api.isWinningMove(bestCol) === 1;

    api.makeMove(bestCol);
    gameState.moveHistory.push({ col: bestCol, row, player: aiPlayer });

    gameState.isAiThinking = false;

    // Animate
    animateDrop(bestCol, row, aiPlayer);

    if (isWin) {
      gameState.isGameOver = true;
      dom.board.classList.remove('disabled');
      setTimeout(() => {
        highlightWinningCells(aiPlayer);
        showResult('lose', aiPlayer);
      }, 550);
      return;
    }

    if (api.isDraw()) {
      gameState.isGameOver = true;
      dom.board.classList.remove('disabled');
      setTimeout(() => {
        showResult('draw', 0);
      }, 550);
      return;
    }

    gameState.isHumanTurn = true;
    dom.board.classList.remove('disabled');
    updateStatus();
  }, 600);
}

// ---- Drop Animation ----

function animateDrop(col, row, player) {
  const cell = getCell(col, row);
  const cls = player === 1 ? 'red' : 'yellow';

  cell.classList.add(cls, 'dropping');

  const onAnimEnd = () => {
    cell.classList.remove('dropping');
    cell.removeEventListener('animationend', onAnimEnd);
    renderBoard();
  };

  cell.addEventListener('animationend', onAnimEnd);
}

// ---- Win Detection & Highlighting ----

function highlightWinningCells(player) {
  const directions = [
    [1, 0],   // horizontal
    [0, 1],   // vertical
    [1, 1],   // diagonal /
    [1, -1],  // diagonal \
  ];

  for (let col = 0; col < COLS; col++) {
    for (let row = 0; row < ROWS; row++) {
      if (api.getCell(col, row) !== player) continue;

      for (const [dc, dr] of directions) {
        const cells = [];
        let valid = true;

        for (let i = 0; i < 4; i++) {
          const c = col + dc * i;
          const r = row + dr * i;

          if (c < 0 || c >= COLS || r < 0 || r >= ROWS) {
            valid = false;
            break;
          }

          if (api.getCell(c, r) !== player) {
            valid = false;
            break;
          }

          cells.push(getCell(c, r));
        }

        if (valid && cells.length === 4) {
          cells.forEach((cell) => cell.classList.add('winning'));
          return; // found the winning line
        }
      }
    }
  }
}

// ---- Hover Preview ----

function setupColumnHovers() {
  dom.columns.forEach((column) => {
    const col = parseInt(column.dataset.col);

    column.addEventListener('mouseenter', () => {
      if (!gameState.isHumanTurn || gameState.isGameOver || gameState.isAiThinking) {
        return;
      }

      const row = getLowestEmptyRow(col);
      if (row === -1) return;

      const hoverClass =
        gameState.humanPlayer === 1 ? 'hover-red' : 'hover-yellow';
      getCell(col, row).classList.add(hoverClass);
    });

    column.addEventListener('mouseleave', () => {
      clearHoverPreview();
    });

    column.addEventListener('click', () => {
      handleColumnClick(col);
    });
  });
}

function clearHoverPreview() {
  Object.values(dom.cells).forEach((cell) => {
    cell.classList.remove('hover-red', 'hover-yellow');
  });
}

// ---- Status Updates ----

function updateStatus() {
  if (gameState.isGameOver) return;

  if (gameState.isAiThinking) {
    dom.turnIndicator.classList.add('hidden');
    dom.thinkingIndicator.classList.remove('hidden');
    dom.resultText.classList.add('hidden');
  } else {
    dom.thinkingIndicator.classList.add('hidden');
    dom.resultText.classList.add('hidden');
    dom.turnIndicator.classList.remove('hidden');

    if (gameState.isHumanTurn) {
      const color = gameState.humanPlayer === 1 ? 'red' : 'yellow';
      dom.turnDot.className = 'turn-dot';
      if (color === 'yellow') dom.turnDot.classList.add('yellow');
      dom.statusText.textContent = 'Your turn';
    } else {
      const aiPlayer = gameState.humanPlayer === 1 ? 2 : 1;
      const color = aiPlayer === 1 ? 'red' : 'yellow';
      dom.turnDot.className = 'turn-dot';
      if (color === 'yellow') dom.turnDot.classList.add('yellow');
      dom.statusText.textContent = "AI's turn";
    }
  }
}

// ---- Results ----

function showResult(result, player) {
  gameState.isGameOver = true;

  // Update status bar
  dom.turnIndicator.classList.add('hidden');
  dom.thinkingIndicator.classList.add('hidden');
  dom.resultText.classList.remove('hidden');

  let message = '';
  let overlayIcon = '';
  let overlayTitle = '';
  let overlaySubtitle = '';

  if (result === 'win') {
    message = '🎉 You win!';
    dom.resultText.className = 'win';
    overlayIcon = '🏆';
    overlayTitle = 'You Win!';
    overlaySubtitle = 'Incredible! You outsmarted the AI.';
  } else if (result === 'lose') {
    message = '🐟 Marlin wins!';
    dom.resultText.className = 'lose';
    overlayIcon = '🐟';
    overlayTitle = 'Marlin Wins!';
    overlaySubtitle = 'The AI found the winning strategy. Try again!';
  } else {
    message = "🤝 It's a draw!";
    dom.resultText.className = 'draw';
    overlayIcon = '🤝';
    overlayTitle = "It's a Draw!";
    overlaySubtitle = 'A well-fought battle. Neither side could win.';
  }

  dom.resultText.textContent = message;
  dom.resultText.id = 'result-text';

  // Show overlay
  dom.overlayIcon.textContent = overlayIcon;
  dom.overlayTitle.textContent = overlayTitle;
  dom.overlaySubtitle.textContent = overlaySubtitle;
  dom.overlay.classList.remove('hidden');

  // Track game over
  if (typeof posthog !== 'undefined') {
    posthog.capture('game_completed', {
      result: result, // 'win', 'lose', or 'draw'
      difficulty: gameState.difficulty,
      human_player: gameState.humanPlayer === 1 ? 'red' : 'yellow',
      total_moves: api.nbMoves()
    });
  }
}

// ---- Navigation ----

function resetToSetup() {
  dom.gameScreen.classList.add('hidden');
  dom.overlay.classList.add('hidden');
  dom.setupScreen.classList.remove('hidden');
  dom.board.classList.remove('disabled');

  // Reset status
  dom.turnIndicator.classList.remove('hidden');
  dom.thinkingIndicator.classList.add('hidden');
  dom.resultText.classList.add('hidden');
  dom.statusText.textContent = 'Your turn';
  dom.turnDot.className = 'turn-dot';

  clearBoardClasses();
  gameState.previousBoard = null;
}

// ---- WASM Initialization ----

// Module is created by marlin.js (Emscripten glue)
// We set the onRuntimeInitialized callback before it loads,
// or if Module is already ready, we detect that too.

function onWasmReady() {
  api = {
    reset: Module.cwrap('marlin_reset', null, []),
    makeMove: Module.cwrap('marlin_make_move', 'number', ['number']),
    canPlay: Module.cwrap('marlin_can_play', 'number', ['number']),
    isWinningMove: Module.cwrap('marlin_is_winning_move', 'number', ['number']),
    getCell: Module.cwrap('marlin_get_cell', 'number', ['number', 'number']),
    nbMoves: Module.cwrap('marlin_nb_moves', 'number', []),
    getBestMove: Module.cwrap('marlin_get_best_move', 'number', ['number']),
    getBestScore: Module.cwrap('marlin_get_best_score', 'number', []),
    isDraw: Module.cwrap('marlin_is_draw', 'number', []),
  };

  // Enable the start button
  dom.startBtn.disabled = false;
  dom.startBtn.textContent = 'Start Game';
}

// ---- Bootstrap ----

document.addEventListener('DOMContentLoaded', () => {
  cacheDom();
  initSetupListeners();
  setupColumnHovers();

  // Show loading state until WASM is ready
  dom.startBtn.disabled = true;
  dom.startBtn.textContent = 'Loading AI…';

  // Set up the WASM ready callback
  if (typeof Module !== 'undefined') {
    if (Module.calledRun) {
      // Already initialized
      onWasmReady();
    } else {
      const existingCallback = Module.onRuntimeInitialized;
      Module.onRuntimeInitialized = function () {
        if (existingCallback) existingCallback();
        onWasmReady();
      };
    }
  } else {
    // Module not yet defined — set up a global for Emscripten to pick up
    window.Module = {
      onRuntimeInitialized: onWasmReady,
    };
  }
});
