#include <cstdint>

#include "PPU.h"

using std::uint16_t;
using std::uint8_t;

void PPU::tick() {
    if (scanline <= 239) {
        // Visible frame.
        tickVisibleFrame();
    } else if (scanline == 241 && dot == 1) {
        // Set vblank.
        // NOTE: Scanline 291 on Dendy
        ppustatus.V = true;
        nmi = ppuctrl.nmiEnable;
    } else if (scanline <= 260) {
        // VBlank.
        // NOTE: Scanline 310 on PAL/Dendy
    } else {
        // Pre-render scanline.
        // Scanline 261 on NTCS
        // NOTE: Scanline 311 on PAL/Dendy
        tickPreRender();
    }

    dot++;

    if (dot == 341) {
        dot = 0;
        scanline++;
    }

    // NOTE: Scanline 312 on PAL/Dendy
    if (scanline < 262) return;
    scanline = 0;

    // First dot is skipped on even frames.
    if (odd) dot = 1;
    odd = !odd;
}

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

void PPU::insertCart(std::shared_ptr<Mapper> cart) {
    this->cart = cart;
}

void PPU::connectScreen(std::shared_ptr<Screen<256, 240>> screen) {
    this->screen = screen;
}

void PPU::setPalette(Palette palette) {
    this->palette = palette;
}

uint8_t PPU::registerRead(uint16_t addr) {
    switch (addr) {
        case 0x2002: {
            // PPUSTATUS
            uint8_t status = ppustatus.status & 0xE0;

            // Reading PPUSTATUS has side effects.
            ppustatus.V = false;
            w = 0;

            return status;
        }
        case 0x2004:
            // OAMDATA
            if (scanline <= 239 && 1 <= dot && dot <= 64 && !fblank()) {
                // Secondary OAM clear.
                return 0xFF;
            }

            return ((uint8_t*)primaryOam.data())[oamaddr];
        case 0x2007:{
            // PPUDATA
            uint8_t data = ppudataBuffer;
            ppudataBuffer = read(v.addr);

            // On PAL, 2C02G and 2C02H palette reads are immediate.
            // if (v.addr >= 0x3F00 && v.addr < 0x4000) data = ppudataBuffer;

            // When the CPU reads from the PPU memory PPUADDR is increased by 1 or 32 depending on 
            // increment mode.
            v.addr += 0x01 + 0x19 * ppuctrl.incrementMode;

            return data;
        }
        default:
            // Never reached.
            return 0x00;
    }
}

void PPU::registerWrite(uint16_t addr, uint8_t data) {
    switch (addr) {
        case 0x2000:
            // PPUCTRL
            ppuctrl.reg = data;
            t.nametable = ppuctrl.nametable;
            break;
        case 0x2001:
            // PPUMASK
            ppumask.reg = data;
            palette.setEmphasis(
                ppumask.emphasizeRed,
                ppumask.emphasizeGreen,
                ppumask.emphasizeBlue
            );
            break;
        case 0x2003:
            // OAMADDR
            oamaddr = data;
            break;
        case 0x2004:
            // OAMDATA
            ((uint8_t*)primaryOam.data())[oamaddr] = data;
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

            // When the CPU writing from the PPU memory PPUADDR is increased by 1 or 32 depending 
            // on increment mode.
            v.addr += 0x01 + 0x19 * ppuctrl.incrementMode;
            break;
    }
}

void PPU::dmaWrite(uint8_t data) {
    ((uint8_t*)primaryOam.data())[oamaddr] = data;
    dmaaddr++;
}

bool PPU::fblank() {
    return !ppumask.enableBackground && !ppumask.enableSprite;
}

// NOTE: Current implementation of read and write assumes that only pattern tables are mapped 
// entirely by mapper.
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

        return paletteRam[addr];
    } else {
        return 0x00;
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

        paletteRam[addr] = data;
    }
}

void PPU::Loopy::incrementX() {
    coarseX++;
    // If wrapped around switch horizontal nametable.
    if (coarseX == 0) nametable = nametable ^ 0x01;
};

void PPU::Loopy::incrementY() {
    fineY++;
    if (fineY != 0) return;
    if (coarseY == 29) {
        coarseY = 0;
        nametable = nametable ^ 0x02;
    } else if (coarseY == 31) {
        coarseY = 0;
    } else {
        coarseY++;
    }
};

uint16_t PPU::tileAddr() {
    return 0x2000 | (v.addr & 0x0FFF);
}

uint16_t PPU::attrAddr() {
    return 0x23C0 | (v.addr & 0x0C00) | (v.coarseY & 0x1C << 1) | (v.coarseX & 0x1C >> 2);
}

void PPU::tickVisibleFrame() {
    // Display a finished frame on the screen.
    if (scanline == 239 && dot == 255) screen->swap();

    if (fblank()) {
        drawDot();
        return;
    }

    updateShifters();
    fetchBackground();
    drawDot();


    // NOTE: Since foreground is fetched at the end of the scanline sprites are drawn one pixel 
    // lower than what is specified in the OAM.
    fetchForeground();
}

void PPU::tickPreRender() {
    if (fblank()) return;

    updateShifters();
    fetchBackground();
    
    if (dot == 1) {
        ppustatus.V = false;
        ppustatus.S = false;
        ppustatus.O = false;
    }

    if (280 <= dot && dot <= 304) {
        // Set v.Y = t.Y
        v.coarseY = t.coarseY;
        v.nametable = (t.nametable & 0x02) | (v.nametable & 0x01);
        v.fineY = t.fineY;
    }
}

void PPU::drawDot() {
    uint8_t background = 0x00;
    uint8_t foreground = 0x00;
    uint8_t output = 0x00;
    bool isForegroundSprite0 = false;
    bool priority;

    bool isBackgroundEnabled = ppumask.enableBackground && (dot >= 8 || ppumask.backgroundLeft);
    bool isForegroundEnabled = ppumask.enableSprite && (dot >= 8 || ppumask.spriteLeft);

    // Get background pixel value.
    if (isBackgroundEnabled) {
        uint16_t selected = 0x8000 >> fineX;
        uint8_t backgroundLow = selected & shifterPatternLow != 0x0000;
        uint8_t backgroundHigh = selected & shifterPatternHigh != 0x0000;
        uint8_t backgroundPalLow = selected & shifterPalLow != 0x0000;
        uint8_t backgroundPalHigh = selected & shifterPalHigh != 0x0000;
        uint8_t backgroundPal = (backgroundPalHigh << 1) | backgroundPalLow;
        
        background = (backgroundPal << 2) | (backgroundHigh << 1) | backgroundLow;
    }

    // Get foreground pixel value.
    if (isForegroundEnabled) {
        uint8_t foregroundLow = 0x00;
        uint8_t foregroundHigh = 0x00;

        for (size_t i = 0; i < 8; i++) {
            if (mpbm[i].x > 0) continue;

            foregroundLow = mpbm[i].low & 0x80 != 0x00;
            foregroundHigh = mpbm[i].high & 0x80 != 0x00;

            foreground = (foregroundHigh << 1) | foregroundLow;

            if (foreground != 0x00) {
                foreground = 0x10 | (mpbm[i].pal << 2) | foreground;
                isForegroundSprite0 = i == 0;
                priority = mpbm[i].prio;
                break;
            }; 
        }
    }
    
    // Get which palette index to output.
    if (background & 0x03 != 0x00 && foreground & 0x03 != 0x00) {
        // Set sprite 0 hit flag.
        ppustatus.S = ppustatus.S | (hasSprite0Current & isForegroundSprite0);

        if (priority) {
            output = background;
        } else {
            output = foreground;
        }
    } else if (background & 0x03 != 0x00) {
        output = background;
    } else if (foreground & 0x03 != 0x00) {
        output = foreground;
    }
    
    // Grayscale forces output color to be white/gray by AND:ing with 0x30.
    if (ppumask.grayscale) output = output & 0x30;

    // Output dot to screen.
    uint8_t r = palette.getR(output);
    uint8_t g = palette.getG(output);
    uint8_t b = palette.getB(output);


    screen->put(dot, scanline, r, g, b);
}

void PPU::fetchBackground() {
    if (dot == 0) return;

    if (dot == 257) {
        // Set v.X = t.X
        v.coarseX = t.coarseX;
        v.nametable = (v.nametable & 0x02) | (t.nametable & 0x01);
    }
    
    if ((257 <= dot && dot <= 320) || (337 <= dot && dot <= 340)) {
        // Unused/ignored fetches (needed for some mappers).
        switch (dot & 0x0007) {
            case 0x0001:
                // Unused fetch.
                nextTile = read(tileAddr());
            return;
            case 0x0003:
                // Ignored fetch.
                read(tileAddr());
            return;
                default:
            return;
        }
    }

    // Dot 1-256 and 321-236
    switch (dot & 0x0007) {
        case 0x0000:
            v.incrementX();
            
            // Load shifters.
            shifterPatternLow = shifterPatternLow | nextPatternLow;
            shifterPatternHigh = shifterPalHigh | nextPatternHigh;

            if (nextAttr & 0x01) {
                shifterPalLow = shifterPalLow | 0x00FF;
            }
            if (nextAttr & 0x02) {
                shifterPalHigh = shifterPalHigh | 0x00FF;
            }
            return;
        case 0x0001:
            nextTile = read(tileAddr());
            return;
        case 0x0003:
            nextAttr = read(attrAddr());
            return;
        case 0x0005:
            nextPatternLow = read(
                (ppuctrl.backgroundTable << 12) |
                (nextTile << 4) |
                v.fineY
            );
            return;
        case 0x0007:
            nextPatternHigh = read(
                (ppuctrl.backgroundTable << 12) |
                (nextTile << 4) |
                (v.fineY + 8)
            );
            return;
        default:
            return;
    }
}

void PPU::fetchForeground() {
    if (dot <= 64) {
        if (dot & 0x0001 == 0x0000) ((uint8_t*)secondaryOam.data())[(dot - 2) >> 1] = 0xFF;
    } else if (dot <= 256) {
        // Read on odd cycles.
        if (dot & 0x0001) return;

        // All sprites searched or all sprites found and overflow set.
        // NOTE: Some behavior where primary and secondary pointers should increment has been left out by this.
        if (primaryPtr >= 0x0100 || ppustatus.O) return; 

        // If current sprite was in range copy it to secondary OAM.
        if (secondaryPtr & 0x03 != 0x00) {
            ((uint8_t*)secondaryOam.data())[secondaryPtr] = ((uint8_t*)primaryOam.data())[primaryPtr];

            // Move to next field in both OAMs.
            secondaryPtr += 0x01;
            primaryPtr += 0x01;
            return;
        }

        // Check if current primary OAM pointer is in range if interpreted as a y coordinate.
        uint8_t y = ((uint8_t*)primaryOam.data())[primaryPtr];
        bool inRange = y <= scanline && scanline <= y + 0x07 + 0x08 * ppuctrl.spriteHeight;

        // If all sprites has been found check for sprite overflow.
        if (secondaryPtr >= 0x20) {
            ppustatus.O = inRange;

            // Sprite overflow bug. Increment both entry pointer and field pointer.
            primaryPtr += 0x05;
            return;
        }
        
        // Copy y coordinate of current primary OAM sprite into secondary OAM.
        ((uint8_t*)secondaryOam.data())[secondaryPtr] = ((uint8_t*)primaryOam.data())[primaryPtr];

        // If the y coordinate is in range copy the other fields to secondary OAM.
        if (inRange) {
            // If it is the first entry in the primary OAM it is sprite 0.
            if (primaryPtr == 0x00) hasSprite0Next = true;

            // Move to next field in both OAMs.
            secondaryPtr += 0x01;
            primaryPtr += 0x01;
            return;
        } 

        // Move to next primary OAM entry.
        primaryPtr += 0x04;
        return;
    } else if (dot <= 320) {
        // NOTE: Skips some reads of the secondary OAM.
        uint8_t entry = (dot - 257) >> 3;

        switch (dot & 0x0007) {
            case 0x0003:
                // Attribute data.
                mpbm[entry].pal = secondaryOam[entry].attr.pal;
                mpbm[entry].prio = secondaryOam[entry].attr.prio;
                mpbm[entry].unused = 0x00;
                return;
            case 0x0004:
                // X coordinate.
                mpbm[entry].x = secondaryOam[entry].x;
                return;
            case 0x0005: {
                uint16_t addr = spriteAddr(secondaryOam[entry]);
                uint8_t low = read(addr);

                if (secondaryOam[entry].attr.flipH) {
                    low = (low & 0xF0) >> 4 | (low & 0x0F) << 4;
                    low = (low & 0xCC) >> 2 | (low & 0x33) << 2;
                    low = (low & 0xAA) >> 1 | (low & 0x55) << 1;
                }

                mpbm[entry].low = low;
                return;
            }
            case 0x0007: {
                uint16_t addr = spriteAddr(secondaryOam[entry]);
                uint8_t high = read(addr + 0x0008);

                if (secondaryOam[entry].attr.flipH) {
                    high = (high & 0xF0) >> 4 | (high & 0x0F) << 4;
                    high = (high & 0xCC) >> 2 | (high & 0x33) << 2;
                    high = (high & 0xAA) >> 1 | (high & 0x55) << 1;
                }

                mpbm[entry].high = high;
                return;
            }
        }
    } else if (dot <= 340) {
        // Busy reading first byte of secondary OAM.
        
        if (dot == 340) {
            // Reset helpers.
            primaryPtr = 0x00;
            secondaryPtr = 0x00;

            // Set sprite 0 hit flags.
            hasSprite0Current = hasSprite0Next;
            hasSprite0Next = false;
        }
    }

    // Sprite 0 hits can't happen on dot 255.
    if (dot == 254) hasSprite0Current = false;
}

uint16_t PPU::spriteAddr(OAM sprite) {
    uint16_t addr = 0x0000;
    uint8_t relativeY = scanline - sprite.y;

    if (ppuctrl.spriteHeight) {
        // 8x16 sprites.
        addr = addr | ((sprite.tile & 0x01) << 12);
        addr = addr | ((sprite.tile & 0xFE) << 4);

        if (sprite.attr.flipV) {
            addr = addr | ((relativeY < 8) << 4);
        } else {
            addr = addr | ((relativeY >= 8) << 4);
        }
    } else {
        // 8x8 sprites.
        addr = addr | ((ppuctrl.spriteTable) << 12);
        addr = addr | (sprite.tile << 4);
    }

    if (sprite.attr.flipV) {
        addr = addr | (7 - (relativeY & 0x07));
    } else {
        addr = addr | (relativeY & 0x07);
    }

    return addr;
}

void PPU::updateShifters() {
    if (dot <= 1) return;
    if (258 <= dot && dot <= 320) return;

    // Background shifters.
    if (dot <= 337) {
        shifterPatternLow = shifterPatternLow << 1;
        shifterPatternHigh = shifterPatternHigh << 1;
        shifterPalLow = shifterPalLow << 1;
        shifterPalHigh = shifterPalHigh << 1;
    }

    // Foreground shifters.
    if (dot <= 257) {
        for (size_t i = 0; i < mpbm.size(); i++) {
            if (mpbm[i].x > 0) {
                mpbm[i].x--;
            } else {
                mpbm[i].high = mpbm[i].high << 1;
                mpbm[i].low = mpbm[i].low << 1;
            }
        }
    }
}