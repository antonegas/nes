#ifndef H_APU
#define H_APU

#include <cstdint>

using std::uint16_t;
using std::uint8_t;

class APU {
    public:
        void tick();
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
    private:
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
};

#endif // H_APU