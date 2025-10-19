#ifndef H_CARTRIDGE
#define H_CARTRIDGE

#include <cstdint>
#include <vector>

#include "Mapper.h"

using std::uint16_t;
using std::uint8_t;

class Cartridge {
    public:
        void reset();
        uint8_t cpuRead(uint16_t addr);
        void cpuWrite(uint16_t addr, uint8_t data);
        uint8_t ppuRead(uint16_t addr);
        void ppuWrite(uint16_t addr, uint8_t data);
        uint16_t mirrorAddr(uint16_t addr);

        bool irq = false;
    private:
        Mapper mapper;
};

#endif // H_CARTRIDGE