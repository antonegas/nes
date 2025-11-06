#include <cstdint>
#include <vector>
#include <array>
#include <algorithm>
#include <iterator>

#include "RomFile.h"
#include "Mapper.h"
#include "Mappers.h"
#include "constants.h"

using std::uint16_t;
using std::uint8_t;

RomFile::RomFile(std::vector<uint8_t> data) {
    if (data.size() < 16) return;

    std::copy(std::begin(data), std::begin(data) + 16, std::begin(this->header.raw));

    if (getType() == RomFile::Type::UNSUPPORTED) return;

    std::size_t current = 16;

    if (hasTrainer()) {
        if (data.size() < current + 512) return;

        std::copy(std::begin(data) + current, std::begin(data) + current + 512, std::begin(trainer));
        current += 512;
    }

    if (data.size() < current + getPrgromSize())  return;

    prgrom.resize(getPrgromSize());
    std::copy(std::begin(data) + current, std::begin(data) + current + getPrgromSize(), std::begin(prgrom));
    current += getPrgromSize();

    if (data.size() < current + getChrromSize()) return;

    chrrom.resize(getChrromSize());
    std::copy(std::begin(data) + current, std::begin(data) + current + getChrromSize(), std::begin(chrrom));
}

Mapper RomFile::getMapper() {
    // Add check to verify correct amount of prgrom and chrrom
    switch (getMapperNumber()) {
        case NROM::number:
            return NROM(prgrom, chrrom, getNametableLayout());
            break;
        default:
            return Mapper(std::vector<uint8_t>(), std::vector<uint8_t>());
    }
}

RomFile::Type RomFile::getType() {
    if (header.ines.nes != 0x1A53454E) return RomFile::Type::UNSUPPORTED;
    if (header.ines.nes2 == 0b00) return RomFile::Type::INES;
    if (header.nes2.nes2 == 0b10) return RomFile::Type::NES2;

    return RomFile::Type::UNSUPPORTED; 
}

NametableLayout RomFile::getNametableLayout() {
    if (header.ines.hasAlternativeNametable) NametableLayout::ALTERNATIVE;
    if (header.ines.isHorizontalArrangement) NametableLayout::HORIZONTAL;

    return NametableLayout::VERTICAL;
}

uint32_t RomFile::getMapperNumber() {
    if (getType() == RomFile::Type::UNSUPPORTED) return 0xFFFFFFFF;

    uint32_t mapper = (header.ines.mapperHigh << 8) | header.ines.mapperLow;
    
    if (getType() == RomFile::Type::NES2) mapper = (header.nes2.mapperHigh << 16) | mapper;

    return mapper;
}

uint8_t RomFile::getSubmapperNumber() {
    if (getType() != RomFile::Type::NES2) return 0xFF;

    return header.nes2.submapper;
}

ConsoleType RomFile::getConsoleType() {
    if (getType() == RomFile::Type::UNSUPPORTED) return ConsoleType::UNSUPPORTED;
    if (header.nes2.consoleType != 0x00) return ConsoleType::UNSUPPORTED;

    return ConsoleType::NES;
}

ConsoleTiming RomFile::getConsoleTiming() {
    if (getType() == RomFile::Type::UNSUPPORTED) return ConsoleTiming::UNSUPPORTED;
    if (getType() == RomFile::Type::INES) {
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
            return ConsoleTiming::UNSUPPORTED;
    }
}

bool RomFile::hasTrainer() {
    if (getType() == RomFile::Type::UNSUPPORTED) return false;
    return header.ines.hasTrainer;
}

ExpansionDevice RomFile::getExpansionDevice() {
    if (getType() == RomFile::Type::UNSUPPORTED) return ExpansionDevice::UNSUPPORTED;
    if (getType() == RomFile::Type::INES) return ExpansionDevice::UNSPECIFIED;
    if (header.nes2.expansionDevice == ExpansionDevice::UNSPECIFIED) return ExpansionDevice::UNSPECIFIED;
    if (header.nes2.expansionDevice == ExpansionDevice::STANDARD) return ExpansionDevice::STANDARD;

    return ExpansionDevice::UNSUPPORTED;
}

uint32_t RomFile::getPrgromSize() {
    if (getType() == RomFile::Type::UNSUPPORTED) return 0x00000000;
    if (getType() == RomFile::Type::INES) return header.ines.prgramBlocks << 14;

    // Exponent ROM size is not supported.
    if (header.nes2.prgromBlocksHigh == 0x0F) return 0x00000000;

    return ((header.nes2.prgromBlocksHigh << 16) | header.nes2.prgromBlocksLow) << 14;
}

uint16_t RomFile::getPrgramSize() {
    if (getType() == RomFile::Type::UNSUPPORTED) return 0x0000;

    // Some versions of iNES supports RAM but it is poorly specified
    if (getType() == RomFile::Type::INES) return 0x0000;

    return 64 << header.nes2.prgramShift;
}

uint16_t RomFile::getPrgnvramSize() {
    if (getType() == RomFile::Type::UNSUPPORTED) return 0x0000;

    // iNES has bit for battery-backed RAM but lets ignore that.
    if (getType() == RomFile::Type::INES) return 0x0000;

    return 64 << header.nes2.prgnvramShift;
}

uint32_t RomFile::getChrromSize() {
    if (getType() == RomFile::Type::UNSUPPORTED) return 0x00000000;
    if (getType() == RomFile::Type::INES) return header.ines.prgramBlocks << 13;

    // Exponent ROM size is not supported.
    if (header.nes2.chrromBlocksHigh == 0x0F) return 0x00000000;

    return ((header.nes2.chrromBlocksHigh << 16) | header.nes2.chrromBlocksLow) << 13;
}

uint16_t RomFile::getChrramSize() {
    if (getType() == RomFile::Type::UNSUPPORTED) return 0x0000;
    if (getType() == RomFile::Type::INES) return 0x0000;

    return 64 << header.nes2.chrramShift;
}

uint16_t RomFile::getChrnvramSize() {
    if (getType() == RomFile::Type::UNSUPPORTED) return 0x0000;
    if (getType() == RomFile::Type::INES) return 0x0000;

    return 64 << header.nes2.chrnvramShift;
}
