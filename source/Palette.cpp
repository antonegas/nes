#include <cstdint>

#include "Palette.h"

using std::uint16_t;
using std::uint8_t;

uint8_t Palette::getR(uint16_t entry) {
    return data[(entry * 3 + emphasis * 192) % data.size()];
}

uint8_t Palette::getG(uint16_t entry) {
    return data[(entry * 3 + emphasis * 192 + 1) % data.size()];
}

uint8_t Palette::getB(uint16_t entry) {
    return data[(entry * 3 + emphasis * 192 + 2) % data.size()];
}

void Palette::setEmphasis(uint16_t emphasis) {
    this->emphasis = emphasis;
}

void Palette::setEmphasis(bool r, bool g, bool b) {
    uint16_t emphasis = (b << 2) | (g << 1) | r;
    this->emphasis = emphasis;
}