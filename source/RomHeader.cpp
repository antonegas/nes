#include <cstdint>
#include <array>

#include "../headers/RomHeader.h"
#include "../headers/constants.h"

using std::uint16_t;
using std::uint8_t;

RomHeader::RomHeader(std::array<uint8_t, 16> header) {
    this->header.raw = header;
}

RomHeader::Type RomHeader::getType() {
    if (header.ines.nes != 0x4E45531A) return RomHeader::Type::UNSUPPORTED;
    if (header.ines.nes2 == 0x00) return RomHeader::Type::INES;
    if (header.nes2.nes2 == 0x10) return RomHeader::Type::NES2;

    return RomHeader::Type::UNSUPPORTED; 
}

NametableLayout RomHeader::getNametableLayout() {
    if (header.ines.hasAlternativeNametable) NametableLayout::ALTERNATIVE;
    if (header.ines.isHorizontalArrangement) NametableLayout::HORIZONTAL;

    return NametableLayout::VERTICAL;
}

uint32_t RomHeader::getMapperNumber() {
    if (getType() == RomHeader::Type::UNSUPPORTED) return 0xFFFFFFFF;

    uint32_t mapper = (header.ines.mapperHigh << 8) | header.ines.mapperLow;
    
    if (getType() == RomHeader::Type::NES2) mapper = (header.nes2.mapperHigh << 16) | mapper;

    return mapper;
}

uint8_t RomHeader::getSubmapperNumber() {
    if (getType() != RomHeader::Type::NES2) return 0xFF;

    return header.nes2.submapper;
}

ConsoleType RomHeader::getConsoleType() {
    if (getType() == RomHeader::Type::UNSUPPORTED) return ConsoleType::UNSUPPORTED;
    if (header.nes2.consoleType != 0x00) return ConsoleType::UNSUPPORTED;

    return ConsoleType::NES;
}

ConsoleTiming RomHeader::getConsoleTiming() {
    if (getType() == RomHeader::Type::UNSUPPORTED) return ConsoleTiming::UNSUPPORTED;
    if (getType() == RomHeader::Type::INES) {
        if (header.ines.isPAL) return ConsoleTiming::PAL;
        return ConsoleTiming::NTSC;
    }

    switch (header.nes2.timing) {
        case ConsoleTiming::NTSC:
            return ConsoleTiming::NTSC;
        case ConsoleTiming::PAL:
            return ConsoleTiming::PAL;
        // NOTE: Multiregion is forced to NTSC timing.
        case ConsoleTiming::MULTIREGION:
            return ConsoleTiming::NTSC;
        default:
            ConsoleTiming::UNSUPPORTED;
    }
}

bool RomHeader::hasTrainer() {
    if (getType() == RomHeader::Type::UNSUPPORTED) return false;
    return header.ines.hasTrainer;
}

ExpansionDevice RomHeader::getExpansionDevice() {
    if (getType() == RomHeader::Type::UNSUPPORTED) return ExpansionDevice::UNSUPPORTED;
    if (getType() == RomHeader::Type::INES) return ExpansionDevice::UNSPECIFIED;
    if (header.nes2.expansionDevice == ExpansionDevice::UNSPECIFIED) return ExpansionDevice::UNSPECIFIED;
    if (header.nes2.expansionDevice == ExpansionDevice::STANDARD) return ExpansionDevice::STANDARD;

    return ExpansionDevice::UNSUPPORTED;
}

uint32_t RomHeader::getPrgromSize() {
    if (getType() == RomHeader::Type::UNSUPPORTED) return 0x00000000;
    if (getType() == RomHeader::Type::INES) return header.ines.prgramBlocks << 14;

    // Exponent ROM size is not supported.
    if (header.nes2.prgromBlocksHigh == 0x0F) return 0x00000000;

    return ((header.nes2.prgromBlocksHigh << 16) | header.nes2.prgromBlocksLow) << 14;
}

uint16_t RomHeader::getPrgramSize() {
    if (getType() == RomHeader::Type::UNSUPPORTED) return 0x0000;

    // Some versions of iNES supports RAM but it is poorly specified
    if (getType() == RomHeader::Type::INES) return 0x0000;

    return 64 << header.nes2.prgramShift;
}

uint16_t RomHeader::getPrgnvramSize() {
    if (getType() == RomHeader::Type::UNSUPPORTED) return 0x0000;

    // iNES has bit for battery-backed RAM but lets ignore that.
    if (getType() == RomHeader::Type::INES) return 0x0000;

    return 64 << header.nes2.prgnvramShift;
}

uint32_t RomHeader::getChrromSize() {
    if (getType() == RomHeader::Type::UNSUPPORTED) return 0x00000000;
    if (getType() == RomHeader::Type::INES) return header.ines.prgramBlocks << 13;

    // Exponent ROM size is not supported.
    if (header.nes2.chrromBlocksHigh == 0x0F) return 0x00000000;

    return ((header.nes2.chrromBlocksHigh << 16) | header.nes2.chrromBlocksLow) << 13;
}

uint16_t RomHeader::getChrramSize() {
    if (getType() == RomHeader::Type::UNSUPPORTED) return 0x0000;
    if (getType() == RomHeader::Type::INES) return 0x0000;

    return 64 << header.nes2.chrramShift;
}

uint16_t RomHeader::getChrnvramSize() {
    if (getType() == RomHeader::Type::UNSUPPORTED) return 0x0000;
    if (getType() == RomHeader::Type::INES) return 0x0000;

    return 64 << header.nes2.chrnvramShift;
}
