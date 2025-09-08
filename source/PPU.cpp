#include <cstdint>

#include "../headers/PPU.h"

using std::uint16_t;
using std::uint8_t;

uint8_t PPU::read(uint16_t address) {
    switch (address) {
    case 0x2000:
        return ppuctrl;
        break;
    case 0x2001:
        return ppumask;
        break;
    case 0x2002:
        return ppustatus;
        break;
    case 0x2003:
        return oamaddr;
        break;
    case 0x2004:
        return oamdata;
        break;
    case 0x2005:
        return ppuscroll;
        break;
    case 0x2006:
        return ppuaddr;
        break;
    case 0x2007:
        return ppudata;
        break;
    default:
        return 0x00;
        break;
    }
}