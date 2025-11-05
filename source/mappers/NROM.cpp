#include <cstdint>

#include "mappers/NROM.h"

using std::uint16_t;
using std::uint8_t;

uint8_t NROM::cpuRead(uint16_t addr) {
    return prgrom[prgAddr(addr)];
}

uint8_t NROM::ppuRead(uint16_t addr) {
    return chrrom[chrAddr(addr)];
}

uint16_t NROM::prgAddr(uint16_t addr) {
    if (prgrom.size() == 0x4000) return addr & 0x3FFF;
    return addr & 0x7FFF;
}

uint16_t NROM::chrAddr(uint16_t addr) {
    return addr & 0x1FFF;
}