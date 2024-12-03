#ifndef H_PPU
#define H_PPU

#include <cstdint>

class PPU {
    public:
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
        void clock();
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