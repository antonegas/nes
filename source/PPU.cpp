#include <cstdint>

#include "../headers/PPU.h"

using std::uint16_t;
using std::uint8_t;

uint8_t PPU::read(uint16_t address) {
    switch (address) {
        case 0x2002:
            // PPUSTATUS
            uint8_t status = ppustatus.data & 0xE0;

            // Reading PPUSTATUS has side effects.
            ppustatus.V = 0;
            // TODO: w register

            return status;
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
            break;
        case 0x2001:
            // TODO: PPUMASK
            break;
        case 0x2003:
            // OAMADDR
            oamaddr = data;
            break;
        case 0x2004:
            // OAMDATA
            ((uint8_t*)oam)[oamaddr] = data;
            break;
        case 0x2005:
            // TODO: PPUSCROLL
            break;
        case 0x2006:
            // TODO: PPUADDR
            break;
        case 0x2007:
            // TODO: PPUDATA
            break;
    }
}

void PPU::dmaWrite(uint8_t data) {
    ((uint8_t*)oam)[oamaddr] = data;
    dmaaddr++;
}