/*
iNES: https://www.nesdev.org/wiki/INES
NES 2.0: https://www.nesdev.org/wiki/NES_2.0
*/

#ifndef H_ROM_METADATA
#define H_ROM_METADATA

#include <cstdint>

class RomMetadata {
    public:
        /**
         * CONSTRUCTOR
         * 
         * Extracts the data stored in the 16 byte header.
         */
        RomMetadata(uint8_t[16]);

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
        int getMapperNumber();
        int getSubmapperNumber();

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
        int programRomSize();
        int characterRomSize();

        /**
         * PROGRAM AND CHARACTER RAM SIZE
         * 
         * TODO: describe
         *
         * Program RAM size reference: https://www.nesdev.org/wiki/NES_2.0#PRG-(NV)RAM/EEPROM
         * Character RAM size reference:
         */
        int programRamSize(); // saved number is shift count (64 << shift)
        int programNvramSize(); // saved number is shift count
        int characterRamSize(); // saved number is shift count
        int characterNvramSize(); // saved number is shift count

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
        int getVsPpu();
        int getVsHardware();
        int getExtendedConsole();

        /**
         * MISCELLANOUS ROM AREA
         *
         * TODO: describe
         *
         * Reference: https://www.nesdev.org/wiki/NES_2.0#Miscellaneous_ROM_Area
         */
        int numberOfMiscellanous();

        /**
         * DEFAULT EXPANSION DEVICE
         * 
         * The NES and other console types supported different types of input devices. If a ROM 
         * requires a specific type of input device this is indicated by the default expansion 
         * device.
         *
         * Reference: https://www.nesdev.org/wiki/NES_2.0#Default_Expansion_Device
         */
        int getExpansionDevice(); // TODO: change to bools for different devices.

    private:
        char identification[4];
        int programROMSize : 12;
        int characterROMSize : 12;
        bool hardwiredNametableLayout : 1;
        bool batteryPresent : 1;
        bool trainerPresent : 1;
        bool alternativeNametables : 1;
        int mapperNumber : 12;
        int consoleType : 2;
        int nesTwoIdentifier : 2;
        int submapperNumber : 4;
        int programRAMShift : 4;
        int programNVRAMShift : 4;
        int characterRAMShift : 4;
        int characterNVRAMShift : 4;
        int clockTiming : 2;
        int VsPPUType : 4;
        int VsHardwareType : 4;
        int extendedConsoleType : 4;
        int miscellaneousROMsPresent : 2;
        int defaultExpansionDevice : 6;
};

#endif // H_ROM_METADATA