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
        // CPU RAM.
        return ram[address & 0x07FF];
    } else if (address <= 0x3FFF) {
        // PPU registers.
        return ppu.read(address & 0x2007);
    } else if (address <= 0x4013) {
        // APU registers.
        return apu.read(address);
    } else if (address == 0x4014) {
        // PPU OAM DMA.
    } else if (address == 0x4015) {
        // APU status.
        return apu.read(address);
    } else if (address <= 0x4017) {
        // Joycons.
        return controllers[address & 0x0001].read();
    } else if (address <= 0xFFFF) {
        // TODO: read from cartridge
        return 0x00;
    }
}

void Bus::write(uint16_t address, uint8_t data) {
    if (address <= 0x1FFF) {
        // CPU RAM.
        ram[address & 0x07FF] = data;
    } else if (address <= 0x3FFF) {
        // PPU registers.
        ppu.write(address & 0x2007, data);
    } else if (address <= 0x4013) {
        // APU registers.
        apu.write(address, data);
    } else if (address == 0x4014) {
        // TODO: PPU OAMDMA.
    } else if (address == 0x4015) {
        // APU status.
        apu.write(address, data);
    } else if (address == 0x4016) {
        // Joystick strobe.
        controllers[0].reload();
        controllers[1].reload();
    } else if (address == 0x4017) {
        // APU frame counter.
        apu.write(address, data);
    } else if (address <= 0xFFFF) {
        // TODO: write to cartridge
    }
}