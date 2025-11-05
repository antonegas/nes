#include <cstdint>

#include "APU.h"

using std::uint16_t;
using std::uint8_t;

uint8_t APU::read(uint16_t addr)
{
    switch (addr) {
        case 0x4000:
            return sq1_vol;
            break;
        case 0x4001:
            return sq1_sweep;
            break;
        case 0x4002:
            return sq1_lo;
            break;
        case 0x4003:
            return sq1_hi;
            break;
        case 0x4004:
            return sq2_vol;
            break;
        case 0x4005:
            return sq2_sweep;
            break;
        case 0x4006:
            return sq2_lo;
            break;
        case 0x4007:
            return sq2_hi;
            break;
        case 0x4008:
            return tri_linear;
            break;
        case 0x4009:
            return 0x00; // unused
            break;
        case 0x400A:
            return tri_lo;
            break;
        case 0x400B:
            return tri_hi;
            break;
        case 0x400C:
            return noise_vol;
            break;
        case 0x400D:
            return 0x00; // unused
            break;
        case 0x400E:
            return noise_lo;
            break;
        case 0x400F:
            return noise_hi;
            break;
        case 0x4010:
            return dmc_freq;
            break;
        case 0x4011:
            return dmc_raw;
            break;
        case 0x4012:
            return dmc_start;
            break;
        case 0x4013:
            return dmc_len;
            break;
        case 0x4014:
            return oamdma;
            break;
        case 0x4015:
            return snd_chn;
            break;
        default:
            return 0x00;
            break;
    }
}