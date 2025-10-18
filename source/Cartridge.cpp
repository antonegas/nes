#include <cstdint>

#include "../headers/Cartridge.h"
#include "../headers/Mapper.h"

using std::uint16_t;
using std::uint8_t;

void Cartridge::reset() {
    mapper.reset();
}

uint8_t Cartridge::cpuRead(uint16_t addr) {
    return mapper.cpuRead(addr);
}

void Cartridge::cpuWrite(uint16_t addr, uint8_t data) {
    mapper.cpuWrite(addr, data);
}

uint8_t Cartridge::ppuRead(uint16_t addr) {
    return mapper.ppuRead(addr);
}

void Cartridge::ppuWrite(uint16_t addr, uint8_t data) {
    mapper.ppuWrite(addr, data);
}

uint16_t Cartridge::mirrorAddr(uint16_t addr) {
    switch (mapper.mirrorMode) {
        case Mapper::HORIZONTAL:
            if (addr & 0x0800) return addr & 0x03FF;
            else return (addr & 0x03FF) | 0x0400;
        case Mapper::VERTICAL:
            return addr & 0x07FF;
        case Mapper::FOUR:
            return addr & 0x0FFF;
    };
}