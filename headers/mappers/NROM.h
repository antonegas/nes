#ifndef H_NROM
#define H_NROM

#include <cstdint>
#include <vector>

#include "../Mapper.h"

using std::uint16_t;
using std::uint8_t;

class NROM : Mapper {
    public:
        NROM(
            std::vector<uint8_t> prgrom, 
            std::vector<uint8_t> chrrom, 
            MirrorMode mirrorMode
        ) : Mapper(prgrom, chrrom, mirrorMode) {};

        virtual uint8_t cpuRead(uint16_t addr);
        virtual uint8_t ppuRead(uint16_t addr);
    private:
        uint16_t prgAddr(uint16_t addr);
        uint16_t chrAddr(uint16_t addr);
};

#endif // H_NROM