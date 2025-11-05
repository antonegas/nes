#ifndef H_NROM
#define H_NROM

#include <cstdint>
#include <vector>

#include "../Mapper.h"
#include "../constants.h"

using std::uint16_t;
using std::uint8_t;

/**
 * NROM (Mapper 0)
 * 
 * A simple mapper with 16 or 32 KiB PRG-ROM, 8 KiB CHR-ROM and hardwired nametable
 * mirroring. If the PRG-ROM is 16 KiB it is mirrored across 0xC000-0xFFFF.
 * 
 * Reference: https://www.nesdev.org/wiki/NROM
 */

class NROM : Mapper {
    public:
        NROM(
            std::vector<uint8_t> prgrom, 
            std::vector<uint8_t> chrrom, 
            NametableLayout mirrorMode
        ) : Mapper(prgrom, chrrom, mirrorMode) {};

        static uint16_t const number = 0x0000;

        virtual uint8_t cpuRead(uint16_t addr);
        virtual uint8_t ppuRead(uint16_t addr);
    private:
        uint16_t prgAddr(uint16_t addr);
        uint16_t chrAddr(uint16_t addr);
};

#endif // H_NROM