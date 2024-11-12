/*
iNES: https://www.nesdev.org/wiki/INES
NES 2.0: https://www.nesdev.org/wiki/NES_2.0
*/

#ifndef H_ROM_METADATA
#define H_ROM_METADATA

#include <cstdint>

class RomMetadata {
    public:
        RomMetadata(uint8_t[16]);
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