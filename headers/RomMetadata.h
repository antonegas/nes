/*
https://www.nesdev.org/wiki/NES_2.0
*/

#ifndef H_ROM_METADATA
#define H_ROM_METADATA

#include <cstdint>

class RomMetadata {
    public:
        RomMetadata(uint8_t[16]);
        bool isSupported();
        uint16_t programRomSize();
        uint16_t characterRomSize();
        bool isMirroredVertically();
        bool isMirroredHorizontally();
        bool isMapperControlled();
        bool hasBattery();
        bool hasTrainer();
        uint16_t getMapperNumber();
        bool isNES();
        bool isVS();
        bool isPlaychoice();
        bool isExtendConsole();
        bool isNES2();
        uint8_t getSubmapper();
        uint8_t programRamSize();
        uint8_t characterRamSize();
        bool isNTSC();
        bool isPAL();
        bool isMultiRegional();
        bool isDendy();
        // TODO: byte 13 (Vs. system ppu and hardare type or extended console type)
        // TODO: byte 14 (Number of miscellaneous roms)
        uint8_t defaultExpansionDevice();
    private:
        uint8_t _metadata[16];
};

#endif // H_ROM_METADATA