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
