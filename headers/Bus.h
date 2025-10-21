#ifndef H_BUS
#define H_BUS

#include <cstdint>
#include <memory>
#include <array>

#include "CPU.h"
#include "PPU.h"
#include "APU.h"
#include "BaseController.h"
#include "Mapper.h"

using std::uint16_t;
using std::uint8_t;

class Bus {
    public:
        // TODO: Bus constructor should set the bus pointer of the cpu to itself
        void tick();
        void power();
        void reset();
        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t data);
    private:
        uint8_t cycle = 0x00; // Master clock modolo CPU * PPU master clocks / clock.

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
        
        std::array<uint8_t, 0x0800> ram;
        CPU cpu;
        PPU ppu;
        APU apu;
        std::array<BaseController, 2> controllers;
        std::shared_ptr<Mapper> cart;

        /**
         * DIRECT MEMORY ACCESS (DMA)
         * 
         * Writing directly to the PPU's OAM using OAMADDR and OAMDATA is slow but there is one more
         * way to transfer sprites to the PPU. DMA or direct memory access allows for halting the 
         * CPU to then transfer an entire page in RAM to the OAM memory.
         * 
         * Reference: https://www.nesdev.org/wiki/PPU_registers#OAMDMA
         */

        bool dmaActive = false;
        bool dmaRead = false; // If DMA wants to read or write.
        bool dmaWait = false;
        uint8_t dmaPage = 0x00;
        uint8_t dmaLower = 0x00;
        uint8_t dmaData = 0x00;

        void dmaInit(uint8_t page);
        void dmaTransfer();
};

#endif // H_BUS