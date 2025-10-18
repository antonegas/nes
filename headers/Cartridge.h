#ifndef H_CARTRIDGE
#define H_CARTRIDGE

#include <cstdint>
#include <vector>

using std::uint16_t;
using std::uint8_t;

class Cartridge {
    public:
        void power();
        void reset();
        uint8_t cpuRead(uint16_t addr);
        void cpuWrite(uint16_t addr, uint8_t data);
        uint8_t ppuRead(uint16_t addr);
        void ppuWrite(uint16_t addr, uint8_t data);

        /**
         * Mirroring
         * 
         * Reference: https://www.nesdev.org/wiki/Mirroring
         */

        enum MirrorMode {
            HORIZONTAL,
            VERTICAL,
            SINGLE,
            FOUR
        };

        MirrorMode mirror = HORIZONTAL;

        /**
         * IRQ
         */

        bool irq = false;
    private:
        std::vector<uint8_t> prgrom;
        std::vector<uint8_t> chrrom;
        std::vector<uint8_t> prgram;
        std::vector<uint8_t> chrram;
};

#endif // H_CARTRIDGE