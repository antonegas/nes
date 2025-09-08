#include <cstring>
#include <cstdint>

#include "../headers/RomMetadata.h"

using std::uint16_t;
using std::uint8_t;

RomMetadata::RomMetadata(uint8_t metadata[16]) {
    std::memcpy(identification, &metadata[0], 4 * sizeof(char));
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

bool RomMetadata::unsupported() {
    if (!isINES() && !isNES2) return true;
    return false
}

bool RomMetadata::isINES() {
    return identification[0] == 'N' && identification[1] == 'E' && identification[2] == 'S' && identification[3] == 0x1A && nesTwoIdentifier != 0x02;
}

bool RomMetadata::isNES2() {
    return identification[0] == 'N' && identification[1] == 'E' && identification[2] == 'S' && identification[3] == 0x1A && nesTwoIdentifier == 0x02;
}

bool RomMetadata::horizontallyMirroredNametable() {
    return hardwiredNametableLayout == 0x00 && alternativeNametables != 0x01;
}

bool RomMetadata::verticallyMirroredNametable() {
    return hardwiredNametableLayout == 0x01 && alternativeNametables != 0x01;
}

bool RomMetadata::mapperControlledNametable() {
    return alternativeNametables;
}

bool RomMetadata::isBatteryPresent() {
    return batteryPresent;
}

bool RomMetadata::isTrainerPresent() {
    return trainerPresent;
}

int RomMetadata::getMapperNumber() {
    return mapperNumber;
}

int RomMetadata::getSubmapperNumber() {
    return submapperNumber;
}

bool RomMetadata::isEntertainmentSystem() {
    return consoleType == 0x00;
}

bool RomMetadata::isVsSystem() {
    return consoleType == 0x01;
}

bool RomMetadata::isPlaychoice() {
    return consoleType == 0x02;
}

bool RomMetadata::isExtendedConsole() {
    return consoleType == 0x03;
}

int RomMetadata::programRomSize() {
    return programROMSize;
}

int RomMetadata::characterRomSize() {
    return characterROMSize;
}

int RomMetadata::programRamSize() {
    return 64 << programRAMShift;
}   

int RomMetadata::programNvramSize() {
    return 64 << programNVRAMShift;
}   

int RomMetadata::characterRamSize() {
    return 64 << characterRAMShift;
} 

int RomMetadata::characterNvramSize() {
    return 64 << characterNVRAMShift;
}

bool RomMetadata::isNtscTiming() {
    return clockTiming == 0x00;
}

bool RomMetadata::isPalTiming() {
    return clockTiming == 0x01;
}

bool RomMetadata::isMultiregionTiming() {
    return clockTiming == 0x02;
}

bool RomMetadata::isDendyTiming() {
    return clockTiming == 0x03;
}

int RomMetadata::getVsPpu() {
    return isVsSystem() ? VsPPUType : -1; 
}

int RomMetadata::getVsHardware() {
    return isVsSystem() ? VsHardwareType : -1;
}

int RomMetadata::getExtendedConsole() {
    return isExtendedConsole() ? extendedConsoleType : -1;
}

int RomMetadata::numberOfMiscellanous() {
    return miscellaneousROMsPresent;
}

int RomMetadata::getExpansionDevice() {
    return defaultExpansionDevice;
}
