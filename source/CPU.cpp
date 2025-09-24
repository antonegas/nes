#include <cstdint>

#include "../headers/CPU.h"

using std::uint16_t;
using std::uint8_t;

// TODO: Handle oops cycles for addressing modes.
uint16_t CPU::ZPX() {
    uint16_t arg = read(pc++);
    uint16_t addr = (arg + x) & 0x00FF;
    return addr;
}

uint16_t CPU::ZPY() {
    uint16_t arg = read(pc++);
    uint16_t addr = (arg + y) & 0x00FF;
    return addr;
}

uint16_t CPU::ABX() {
    // NOTE: might cause oops cycle.
    uint16_t arg = read(pc++);
    uint16_t addr = arg + x;
    return addr;
}

uint16_t CPU::ABY() {
    // NOTE: might cause oops cycle.
    uint16_t arg = read(pc++);
    uint16_t addr = arg + y;
    return addr;
}

uint16_t CPU::IZX() {
    uint16_t arg = read(pc++);
    uint16_t low = read((arg + x) & 0x00FF);
    uint16_t high = read((arg + x + 1) & 0x00FF);
    uint16_t addr = (high << 8) | low + y;
    return addr;
}

uint16_t CPU::IZY() {
    // NOTE: might cause oops cycle.
    uint16_t arg = read(pc++);
    uint16_t low = read(arg);
    uint16_t high = read((arg + 1) & 0x00FF);
    uint16_t addr = (high << 8) | low;
    return addr;
}

uint16_t CPU::IMP() {
    return 0x0000; // TODO: because of this hack it is probably better to set addr instead of returning.
}

uint16_t CPU::ACC() {
    return a;
}

uint16_t CPU::IMM() {
    uint16_t addr = pc;
    pc++;
    return addr;
}

uint16_t CPU::ZP0() {
    uint16_t arg = read(pc++);
    uint16_t addr = arg & 0x00FF;
    return addr;
}

uint16_t CPU::ABS() {
    uint16_t low = read(pc++);
    uint16_t high = read(pc++);
    uint16_t addr = (high << 8) | low;
    return addr;
}

uint16_t CPU::REL() {
    uint16_t arg = read(pc++);

    // If the 8:th bit is set the offset is negative.
    if (arg & 0x80) {
        arg = arg | 0xFF00;
    }

    uint16_t addr = pc + arg;
    return addr;
}

uint16_t CPU::IND() {
    // NOTE: bug described by https://forums.nesdev.org/viewtopic.php?t=15587
    uint16_t ptr_low = read(pc++);
    uint16_t ptr_high = read(pc++);
    uint16_t ptr = (ptr_high << 8) | ptr_low;
    uint16_t low = read(ptr);
    uint16_t high = read(ptr + 1);
    uint16_t addr = 0;

    if (ptr_low != 0x00FF) {
        // Normal
        addr = (read(ptr + 1) << 8) | read(ptr);
    } else {
        // Bugged
        addr = (read(ptr & 0xFF00) << 8) | read(ptr);
    }

    return addr;
}