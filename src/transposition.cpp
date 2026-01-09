/**
 * transposition.cpp - Transposition Table Implementation
 */

#include "transposition.hpp"

TranspositionTable::TranspositionTable(size_t size) : size_(size) {
    table_.resize(size);
}

void TranspositionTable::put(uint64_t key, int8_t value) {
    size_t idx = index(key);
    table_[idx].key = key;
    table_[idx].value = value;
}

int8_t TranspositionTable::get(uint64_t key) const {
    size_t idx = index(key);
    if (table_[idx].key == key) {
        return table_[idx].value;
    }
    return 0;  // Not found (0 indicates unknown, which is also a draw score)
}

void TranspositionTable::reset() {
    for (auto& entry : table_) {
        entry.key = 0;
        entry.value = 0;
    }
}
