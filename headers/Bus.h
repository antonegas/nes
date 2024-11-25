#ifndef H_BUS
#define H_BUS

#include <cstdint>

class Bus {
    public:
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
        void clock();
    
    private:
        uint8_t ram[2048];

        // Controllers
        // TODO: probably change to an abstract "controller" class which can implement more controllers than the normal NES controller.
        uint8_t controller1;
        uint8_t controller2;

        // PPU registers
        uint8_t ppuctrl;
        uint8_t ppumask;
        uint8_t ppustatus;
        uint8_t oamaddr;
        uint8_t oamdata;
        uint16_t ppuscroll;
        uint16_t ppuaddr;
        uint8_t ppudata;

        // APU registers
        uint8_t sq1_vol;
        uint8_t sq1_sweep;
        uint8_t sq1_lo;
        uint8_t sq1_hi;
        uint8_t sq2_vol;
        uint8_t sq2_sweep;
        uint8_t sq2_lo;
        uint8_t sq2_hi;
        uint8_t tri_linear;
        uint8_t tri_lo;
        uint8_t tri_hi;
        uint8_t noise_vol;
        uint8_t noise_lo;
        uint8_t noise_hi;
        uint8_t dmc_freq;
        uint8_t dmc_raw;
        uint8_t dmc_start;
        uint8_t dmc_len;
        uint8_t oamdma;
        uint8_t snd_chn;

        // I/O registers
        uint8_t joy1;
        uint8_t joy2;
};

#endif // H_BUS