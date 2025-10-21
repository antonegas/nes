#include <cstdint>

#include "../headers/PPU.h"

using std::uint16_t;
using std::uint8_t;

void PPU::power() {
    ppuctrl.reg = 0x0000;
    ppumask.reg = 0x0000;
    ppustatus.status = 0x0000;
    oamaddr = 0x0000;
    w = 0;
    v.addr = 0x0000;
    t.addr = 0x0000;
    odd = false;
    nmi = false;
    dmaaddr = 0x00;
}

void PPU::reset() {
    ppuctrl.reg = 0x0000;
    ppumask.reg = 0x0000;
    ppudataBuffer = 0x00;
    w = 0;
    t.addr = 0x0000;
    odd = false;
    nmi = false;
    dmaaddr = 0x00;
}

uint8_t PPU::registerRead(uint16_t addr) {
    switch (addr) {
        case 0x2002:
            // PPUSTATUS
            uint8_t status = ppustatus.status & 0xE0;

            // Reading PPUSTATUS has side effects.
            ppustatus.V = 0;
            w = 0;

            return status;
        case 0x2004:
            // OAMDATA
            return ((uint8_t*)oam.data())[oamaddr];
        case 0x2007:
            // PPUDATA
            uint8_t data = ppudataBuffer;
            ppudataBuffer = read(v.addr);

            // On PAL, 2C02G and 2C02H palette reads are immediate.
            // if (v.addr >= 0x3F00 && v.addr < 0x4000) data = ppudataBuffer;

            // When the CPU reads from the PPU memory PPUADDR is increased by 1 or 32 depending on increment mode.
            v.addr += 0x01 + 0x19 * ppuctrl.incrementMode;

            return data;
        default:
            return 0x00;
    }
}

void PPU::registerWrite(uint16_t addr, uint8_t data) {
    switch (addr) {
        case 0x2000:
            // PPUCTRL
            ppuctrl.reg = data;
            t.nametableSelect = ppuctrl.nametableSelect;
            break;
        case 0x2001:
            // PPUMASK
            ppumask.reg = data;
            break;
        case 0x2003:
            // OAMADDR
            oamaddr = data;
            break;
        case 0x2004:
            // OAMDATA
            ((uint8_t*)oam.data())[oamaddr] = data;
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
            write(v.addr, data);

            // When the CPU writing from the PPU memory PPUADDR is increased by 1 or 32 depending on increment mode.
            v.addr += 0x01 + 0x19 * ppuctrl.incrementMode;
            break;
    }
}

void PPU::dmaWrite(uint8_t data) {
    ((uint8_t*)oam.data())[oamaddr] = data;
    dmaaddr++;
}

// NOTE: Current implementation of read and write assumes that only pattern tables are mapped entirely by mapper.
uint8_t PPU::read(uint16_t addr) {
    addr = addr & 0x3FFF; // PPU addresses are 14 bits.

    if (addr <= 0x1FFF) {
        return cart->ppuRead(addr);
    } else if (addr <= 0x3EFF) {
        return vram[cart->mirrorAddr(addr)];
    } else if (addr <= 0x3FFF) {
        addr = addr & 0x001F;

        // Every 4:th byte is mapped to 0x00 of the palette RAM.
        if (addr & 0x000F == 0x0000) addr = 0x0000; 

        return palette[addr];
    }
}

void PPU::write(uint16_t addr, uint8_t data) {
    addr = addr & 0x3FFF; // PPU addresses are 14 bits.

    if (addr <= 0x1FFF) {
        cart->ppuWrite(addr, data);
    } else if (addr <= 0x3EFF) {
        vram[cart->mirrorAddr(addr)] = data;
    } else if (addr <= 0x3FFF) {
        addr = addr & 0x001F;

        // Every 4:th byte is mapped to 0x00 of the palette RAM.
        if (addr & 0x000F == 0x0000) addr = 0x0000; 

        palette[addr] = data;
    }
}