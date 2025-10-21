#include "../headers/Mapper.h"

uint16_t Mapper::mirrorAddr(uint16_t addr) {
    switch (mirrorMode) {
        case Mapper::HORIZONTAL:
            if (addr & 0x0800) return addr & 0x03FF;
            else return (addr & 0x03FF) | 0x0400;
        case Mapper::VERTICAL:
            return addr & 0x07FF;
        case Mapper::FOUR:
            return addr & 0x0FFF;
    };
}