#include <cstdint>

#include "../headers/Bus.h"
#include "../headers/CPU.h"
#include "../headers/APU.h"
#include "../headers/BaseController.h"

using std::uint16_t;
using std::uint8_t;

void Bus::tick() {
    // PAL
    uint8_t ppurate = 5;
    uint8_t cpurate = 16;
    uint8_t offset = 0;

    // NTSC
    // uint8_t ppurate = 4;
    // uint8_t cpurate = 12;
    // uint8_t offset = 0;

    // No need to increase master cycle over CPU * PPU master clocks / clock.
    cycle = cycle % (ppurate * cpurate);

    // Tick CPU and/or PPU if the current master cycle lines up with their clock rate.
    if ((cycle + offset) % cpurate == 0) cpu.tick();
    if (cycle % ppurate == 0) ppu.tick();

    // Tick the master clock once.
    cycle++;
}

uint8_t Bus::read(uint16_t address) {
    if (address <= 0x1FFF) {
        // RAM
        return ram[address & 0x7FF];
    } else if (address <= 0x3FFF) {
        // PPU registers.
        return ppu.read(address & 0x2007);
    } else if (address <= 0x4015) {
        // APU and I/O registers.
        // TODO: 0x4017 can also be an APU address
        return apu.read(address);
    } else if (address <= 0x4017) {
        switch (address) {
            case 0x4016:
                return controllers[0].read();
                break;
            case 0x4017:
                return controllers[1].read();
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
    if (address <= 0x1FFF) {
        // RAM
        ram[address & 0x7FF] = data;
    } else if (address <= 0x3FFF) {
        // PPU registers.
        ppu.write(address & 0x2007, data);
    } else if (address <= 0x4015) {
        // TODO: move to APU and I/O classes
        // APU and I/O registers.
        // TODO: 0x4017 can also be an APU address
        apu.write(address, data);
    } else if (address <= 0x4017) {
        if (address == 0x4016) {
            controllers[0].reload();
            controllers[1].reload();
        }
    } else if (address <= 0xFFFF) {
        // TODO: this is probably the cartridge
    }
}