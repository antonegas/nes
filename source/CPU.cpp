#include <cstdint>

#include "../headers/CPU.h"

using std::uint16_t;
using std::uint8_t;

void CPU::power() {
    a = 0x00;
    x = 0x00;
    y = 0x00;

    // Set program counter to value set by ROM.
    uint16_t arg = 0xFFFC;
    uint16_t low = read(arg);
    uint16_t high = read(arg + 1);
    pc = (high << 8) | low;

    s = 0xFD;
    p = I | U;
}

void CPU::reset() {
    // Set program counter to value set by ROM.
    uint16_t arg = 0xFFFC;
    uint16_t low = read(arg);
    uint16_t high = read(arg + 1);
    pc = (high << 8) | low;

    // Tries to push pc and p to stack but writes are prohibited during reset.
    s = s - 3;

    // Disable interrupts
    setFlag(I, 1);
}

void CPU::irq() {
    if (getFlag(I)) return; // Ignore interrupt requests if interrupts are disabled.
    interrupt(0xFFFE, 0);
}

void CPU::nmi() {
    interrupt(0xFFFA, 0);
}

uint8_t CPU::pop() {
    s++;
    return read(0x0100 + s);
}

void CPU::push(uint8_t data) {
    write(0x0100 + s, data);
    s--;
}

void CPU::interrupt(uint16_t addr, bool brk) {
    // Push program counter.
    push((pc >> 8) & 0x00FF);
    push(pc & 0x00FF);

    // Push the status register with the B flag set.
    setFlag(B, brk);
    push(p);

    // Disable interrupts
    setFlag(I, 1);

    // Set pc to a value defined at addr.
    uint16_t low = read(addr);
    uint16_t high = read(addr);
    pc = (high << 8) | low;
}

void CPU::irq() {
    if (getFlag(I)) return; // Ignore interupt requests if interupts are disabled.
    interrupt(0xFFFE);
}

void CPU::nmi() {
    interrupt(0xFFFA);
}

uint8_t CPU::getFlag(StatusFlag flag) {
    return (bool)(p & flag);
}

void CPU::setFlag(StatusFlag flag, bool value) {
    if (flag == U) return; // Should always be set.

    if (value) {
        p = p | flag;
    } else {
        p = p & ~flag;
    }
}

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
    uint16_t addr = (high << 8) | low;
    return addr;
}

uint16_t CPU::IZY() {
    // NOTE: might cause oops cycle.
    uint16_t arg = read(pc++);
    uint16_t low = read(arg);
    uint16_t high = read((arg + 1) & 0x00FF);
    uint16_t addr = (high << 8) | low + y;
    return addr;
}

uint16_t CPU::IMP() {
    return 0x0000;
}

uint16_t CPU::ACC() {
    return 0x0000;
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
    // NOTE: might cause oops cycle.
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