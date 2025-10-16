#include <cstdint>

#include "../headers/PPU.h"

using std::uint16_t;
using std::uint8_t;

uint8_t PPU::read(uint16_t address) {
    switch (address) {
        case 0x2002:
            // TODO: PPUSTATUS
            return 0x00;
        case 0x2004:
            // TODO: OAMDATA
            return 0x00;
        case 0x2007:
            // TODO: PPUDATA
            return 0x00;
        default:
            return 0x00;
    }
}

void PPU::write(uint16_t address, uint8_t data) {
    switch (address) {
        case 0x2000:
            // TODO: PPUCTRL
            return;
        case 0x2001:
            // TODO: PPUMASK
            return;
        case 0x2003:
            // TODO: OAMAADDR
            return;
        case 0x2004:
            // TODO: OAMADATA
            return;
        case 0x2005:
            // TODO: PPUSCROLL
            return;
        case 0x2006:
            // TODO: PPUADDR
            return;
        case 0x2007:
            // TODO: PPUDATA
            return;
    }
}