/*
iNES: https://www.nesdev.org/wiki/INES
NES 2.0: https://www.nesdev.org/wiki/NES_2.0
*/

#ifndef H_ROM_HEADER
#define H_ROM_HEADER

#include <cstdint>
#include <array>

using std::int32_t;
using std::uint16_t;
using std::uint8_t;

/**
 * ROM HEADERS
 * 
 * Since each cartridge had an associated mapper ROM files has to describe information
 * about these mappers among other things. There are two standards for describing this
 * in a ROM header, iNES and NES 2.0 (which is backwards compatible with iNES).
 * 
 * iNES Reference: https://www.nesdev.org/wiki/INES
 * NES 2.0 Reference: https://www.nesdev.org/wiki/NES_2.0
 */

class RomHeader {
    public:
        RomHeader(std::array<uint8_t, 16> header);

        enum Type {
            INES,
            NES2,
            UNSUPPORTED = 0xFF
        };

        Type getType();

        /**
         * MAPPERS
         * 
         * Each mapper is identified with a number iNES supports 256 mappers and NES 2.0
         * up to 4096. Due to some mappers having some differences there are also
         * submappers which describe different submappers within a mapper.
         * 
         * Reference: https://www.nesdev.org/wiki/Mapper
         */

        uint32_t getMapper();
        uint8_t getSubmapper();

        /**
         * TRAINER
         * 
         * Some ROM files has 512 bytes of trainer area following the header which should
         * be placed into CPU memory at 0x7000.
         * 
         * Reference: https://www.nesdev.org/wiki/NES_2.0#Trainer_Area
         */

        bool hasTrainer();

        /**
         * ROM SIZE AND AVAILABLE RAM
         * 
         * Cartridges have different amount of PRG-ROM and CHR-ROM. There might also exist
         * extra PRG-RAM and CHR-RAM for some mappers.
         * 
         * PRG-ROM Reference: https://www.nesdev.org/wiki/NES_2.0#PRG-ROM_Area
         * CHR-ROM Reference: https://www.nesdev.org/wiki/NES_2.0#CHR-ROM_Area
         * PRG-RAM Reference: https://www.nesdev.org/wiki/NES_2.0#PRG-(NV)RAM/EEPROM
         * CHR-RAM Reference: https://www.nesdev.org/wiki/NES_2.0#CHR-(NV)RAM
         */

        uint32_t getPrgromSize();
        uint16_t getPrgramSize();
        uint16_t getPrgnvramSize();
        uint32_t getChrromSize();
        uint16_t getChrramSize();
        uint16_t getChrnvramSize();

        /**
         * NAMETABLE LAYOUT
         * 
         * The nametables can be mapped in different ways to allow scrolling. A mapper can
         * hardwire this layout, switch between layouts or support an entirely different layout.
         * 
         * Reference: https://www.nesdev.org/wiki/NES_2.0#Nametable_layout
         */

        enum NametableLayout {
            HORIZONTAL,
            VERTICAL,
            ALTERNATIVE
        };

        NametableLayout getNametableLayout();

        /**
         * CONSOLE TYPE
         * 
         * The header also supports different kinds of console hardware, NES, Vs. System, 
         * Playchoice and extended console types. The Vs. System inturn also requires different 
         * hardware depending on the ROM.
         * 
         * Reference: https://www.nesdev.org/wiki/NES_2.0#File_Structure
         * Vs. System Reference: https://www.nesdev.org/wiki/NES_2.0#Vs._System_Type
         */

        enum ConsoleType {
            NES = 0x00,
            // VS_SYSTEM = 0x01,
            // PLAYCHOICE = 0x02,
            // EXTENDED = 0x03,
            UNSUPPORTED = 0xFF
        };

        ConsoleType getConsoleType();

        /**
         * CONSOLE TIMING
         * 
         * Since the console was released in regions with different TV standards. This resulted
         * in the CPU and PPU having to run at different clock speeds depending on region.
         * 
         * Reference: https://www.nesdev.org/wiki/NES_2.0#CPU/PPU_Timing
         */

        enum ConsoleTiming {
            NTSC = 0x00,
            PAL = 0x01,
            // DENDY = 0x02,
            MULTIREGION = 0x03,
            UNSUPPORTED = 0xFF
        };

        ConsoleTiming getConsoleTiming();

        /**
         * EXPANSION DEVICE
         * 
         * The console supported differnent types of input devices. The header can specify 
         * which one of these should be used when playing the game.
         * 
         * Reference: www.nesdev.org/wiki/NES_2.0#Default_Expansion_Device
         */

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

        ExpansionDevice getExpansionDevice();

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
            std::array<uint8_t, 16> raw;
        } header;
};

#endif // H_ROM_HEADER