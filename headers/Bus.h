#ifndef H_BUS
#define H_BUS

#include <cstdint>

#include "CPU.h"
#include "PPU.h"
#include "APU.h"

class Bus {
    public:
        // TODO: Bus constructor should set the bus pointer of the cpu to itself
        // TODO: Make sure that read and write access the correct register/memory as it can be different between read and write
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
        void clock(); 
    private:
        uint8_t ram[2048];
        CPU cpu;
        PPU ppu;
        APU apu;

        // Controllers
        // TODO: probably change to an abstract "controller" class which can implement more controllers than the normal NES controller.
        uint8_t controller1;
        uint8_t controller2;

        // I/O registers
        uint8_t joy1;
        uint8_t joy2;
};

#endif // H_BUS