#include <cstdint>

#include "../headers/Cartridge.h"

using std::uint16_t;
using std::uint8_t;

uint16_t Cartridge::mirrorAddr(uint16_t addr) {
    uint8_t table = addr & 0x0C00;
    addr = addr & 0x03FF;

    switch (mirrorMode) {
        case HORIZONTAL:
            if (table == 0x0000 || table == 0x0400) return addr;
            else return addr | 0x0400;
        case VERTICAL:
            if (table == 0x0000 || table == 0x0800) return addr;
            else return addr | 0x0400;
        case SINGLE:
            return addr & 0x07FF;
        case FOUR:
            return addr;
    };
}