#include <cstdint>

#include "../headers/StandardController.h"

using std::uint8_t;

uint8_t StandardController::read() {
    if (!remaining) return 0x01;

    bool bit = (buffer & 0x80); // Capture the bit before shifting.
    buffer = buffer << 1;
    remaining--;

    return bit;
}

void StandardController::reload() {
    buffer = state;
    remaining = 0x08;
}