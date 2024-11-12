#include "../headers/RomMetadata.h"
#include <cstring>

RomMetadata::RomMetadata(uint8_t metadata[16]) {
    memcpy(identification, &metadata[0], 4 * sizeof(char));
    programROMSize = ((metadata[9] & 0x0F) << 8) + metadata[4];
    characterROMSize = ((metadata[9] & 0xF0) << 4) + metadata[5];
    hardwiredNametableLayout = metadata[6] & 0x01;
    batteryPresent = (metadata[6] & 0x02) >> 1;
    trainerPresent = (metadata[6] & 0x04) >> 2;
    alternativeNametables = (metadata[6] & 0x08) >> 3;
    mapperNumber = ((metadata[8] & 0x0F) << 8) + ((metadata[7] & 0xF0)) + ((metadata[6] & 0xF0) >> 4);
    consoleType = metadata[7] & 0x03;
    nesTwoIdentifier = (metadata[7] & 0x0C) >> 2;
    submapperNumber = (metadata[8] & 0xF0) >> 4;
    programRAMShift = metadata[10] & 0x0F;
    programNVRAMShift = (metadata[10] & 0xF0) >> 4;
    characterRAMShift = metadata[11] & 0x0F;
    characterNVRAMShift = (metadata[11] & 0x0F) >> 4;
    clockTiming = metadata[12] & 0x03;
    VsPPUType = metadata[13] & 0x0F;
    VsHardwareType = (metadata[13] & 0xF0) >> 4;
    extendedConsoleType = metadata[13] & 0x0F;
    miscellaneousROMsPresent = metadata[14] & 0x03;
    defaultExpansionDevice = metadata[15] & 0x3F;
}