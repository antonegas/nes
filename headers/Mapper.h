#ifndef H_MAPPER
#define H_MAPPER

#include <cstdint>
#include <vector>

#include "constants.h"

using std::uint16_t;
using std::uint8_t;

class Mapper {
    public:
        Mapper(
            std::vector<uint8_t> prgrom, 
            std::vector<uint8_t> chrrom
        ) : prgrom{prgrom}, chrrom{chrrom} {};
        Mapper(
            std::vector<uint8_t> prgrom, 
            std::vector<uint8_t> chrrom, 
            NametableLayout mirrorMode
        ) : prgrom{prgrom}, chrrom{chrrom}, mirrorMode{mirrorMode} {};
        
        static uint16_t const number;

        virtual void reset() {};
        virtual uint8_t cpuRead(uint16_t addr) { return 0x00; };
        virtual void cpuWrite(uint16_t addr, uint8_t data) {};
        virtual uint8_t ppuRead(uint16_t addr) { return 0x00; };
        virtual void ppuWrite(uint16_t addr, uint8_t data) {};
        virtual uint16_t mirrorAddr(uint16_t addr);
    protected:
        /**
         * NAMETABLE MIRRORING
         * 
         * To allow scrolling there are two ways of mirroring the nametables supported by the PPU hardware, 
         * horizontal or vertical mirroring. Which one of these are used is determined by the mapper and 
         * allows for vertical or horizontal scrolling respectively. Some mappers might even have extra VRAM
         * allowing for 4-screen mirroring using the extra VRAM as two more nametables.
         * 
         * Reference: https://www.nesdev.org/wiki/Mirroring
         */

        NametableLayout mirrorMode = NametableLayout::VERTICAL;

        std::vector<uint8_t> prgrom;
        std::vector<uint8_t> chrrom;
        std::vector<uint8_t> prgram;
        std::vector<uint8_t> chrram;
};

#endif // H_MAPPER