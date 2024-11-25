#ifndef H_PPU
#define H_PPU

#include <cstdint>

class PPU {
    public:
    private:
        // PPU registers
        uint8_t ppuctrl;
        uint8_t ppumask;
        uint8_t ppustatus;
        uint8_t oamaddr;
        uint8_t oamdata;
        uint16_t ppuscroll;
        uint16_t ppuaddr;
        uint8_t ppudata;
};

#endif // H_PPU