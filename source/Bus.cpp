#include <cstdint>
#include <memory>

#include "Bus.h"

using std::uint64_t;
using std::uint16_t;
using std::uint8_t;

Bus::Bus() {
    this->cpu.bus = this;
}

void Bus::update(uint64_t time) {
    if (previousTime == 0x0000000000000000) {
        previousTime = time;
        return;
    }

    uint64_t passed = time - previousTime;

    // NTSC (21.477272 MHz +/- 40 Hz)
    uint64_t mainClock = 21477272;

    // PAL/Dendy (26.601712 MHz +/- 50 Hz)
    // uint32_t mainClock = 26601712;

    // Calculate how many cycles has passed.
    uint64_t nanoCycles = (passed * mainClock + remainingCycles);
    uint64_t cycles = nanoCycles / 1000000000 / 12;

    // Set helper fields.
    remainingCycles = nanoCycles - cycles * 1000000000 * 12;
    previousTime = time;

    // Tick the bus for the amount of cycles passed since last update.
    while (cycles--) {
        tick();
    }
}

void Bus::tick() {
    if (!cartInserted) return;

    uint8_t ppurate = 1;
    uint8_t cpurate = 3;

    // NTSC (3 dots / CPU cycle)
    // uint8_t ppurate = 4;
    // uint8_t cpurate = 12;

    // PAL (3.2 dots / CPU cycle)
    // uint8_t ppurate = 5;
    // uint8_t cpurate = 16;

    // Dendy (3 dots / CPU cycle)
    // uint8_t ppurate = 5;
    // uint8_t cpurate = 15;

    uint8_t offset = 0;

    // No need to increase main cycle over CPU * PPU main clocks / clock.
    cycle = cycle % (ppurate * cpurate);

    // Tick CPU and/or PPU if the current main cycle lines up with their clock rate.
    if ((cycle + offset) % cpurate == 0) {
        cpu.tick();

        // If DMA is active move data to PPU.
        if (dmaActive) dmaTransfer();
    }
    if (cycle % ppurate == 0) ppu.tick();

    // If the PPU has indicated an NMI one should be triggered on the CPU.
    if (ppu.nmi) cpu.delay(&CPU::nmi);
    ppu.nmi = false;

    // Tick the main clock once.
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

void Bus::insertCart(std::shared_ptr<Mapper> cart) {
    this->cart = cart;
    ppu.insertCart(cart);
    cartInserted = true;
    ppu.power();
    cpu.power();
}

void Bus::connectScreen(std::shared_ptr<Screen<256, 240>> screen) {
    ppu.connectScreen(screen);
}

void Bus::connectController(std::shared_ptr<BaseController> controller, uint16_t addr) {
    controllers[addr & 0x0001] = controller;
}

void Bus::setPalette(Palette palette) {
    ppu.setPalette(palette);
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
        return apu.read(addr);
    } else if (addr == 0x4015) {
        // APU status.
        return apu.read(addr);
    } else if (addr <= 0x4017) {
        // Joycons.
        if (controllers[addr & 0x0001]) {
            return controllers[addr & 0x0001]->read();
        } else {
            return 0x00;
        }
    } else if (addr <= 0xFFFF) {
        // Read from cartridge
        if (cart) {
            return cart->cpuRead(addr);
        } else {
            return 0x00;
        }
    } else {
        // Never reached.
        return 0x00;
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
        if (controllers[0]) controllers[0]->reload();
        if (controllers[1]) controllers[1]->reload();
    } else if (addr == 0x4017) {
        // APU frame counter.
        apu.write(addr, data);
    } else if (addr <= 0xFFFF) {
        // Write to cartridge.
        if (cart) cart->cpuWrite(addr, data);
    }
}

void Bus::dmaInit(uint8_t page) {
    cpu.suspended = true;
    dmaRead = true;
    dmaWait = true;
    dmaActive = true;
    dmaPage = page;
    dmaLower = 0x00;
}

void Bus::dmaTransfer() {
    if (!dmaWait) {
        cpu.suspended = false;
        dmaActive = false;
        return;
    }

    // Wait one CPU and DMA disagree wait one CPU cycle.
    if (dmaRead && !cpu.dmaRead) return;
    if (!dmaRead && cpu.dmaRead) return;
    
    if (dmaRead && cpu.dmaRead) {
        dmaData = read((dmaPage << 8) | dmaLower);
        dmaRead = false;
        dmaLower++;
    }
    if (!dmaRead && !cpu.dmaRead) {
        ppu.dmaWrite(dmaData);
        dmaRead = true;

        // CPU can only be unhalted on DMA read cycles.
        if (dmaLower == 0x00) dmaWait = false;
    }
}