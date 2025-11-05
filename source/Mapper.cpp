#include <cstdint>

#include "Mapper.h"
#include "constants.h"

using std::uint16_t;

uint16_t Mapper::mirrorAddr(uint16_t addr) {
    switch (mirrorMode) {
        case NametableLayout::VERTICAL:
            if (addr & 0x0800) return addr & 0x03FF;
            else return (addr & 0x03FF) | 0x0400;
        case NametableLayout::HORIZONTAL:
            return addr & 0x07FF;
        case NametableLayout::FOUR:
            return addr & 0x0FFF;
    };
}