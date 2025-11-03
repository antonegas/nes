#ifndef H_PPU
#define H_PPU

#include <cstdint>
#include <memory>
#include <array>

#include "Mapper.h"
#include "Palette.h"
#include "Screen.h"

using std::uint16_t;
using std::uint8_t;

class PPU {
    public:
        void tick();
        void power();
        void reset();
        void insertCart(std::shared_ptr<Mapper> cart);
        void connectScreen(std::shared_ptr<Screen<256, 240>> screen);
        uint8_t registerRead(uint16_t addr);
        void registerWrite(uint16_t addr, uint8_t data);
        void dmaWrite(uint8_t data);

        bool nmi = false;
    private:
        std::shared_ptr<Screen<256, 240>> screen;
        Palette palette;

        bool fblank(); // Forced blank.

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
                uint8_t nametable : 2;
                uint8_t incrementMode : 1;
                uint16_t spriteTable : 1;
                uint16_t backgroundTable : 1;
                uint8_t spriteHeight : 1;
                uint8_t unused : 1;
                bool nmiEnable : 1;
            };
            uint8_t reg = 0x00;
        } ppuctrl;

        union PPUMASK {
            struct {
                bool grayscale : 1;
                bool backgroundLeft : 1;
                bool spriteLeft : 1;
                bool enableBackground : 1;
                bool enableSprite : 1;
                bool emphasizeRed : 1;
                bool emphasizeGreen : 1;
                bool emphasizeBlue : 1;
            };
            uint8_t reg = 0x00;
        } ppumask;

        union PPUSTATUS {
            struct {
                uint8_t unused : 5;
                bool O : 1; // Sprite overflow.
                bool S : 1; // Sprite 0 hit.
                bool V : 1; // VBlank.
            };
            uint8_t status = 0x00;
        } ppustatus;

        uint8_t ppudataBuffer = 0x00;

        /**
         * INTERNAL REGISTERS
         * 
         * To facilitate scrolling the PPU has some internal registers. Current VRAM address (v), 
         * temporary VRAM address (t), fine x scroll (x) and write toggle (w).
         * 
         * Reference: https://www.nesdev.org/wiki/PPU_scrolling#PPU_internal_registers
         */

        union Loopy {
            struct {
                uint8_t coarseX : 5;
                uint8_t coarseY : 5;
                uint8_t nametable : 2;
                uint8_t fineY : 3;
                uint8_t unused : 1;
            };
            struct {
                uint8_t low;
                uint8_t high;
            };
            uint16_t addr = 0x0000;

            void incrementX();
            void incrementY();
        };
        
        Loopy v;
        Loopy t;
        uint8_t fineX = 0x00;
        bool w = 0x00;

        uint16_t tileAddr();
        uint16_t attrAddr();

        /**
         * MEMORY MAP
         * 
         * 0x0000-0x0FFF: Pattern table 0
         * 0x1000-0x1FFF: Pattern table 1
         * 0x2000-0x23BF: Nametable 0
         * 0x23C0-0x23FF: Attribute table 0
         * 0x2400-0x27BF: Nametable 1
         * 0x27C0-0x27FF: Attribute table 1
         * 0x2800-0x2BBF: Nametable 2
         * 0x2C00-0x2FBF: Attribute table 2
         * 0x2FC0-0x2FFF: Name table 3
         * 0x3000-0x3EFF: Attribute table 3
         * 0x3F00-0x3F1F: Palette RAM
         * 0x3F20-0x3FFF: Mirrors of 0x3F00-0x3F1F
         * 
         * Reference: https://www.nesdev.org/wiki/PPU_memory_map
         */

        std::shared_ptr<Mapper> cart;
        // NOTE: Only 2kB on actual hardware but 4kb here to allow 4-screen mirroring.
        std::array<uint8_t, 0x1000> vram;
        std::array<uint8_t, 0x20> paletteRam;

        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t data);

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

        typedef struct {
            uint8_t y = 0x00;
            uint8_t tile = 0x00;
            union Attribute {
                struct {
                    uint8_t pal : 2;
                    uint8_t unused : 3;
                    uint8_t prio : 1;
                    uint8_t flipH : 1;
                    uint8_t flipV : 1;
                };
                uint8_t data = 0x00;
            } attr;
            uint8_t x = 0x00;
        } OAM;

        std::array<OAM, 64> primaryOam;
        std::array<OAM, 8> secondaryOam;

        /**
         * MOTION PICTURE BUFFER MEMORY (MPBM)
         * 
         * The MPBM contains information about the sprites to render on the current line. It stores the
         * low and high bits of the sprites pixel on the scanline, palette and priority attribute data and 
         * the x position of the sprite.
         * 
         * The x position is decremented each dot until it reaches zero, when rendering of the sprite can
         * start. After the x position reaches zero the low and high bits of the sprite line are shifted 
         * each dot. 
         * 
         * Reference: https://github.com/emu-russia/breaks/blob/master/BreakingNESWiki_DeepL/PPU/fifo.md
         * NesDev reference: https://forums.nesdev.org/viewtopic.php?t=26291
         */

        typedef struct {
            uint8_t low = 0x00;
            uint8_t high = 0x00;
            uint8_t pal : 2 = 0x00;
            bool prio : 1 = false;
            uint8_t unused : 5 = 0x00;
            uint8_t x = 0x00;
        } MPBM;

        std::array<MPBM, 8> mpbm;

        /**
         * RENDERING
         * 
         * TODO: Document
         * 
         * Reference: https://www.nesdev.org/wiki/PPU_rendering
         * Sprite evaluation reference: https://www.nesdev.org/wiki/PPU_sprite_evaluation
         */

        uint16_t scanline = 0x00;
        uint16_t dot = 0x00;
        bool odd = false;

        uint8_t nextTile = 0x00;
        uint8_t nextAttr = 0x00;
        uint8_t nextPatternLow = 0x00;
        uint8_t nextPatternHigh = 0x00;

        uint16_t shifterPatternLow = 0x0000;
        uint16_t shifterPatternHigh = 0x0000;
        uint16_t shifterPalLow = 0x0000;
        uint16_t shifterPalHigh = 0x0000;

        uint8_t primaryPtr = 0x00;
        uint8_t secondaryPtr = 0x00;

        bool hasSprite0Next = false;
        bool hasSprite0Current = false;

        void tickVisibleFrame();
        void tickPreRender();
        void drawDot();
        void fetchBackground();
        void fetchForeground();
        uint16_t spriteAddr(OAM tile);
        void updateShifters();
};

#endif // H_PPU