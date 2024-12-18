#include "../headers/Bus.h"

uint8_t Bus::read(uint16_t address) {
    uint16_t masked_address; // Used when addresses are repeated

    if (address <= 0x1FFF) {
        // RAM
        return ram[address & 0x7FF];
    } else if (address <= 0x3FFF) {
        // PPU registers
        masked_address = address & 0x2007;
        return ppu.read(masked_address);
    } else if (address <= 0x4015) {
        // APU and I/O registers
        // TODO: 0x4017 can also be an APU address
        return apu.read(address);
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
    } else if (address <= 0xFFFF) {
        // TODO: this is probably cartridge
        return 0;
    }
}

void Bus::write(uint16_t address, uint8_t data) {
    uint16_t masked_address; // Used when addresses are repeated

    if (address <= 0x1FFF) {
        // RAM
        ram[address & 0x7FF] = data;
    } else if (address <= 0x3FFF) {
        // PPU registers
        masked_address = address & 0x2007;
        ppu.write(masked_address, data);
    } else if (address <= 0x4015) {
        // TODO: move to APU and I/O classes
        // APU and I/O registers
        // TODO: 0x4017 can also be an APU address
        apu.write(address, data);
    } else if (address <= 0x4017) {
        // This is not joy/controller when writings
        // switch (address) {
        // case 0x4016:
        //     // based on standard nes controller
        //     bool data = (controller1 & 0x80) > 0;
        //     controller1 = controller1 << 1;
        //     return data;
        //     break;
        // case 0x4017:
        //     bool data = (controller2 & 0x80) > 0;
        //     controller2 = controller2 << 1;
        //     return data;
        //     break;
        // default:
        //     break;
        // }
    } else if (address <= 0xFFFF) {
        // TODO: this is probably the cartridge
    }
}

void Bus::clock();