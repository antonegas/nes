#include "../headers/Bus.h"

uint8_t Bus::read(uint16_t address) {
    uint16_t masked_address; // Used when addresses are repeated

    if (address <= 0x1FFF) {
        // RAM
        return ram[address & 0x7FF];
    } else if (address <= 0x3FFF) {
        // TODO: move to PPU classes
        // PPU registers
        masked_address = address & 0x2007;
        switch (masked_address) {
        case 0x2000:
            return ppuctrl;
            break;
        case 0x2001:
            return ppumask;
            break;
        case 0x2002:
            return ppustatus;
            break;
        case 0x2003:
            return oamaddr;
            break;
        case 0x2004:
            return oamdata;
            break;
        case 0x2005:
            return ppuscroll;
            break;
        case 0x2006:
            return ppuaddr;
            break;
        case 0x2007:
            return ppudata;
            break;
        default:
            return 0x00;
            break;
        }
    } else if (address <= 0x4015) {
        // TODO: move to APU and I/O classes
        // APU and I/O registers
        switch (address) {
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
    } else if (address <= 0x4017) {
        switch (address) {
        case 0x4016:
            // based on standard nes controller
            bool data = (controller1 & 0x80) > 0;
            controller1 = controller1 << 1;
            return data;
            break;
        case 0x4017:
            bool data = (controller2 & 0x80) > 0;
            controller2 = controller2 << 1;
            return data;
            break;
        default:
            break;
        }
    } else if (address <= 0x401F) {
        // Disabled
    } else if (address <= 0xFFFF) {
        // Cartridge
    }
}

void Bus::write(uint16_t address, uint8_t data) {
    if (address <= 0x1FFF) {
        // RAM
    } else if (address <= 0x3FFF) {
        // PPU registers
    } else if (address <= 0x4017) {
        // APU and I/O registers
    } else if (address <= 0x401F) {
        // Disabled
    } else if (address <= 0xFFFF) {
        // Cartridge
    }
}

void Bus::clock();