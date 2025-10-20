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

        /**
         * ROM FILE HEADER FORMATS
         * 
         * There are two supported types of header formats (iNES and NES 2.0). 
         * There exists other formats only these two are supported.
         * 
         * iNES: https://www.nesdev.org/wiki/INES
         * NES 2.0: https://www.nesdev.org/wiki/NES_2.0
         */

        bool unsupported();
        bool isINES(); 
        bool isNES2();

        /**
         * NAMETABLE LAYOUT
         *
         * The console nametable can be mirrored horizontally or vertically by the console hardware.
         * Some mappers also implement the ability to switch mirroring mode.
         *
         * Reference: https://www.nesdev.org/wiki/NES_2.0#Nametable_layout
         */

        bool horizontallyMirroredNametable();
        bool verticallyMirroredNametable();
        bool mapperControlledNametable();

        /**
         * CARTRIDGE BATTERY
         *
         * Some cartridges contained battery to save the 0x6000-0x7FFF across console restarts.
         *
         * Reference: https://www.nesdev.org/wiki/INES#Flags_6
         */

        bool isBatteryPresent();

        /**
         * TRAINER AREA
         *
         * There may exist 512 bytes of additionally data before the program ROM.
         * These bytes should be loaded into 0x7000-0x7200.
         *
         * Reference: https://www.nesdev.org/wiki/NES_2.0#Trainer_Area
         */

        bool isTrainerPresent();

        /**
         * MAPPERS AND SUBMAPPERS
         *
         * NES cartridges contained hardware which may different based on the game.
         * The different hardware are called mappers as they decide which data is returned when
         * reading from addresses on the cartridge. These mappers also needs to be emulated.
         * The NES 2.0 header format supports up to 4096 different mappers.
         * The iNES header format only supports the first 256 of those mappers.
         * 
         * Two ROMs with the same mapper number may still work differently. This can sometimes be 
         * determined by just looking at the program ROM size, but when this is not enough 
         * submapper numbers are used.
         *
         * Mapper reference: https://www.nesdev.org/wiki/Mapper
         * Submapper reference: https://www.nesdev.org/wiki/NES_2.0_submappers
         */

        uint16_t getMapperNumber();
        uint8_t getSubmapperNumber();

        /**
         * CONSOLE TYPES
         *
         * There were multiple different consoles but similar consoles from the japanese company 
         * for which the N in NES stand for. Because the console types are similar but not the same 
         * they require changes to the emulation in order to work correctly.
         *
         * Reference: https://www.nesdev.org/wiki/NES_2.0#Header
         */

        bool isEntertainmentSystem();
        bool isVsSystem();
        bool isPlaychoice();
        bool isExtendedConsole();

        /**
         * PROGRAM AND CHARACTER ROM SIZE
         * 
         * ROMs have different amounts of program and character ROM data.
         * 
         * Program ROM size reference: https://www.nesdev.org/wiki/NES_2.0#PRG-ROM_Area
         * Character ROM size reference: https://www.nesdev.org/wiki/NES_2.0#CHR-ROM_Area
         */

        uint32_t programRomSize();
        uint32_t characterRomSize();

        /**
         * PROGRAM AND CHARACTER RAM SIZE
         * 
         * TODO: describe
         *
         * Program RAM size reference: https://www.nesdev.org/wiki/NES_2.0#PRG-(NV)RAM/EEPROM
         * Character RAM size reference:
         */

        uint32_t programRamSize();
        uint32_t programNvramSize();
        uint32_t characterRamSize();
        uint32_t characterNvramSize();

        /**
         * CPU AND PPU TIMING
         * 
         * Consoles had different hardware depending on region. The only difference between the 
         * hardware is the clock rates. This difference was due to different television standards 
         * or more broadly the electrical grid. Mutliregion timing indicates that the ROM is 
         * identical regardless of region or that the game automatically detects the timing of the 
         * console. The different regions are listed below.
         *
         * NTSC: North America, Japan, South Korea, Taiwan
         * PAL: Western Europe, Australia
         * DENDY: Eastern Europe, Russia, Mainland China, India, Africa
         *
         * Reference: https://www.nesdev.org/wiki/NES_2.0#CPU/PPU_Timing
         */

        bool isNtscTiming();
        bool isPalTiming();
        bool isMultiregionTiming();
        bool isDendyTiming();

        /**
         * VS. SYSTEM AND EXTENDED CONSOLE TYPE
         * 
         * The Vs. System and extended consoles have different types of hardware and PPUs.
         *
         * Vs. System type reference: https://www.nesdev.org/wiki/NES_2.0#Vs._System_Type
         * Extended type reference: https://www.nesdev.org/wiki/NES_2.0#Extended_Console_Type
         */

        uint8_t getVsPpu();
        uint8_t getVsHardware();
        uint8_t getExtendedConsole();

        /**
         * MISCELLANOUS ROM AREA
         *
         * TODO: describe
         *
         * Reference: https://www.nesdev.org/wiki/NES_2.0#Miscellaneous_ROM_Area
         */

        uint8_t numberOfMiscellanous();

        /**
         * DEFAULT EXPANSION DEVICE
         * 
         * The NES and other console types supported different types of input devices. If a ROM 
         * requires a specific type of input device this is indicated by the default expansion 
         * device.
         *
         * Reference: https://www.nesdev.org/wiki/NES_2.0#Default_Expansion_Device
         */

        uint8_t getExpansionDevice(); // TODO: change to enum for different devices.

    private:
        union HEADER {
            struct {
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
            struct {
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
                uint8_t subMapper : 4;
                // byte 9
                uint8_t prgromBlockHigh : 4;
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
                } consoleType;
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