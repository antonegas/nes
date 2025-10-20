/*
iNES: https://www.nesdev.org/wiki/INES
NES 2.0: https://www.nesdev.org/wiki/NES_2.0
*/

#ifndef H_ROM_HEADER
#define H_ROM_HEADER

#include <cstdint>

using std::int32_t;
using std::uint16_t;
using std::uint8_t;

class RomHeader {
    public:
        /**
         * CONSTRUCTOR
         * 
         * Extracts the data stored in the 16 byte header.
         */

        RomHeader(uint8_t[16]);

        enum Type {
            INES,
            NES2,
            UNSUPPORTED = 0xFF
        };

        enum NametableLayout {
            HORIZONTAL,
            VERTICAL,
            ALTERNATIVE
        };

        enum ConsoleType {
            NES = 0x00,
            // VS_SYSTEM = 0x01,
            // PLAYCHOICE = 0x02,
            // EXTENDED = 0x03,
            UNSUPPORTED = 0xFF
        };

        enum ConsoleTiming {
            NTSC = 0x00,
            PAL = 0x01,
            // DENDY = 0x02,
            MULTIREGION = 0x03,
            UNSUPPORTED = 0xFF
        };

        enum ExpansionDevice {
            UNSPECIFIED = 0x00,
            STANDARD = 0x01,
            // FOURSCORE = 0x02,
            // FAMICOM_FOUR = 0x03,
            // VS_4016 = 0x04,
            // VS_4017 = 0x05,
            // VS_ZAPPER = 0x07,
            // ZAPPER_4017 = 0x08,
            // ZAPPER_4016 = 0x49,
            // TWO_ZAPPERS = 0x09,
            // SNES_CONTROLLER = 0x2B,
            UNSUPPORTED = 0xFF
        };

        Type getType();
        NametableLayout getNametableLayout();
        uint32_t getMapper();
        uint8_t getSubmapper();
        ConsoleType getConsoleType();
        ConsoleTiming getConsoleTiming();
        bool hasTrainer();
        ExpansionDevice getExpansionDevice();
        uint32_t getPrgromSize();
        uint16_t getPrgramSize();
        uint16_t getPrgnvramSize();
        uint32_t getChrromSize();
        uint16_t getChrramSize();
        uint16_t getChrnvramSize();

    private:
        union HEADER {
            struct INES {
                // byte 0-3
                uint32_t nes;
                // byte 4
                uint8_t prgromBlocks;
                // byte 5
                uint8_t chrromBlocks;
                // byte 6
                bool isHorizontalArrangement : 1;
                bool hasBattery : 1;
                bool hasTrainer : 1;
                bool hasAlternativeNametable : 1;
                uint8_t mapperLow : 4;
                // byte 7
                bool isVsSystem : 1;
                bool isPlayChoice : 1;
                uint8_t nes2 : 2;
                uint8_t mapperHigh : 4;
                // byte 8
                uint8_t prgramBlocks;
                // byte 9
                bool isPAL : 1;
                uint8_t unused1 : 7;
                // byte 10
                uint8_t tvSystem : 2;
                uint8_t unused2 : 2;
                bool hasPrgRam : 1;
                bool hasBusConflicts : 1;
                // byte end of 10-15
                uint8_t padding : 42;
            } ines;
            struct NES2 {
                // byte 0-3
                uint32_t nes;
                // byte 4
                uint8_t prgromBlocksLow;
                // byte 5
                uint8_t chrromBlocksLow;
                // byte 6
                bool isHorizontalArrangement : 1;
                bool hasBattery : 1;
                bool hasTrainer : 1;
                bool hasAlternativeNametable : 1;
                uint8_t mapperLow : 4;
                // byte 7
                uint8_t consoleType : 2;
                uint8_t nes2 : 2;
                uint8_t mapperMid : 4;
                // byte 8
                uint8_t mapperHigh : 4;
                uint8_t submapper : 4;
                // byte 9
                uint8_t prgromBlocksHigh : 4;
                uint8_t chrromBlocksHigh : 4;
                // byte 10
                uint8_t prgramShift : 4;
                uint8_t prgnvramShift : 4;
                // byte 11
                uint8_t chrramShift : 4;
                uint8_t chrnvramShift : 4;
                // byte 12
                uint8_t timing : 2;
                uint8_t unused0 : 6;
                // byte 13
                union ConsoleType {
                    struct {
                        uint8_t ppu : 4;
                        uint8_t hardware : 4;
                    } vsSystem;
                    struct {
                        uint8_t type : 4;
                        uint8_t padding : 4;
                    } extended;
                    uint8_t raw;
                } extendedConsoleType;
                // byte 14
                uint8_t miscRomsCount : 2;
                uint8_t unused1 : 6;
                // byte 15
                uint8_t expansionDevice : 6;
                uint8_t padding : 2;
            } nes2;
            uint8_t raw[16];
        } header;
};

#endif // H_ROM_HEADER