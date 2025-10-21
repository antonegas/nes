#include <cstdint>

#include "../headers/Bus.h"

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

    // If the PPU has indicated an NMI one should be triggered on the CPU.
    if (ppu.nmi) cpu.delay(&CPU::nmi);
    ppu.nmi = false;

    // If DMA is active move data to PPU.
    if (dmaActive) dmaTransfer();

    // Tick the master clock once.
    cycle++;
}

void Bus::power() {
    cpu.power();
    ppu.power();
    apu.power();
}

void Bus::reset() {
    cpu.reset();
    ppu.reset();
    apu.reset();
}

uint8_t Bus::read(uint16_t addr) {
    if (addr <= 0x1FFF) {
        // CPU RAM.
        return ram[addr & 0x07FF];
    } else if (addr <= 0x3FFF) {
        // PPU registers.
        return ppu.registerRead(addr & 0x2007);
    } else if (addr <= 0x4013) {
        // APU registers.
        return apu.read(addr);
    } else if (addr == 0x4014) {
        // PPU OAM DMA.
    } else if (addr == 0x4015) {
        // APU status.
        return apu.read(addr);
    } else if (addr <= 0x4017) {
        // Joycons.
        return controllers[addr & 0x0001].read();
    } else if (addr <= 0xFFFF) {
        // Read from cartridge
        return cart->cpuRead(addr);
    }
}

void Bus::write(uint16_t addr, uint8_t data) {
    if (addr <= 0x1FFF) {
        // CPU RAM.
        ram[addr & 0x07FF] = data;
    } else if (addr <= 0x3FFF) {
        // PPU registers.
        ppu.registerWrite(addr & 0x2007, data);
    } else if (addr <= 0x4013) {
        // APU registers.
        apu.write(addr, data);
    } else if (addr == 0x4014) {
        // PPU OAMDMA.
        dmaInit(data);
    } else if (addr == 0x4015) {
        // APU status.
        apu.write(addr, data);
    } else if (addr == 0x4016) {
        // Joystick strobe.
        controllers[0].reload();
        controllers[1].reload();
    } else if (addr == 0x4017) {
        // APU frame counter.
        apu.write(addr, data);
    } else if (addr <= 0xFFFF) {
        // Write to cartridge.
        cart->cpuWrite(addr, data);
    }
}

void Bus::dmaInit(uint8_t page) {
    cpu.suspended = true;
    dmaRead = true;
    dmaWait = true;
    dmaPage = page;
}

void Bus::dmaTransfer() {
    if (!dmaWait) {
        cpu.suspended = false;
        return;
    }

    if (dmaRead && cpu.dmaRead) {
        dmaData = read((dmaPage << 8) | dmaLower);
        dmaRead = false;
    }
    if (!dmaRead && !cpu.dmaRead) {
        ppu.dmaWrite(dmaData);
        dmaRead = true;
        dmaLower++;
    }

    // CPU can only be unhalted on DMA read cycles.
    if (dmaLower == 0x00) dmaWait = false;
}