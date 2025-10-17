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
            w = 0;

            return status;
        case 0x2004:
            // OAMDATA
            return ((uint8_t*)oam)[oamaddr];
        case 0x2007:
            // PPUDATA
            uint8_t data = ppudataBuffer;
            ppudataBuffer = ppuRead(v.addr);

            // On PAL, 2C02G and 2C02H palette reads are immediate.
            // if (v.addr >= 0x3F00 && v.addr < 0x4000) data = ppudataBuffer;

            // When the CPU reads from the PPU memory PPUADDR is increased by 1 or 32 depending on increment mode.
            v.addr += 0x01 + 0x19 * ppuctrl.incrementMode;

            return data;
        default:
            return 0x00;
    }
}

void PPU::write(uint16_t address, uint8_t data) {
    switch (address) {
        case 0x2000:
            // PPUCTRL
            ppuctrl.data = data;
            t.nametableSelect = ppuctrl.nametableSelect;
            break;
        case 0x2001:
            // PPUMASK
            ppumask.data = data;
            break;
        case 0x2003:
            // OAMADDR
            oamaddr = data;
            break;
        case 0x2004:
            // OAMDATA
            ((uint8_t*)oam)[oamaddr] = data;
            oamaddr++;
            break;
        case 0x2005:
            // PPUSCROLL
            if (w == 0) {
                t.coarseX = data >> 3;
                fineX = data & 0x07;
                w = 1;
            } else {
                t.coarseY = data >> 3;
                t.fineY = data & 0x07;
                w = 0;
            }
            break;
        case 0x2006:
            // PPUADDR
            if (w == 0) {
                t.high = data & 0x3F;
                w = 1;
            } else {
                t.low = data;
                v = t;
                w = 0;
            }
            break;
        case 0x2007:
            // PPUDATA
            ppuWrite(v.addr, data);

            // When the CPU writing from the PPU memory PPUADDR is increased by 1 or 32 depending on increment mode.
            v.addr += 0x01 + 0x19 * ppuctrl.incrementMode;
            break;
    }
}

void PPU::dmaWrite(uint8_t data) {
    ((uint8_t*)oam)[oamaddr] = data;
    dmaaddr++;
}