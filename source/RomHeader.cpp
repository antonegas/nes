#include <cstdint>
#include <array>

#include "../headers/RomHeader.h"

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

RomHeader::NametableLayout RomHeader::getNametableLayout() {
    if (header.ines.hasAlternativeNametable) RomHeader::NametableLayout::ALTERNATIVE;
    if (header.ines.isHorizontalArrangement) RomHeader::NametableLayout::HORIZONTAL;

    return RomHeader::NametableLayout::VERTICAL;
}

uint32_t RomHeader::getMapper() {
    if (getType() == RomHeader::Type::UNSUPPORTED) return 0xFFFFFFFF;

    uint32_t mapper = (header.ines.mapperHigh << 8) | header.ines.mapperLow;
    
    if (getType() == RomHeader::Type::NES2) mapper = (header.nes2.mapperHigh << 16) | mapper;

    return mapper;
}

uint8_t RomHeader::getSubmapper() {
    if (getType() != RomHeader::Type::NES2) return 0xFF;

    return header.nes2.submapper;
}

RomHeader::ConsoleType RomHeader::getConsoleType() {
    if (getType() == RomHeader::Type::UNSUPPORTED) return RomHeader::ConsoleType::UNSUPPORTED;
    if (header.nes2.consoleType != 0x00) return RomHeader::ConsoleType::UNSUPPORTED;

    return RomHeader::ConsoleType::NES;
}

RomHeader::ConsoleTiming RomHeader::getConsoleTiming() {
    if (getType() == RomHeader::Type::UNSUPPORTED) return RomHeader::ConsoleTiming::UNSUPPORTED;
    if (getType() == RomHeader::Type::INES) {
        if (header.ines.isPAL) return RomHeader::ConsoleTiming::PAL;
        return RomHeader::ConsoleTiming::NTSC;
    }

    switch (header.nes2.timing) {
        case RomHeader::ConsoleTiming::NTSC:
            return RomHeader::ConsoleTiming::NTSC;
        case RomHeader::ConsoleTiming::PAL:
            return RomHeader::ConsoleTiming::PAL;
        // NOTE: Multiregion is forced to NTSC timing.
        case RomHeader::ConsoleTiming::MULTIREGION:
            return RomHeader::ConsoleTiming::NTSC;
        default:
            RomHeader::ConsoleTiming::UNSUPPORTED;
    }
}

bool RomHeader::hasTrainer() {
    if (getType() == RomHeader::Type::UNSUPPORTED) return false;
    return header.ines.hasTrainer;
}

RomHeader::ExpansionDevice RomHeader::getExpansionDevice() {
    if (getType() == RomHeader::Type::UNSUPPORTED) return RomHeader::ExpansionDevice::UNSUPPORTED;
    if (getType() == RomHeader::Type::INES) return RomHeader::ExpansionDevice::UNSPECIFIED;
    if (header.nes2.expansionDevice == RomHeader::ExpansionDevice::UNSPECIFIED) return RomHeader::ExpansionDevice::UNSPECIFIED;
    if (header.nes2.expansionDevice == RomHeader::ExpansionDevice::STANDARD) return RomHeader::ExpansionDevice::STANDARD;

    return RomHeader::ExpansionDevice::UNSUPPORTED;
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
