#ifndef H_BUS
#define H_BUS

#include <cstdint>

#include "CPU.h"
#include "PPU.h"
#include "APU.h"
#include "BaseController.h"

using std::uint16_t;
using std::uint8_t;

class Bus {
    public:
        // TODO: Bus constructor should set the bus pointer of the cpu to itself
        void tick(); 
        void reset();
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
    private:
        // Cycle of the master clock modolo CPU * PPU master clocks / clock.
        uint8_t cycle = 0x00;

        /**
         * MEMORY MAP
         * 
         * 0x0000-0x07FF: 2KB CPU RAM
         * 0x0800-0x1FFF: Mirrors of 0x0000-0x07FF
         * 0x2000-0x2007: PPU registers
         * 0x2008-0x3FFF: Mirrors of 0x2000-0x2007
         * 0x4000-0x4015: APU registers (0x4014 is OAM DMA high address)
         * 0x4016-0x4016: Joystick strobe/joystick 1 data
         * 0x4017-0x4017: APU frame counter/joystick 2 data
         * 0x4018-0x401F: Disabled APU and I/O functionality
         * 0x4020-0xFFFF: Cartridge/mapper address space
         * 
         * Reference: https://www.nesdev.org/wiki/CPU_memory_map
         */
        uint8_t ram[2048];
        CPU cpu;
        PPU ppu;
        APU apu;
        BaseController controllers[2];
};

#endif // H_BUS