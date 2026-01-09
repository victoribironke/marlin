/**
 * main.cpp - Marlin Connect 4 Engine CLI Interface
 * 
 * COMMAND PROTOCOL:
 * =============================================================================
 * 
 * The engine reads commands from stdin (standard input) and responds to stdout.
 * This is similar to how chess engines work with UCI protocol.
 * 
 * Commands:
 *   position [moves]  - Set up a position by playing the given move string
 *                       Example: "position 4433221" plays moves 4,4,3,3,2,2,1
 *   display           - Show the current board state
 *   go                - Find the best move (solver will be added later)
 *   quit              - Exit the program
 * 
 * Move String Format:
 *   A string of digits 1-7, each representing a column to drop a piece in.
 *   Example: "4433" means:
 *     - Player 1 drops in column 4
 *     - Player 2 drops in column 4
 *     - Player 1 drops in column 3
 *     - Player 2 drops in column 3
 */

#include "position.hpp"
#include <iostream>
#include <string>
#include <sstream>

/**
 * parse_moves - Parse a move string and apply moves to a position.
 * 
 * @param pos       The Position object to modify
 * @param moves     A string like "4433221" where each digit is a column (1-7)
 * @return          Number of moves successfully parsed, or -1 if invalid
 */
int parse_moves(Position& pos, const std::string& moves) {
    int count = 0;
    
    for (char c : moves) {
        // Check if character is a valid column digit (1-7)
        if (c < '1' || c > '7') {
            std::cerr << "Error: Invalid character '" << c << "' in move string\n";
            return -1;
        }
        
        // Convert '1'-'7' to 0-6 (C++ uses 0-indexed arrays)
        int col = c - '1';
        
        // Check if column is playable
        if (!pos.can_play(col)) {
            std::cerr << "Error: Column " << (col + 1) << " is full\n";
            return -1;
        }
        
        // Make the move
        pos.make_move(col);
        count++;
    }
    
    return count;
}

/**
 * handle_position - Handle the "position" command.
 * 
 * Resets the board and replays the given moves.
 */
void handle_position(Position& pos, const std::string& args) {
    // Reset to empty position
    pos = Position();
    
    // If there are moves to play, parse them
    if (!args.empty()) {
        int result = parse_moves(pos, args);
        if (result >= 0) {
            std::cout << "Played " << result << " moves\n";
        }
    } else {
        std::cout << "Position reset to empty board\n";
    }
}

/**
 * handle_go - Handle the "go" command.
 * 
 * For now, this is a placeholder. The actual solver will be added in Phase 3.
 */
void handle_go(Position& pos) {
    // Check for immediate wins
    for (int col = 0; col < Position::WIDTH; col++) {
        if (pos.can_play(col) && pos.is_winning_move(col)) {
            std::cout << "bestmove " << (col + 1) << " (immediate win!)\n";
            return;
        }
    }
    
    // Placeholder: just pick the first playable column
    for (int col = 0; col < Position::WIDTH; col++) {
        if (pos.can_play(col)) {
            std::cout << "bestmove " << (col + 1) << " (placeholder - no solver yet)\n";
            return;
        }
    }
    
    std::cout << "No moves available (draw)\n";
}

/**
 * main - Main command loop.
 */
int main() {
    std::cout << "Marlin Connect 4 Engine v0.1\n";
    std::cout << "Type 'help' for available commands.\n\n";
    
    Position pos;
    std::string line;
    
    while (true) {
        std::cout << "> ";
        
        // Read a line from stdin
        if (!std::getline(std::cin, line)) {
            break;  // EOF or error
        }
        
        // Parse the command and arguments
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        
        // Get remaining arguments (if any)
        std::string args;
        std::getline(iss >> std::ws, args);  // Skip whitespace, get rest
        
        // Handle commands
        if (command == "quit" || command == "exit") {
            std::cout << "Goodbye!\n";
            break;
        }
        else if (command == "help") {
            std::cout << "Commands:\n";
            std::cout << "  position [moves]  - Set position (e.g., 'position 4433')\n";
            std::cout << "  display           - Show the board\n";
            std::cout << "  go                - Find best move\n";
            std::cout << "  quit              - Exit\n";
        }
        else if (command == "position") {
            handle_position(pos, args);
        }
        else if (command == "display" || command == "d") {
            pos.display();
        }
        else if (command == "go") {
            handle_go(pos);
        }
        else if (command.empty()) {
            // Ignore empty lines
        }
        else {
            std::cout << "Unknown command: " << command << " (type 'help' for commands)\n";
        }
    }
    
    return 0;
}
