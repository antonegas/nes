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
        // TODO: Make sure that read and write access the correct register/memory as it can be different between read and write
        void tick(); 
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
    private:
        // Cycle of the master clock modolo CPU * PPU master clocks / clock.
        uint8_t cycle = 0x00;

        uint8_t ram[2048];
        CPU cpu;
        PPU ppu;
        APU apu;

        // Controllers.
        BaseController controllers[2];
};

#endif // H_BUS