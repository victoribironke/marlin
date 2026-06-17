@echo off
REM ============================================================================
REM build_wasm.bat - Compile Marlin Connect 4 solver to WebAssembly
REM
REM Prerequisites:
REM   1. Install Emscripten SDK: https://emscripten.org/docs/getting_started/
REM   2. Activate emsdk in this terminal session:
REM        D:\emsdk\emsdk_env.bat
REM
REM Usage:
REM   build_wasm.bat
REM
REM Output:
REM   web/marlin.js    - Emscripten glue code (loads the WASM)
REM   web/marlin.wasm  - Compiled WebAssembly binary
REM ============================================================================

echo.
echo  === Marlin WASM Build ===
echo.

echo Compiling C++ to WebAssembly...

D:\emsdk\upstream\emscripten\emcc.exe ^
    src/position.cpp ^
    src/solver.cpp ^
    src/transposition.cpp ^
    src/bindings.cpp ^
    -Iinclude ^
    -O2 ^
    -std=c++17 ^
    -sALLOW_MEMORY_GROWTH=1 ^
    -sEXPORTED_FUNCTIONS="['_marlin_reset','_marlin_make_move','_marlin_can_play','_marlin_is_winning_move','_marlin_get_cell','_marlin_nb_moves','_marlin_get_best_move','_marlin_get_best_score','_marlin_is_draw']" ^
    -sEXPORTED_RUNTIME_METHODS="['cwrap','ccall']" ^
    -o web/marlin.js

if %ERRORLEVEL% neq 0 (
    echo.
    echo BUILD FAILED
    exit /b 1
)

echo.
echo Build successful!
echo   web/marlin.js   - Emscripten glue code
echo   web/marlin.wasm - WebAssembly binary
echo.
echo To test locally:
echo   npx serve web
echo.
