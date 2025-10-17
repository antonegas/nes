#ifndef H_PPU
#define H_PPU

#include <cstdint>

using std::uint16_t;
using std::uint8_t;

class PPU {
    public:
        void tick();
        void reset();
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
        void dmaWrite(uint8_t data);
    private:
        /**
         * PPU REGISTERS
         * 
         * The PPU has some registers which can be written to and/or read from by the CPU: PPUCTRL, 
         * PPUMASK, PPUSTATUS, OAMADDR, OAMDATA, PPUSCROLL, PPUADDR, PPUDATA and OAMDMA.
         * 
         * PPUCTRL (VPHBSINN, write only): NMI enable (V), master slave/unused (P), sprite height 8x8 
         * or 8x16 (H), background tile select (B), sprite tile select (S), increment mode going 
         * across or down (I) and nametable select.
         * 
         * PPUMASK (BGRsbMmG, write only): enhanced blue/green/red (BGR), enable sprite (s), enable 
         * background (b), sprite enable in left column (M), background enable in left column (m) 
         * and grayscale (G).
         * 
         * PPUMASK (VSO-----, read only): vblank (V), sprite 0 hit (S) and sprite overflow (O).
         * 
         * OAMADDR (write only): Sprite RAM address.
         * OAMADATA (write only): Sprite RAM data.
         * PPUSCROLL (write only): X scroll then Y scroll with two writes.
         * PPUADDR (write only): Address where CPU will write to/read from.
         * PPUDATA (write): Writes to the PPUADDR and increments based on PPUCTRL increment mode.
         * PPUDATA (read): A buffered response for the given address, one read behind.
         * OAMDMA (write): Suspends CPU to allow PPU to copy a page from its memory at address 0xXX00.
         * 
         * Reference: https://www.nesdev.org/wiki/PPU_registers
         */

        union PPUCTRL {
            struct {
                uint8_t nametableSelect : 2;
                uint8_t incrementMode : 1;
                uint8_t spriteTileSelect : 1;
                uint8_t backgroundTileSelect : 1;
                uint8_t spriteHeight : 1;
                uint8_t unused : 1;
                uint8_t nmiEnable : 1;
            };
            uint8_t data = 0x00;
        } ppuctrl;

        union PPUMASK {
            struct {
                uint8_t grayscale : 1;
                uint8_t backgroundLeft : 1;
                uint8_t spriteLeft : 1;
                uint8_t enableBackground : 1;
                uint8_t enableSprite : 1;
                uint8_t emphasizeRed : 1;
                uint8_t emphasizeGreen : 1;
                uint8_t emphasizeBlue : 1;
            };
            uint8_t data = 0x00;
        } ppumask;

        union PPUSTATUS {
            struct {
                uint8_t unused : 5;
                uint8_t O : 1;
                uint8_t S : 1;
                uint8_t V : 1;
            };
            uint8_t data = 0x00;
        } ppustatus;

        /**
         * OBJECT ATTRIBUTE MEMORY (OAM)
         * 
         * The OAM contains information about sprites to be displayed. It stores y position sprite tile
         * attributes and x position.
         * 
         * The attributes (VHB111PP): vertical flip (V), horizontal flip (H), in front/behind background (B)
         * and sprite palette (P).
         * 
         * Reference: https://www.nesdev.org/wiki/PPU_OAM
         */
        
        uint8_t oamaddr = 0x00;
        uint8_t dmaaddr = 0x00;

        struct OAM {
            uint8_t y = 0x00;
            uint8_t tile = 0x00;
            uint8_t attr = 0x00;
            uint8_t x = 0x00;
        } oam[64];

        // uint16_t ppuscroll = 0x0000;
        uint16_t ppuaddr = 0x0000;
        // uint8_t ppudata; // NOTE: not a real address. Writes to vram at ppuaddr.

        // Latches for double write registers.
        bool ppuscrollLatch = false;
        bool ppuaddrLatch = false;

        // Last PPUSCROLL/PPUADDR writes.
        uint8_t ppuscrollLast = 0x00;
        uint8_t ppuaddrLast = 0x00;

        // Odd frame indicator.
        bool oddFrame = false;
};

#endif // H_PPU