#include <cstdint>

#include "../headers/CPU.h"
#include "../headers/Bus.h"

using std::uint16_t;
using std::uint8_t;

void CPU::tick() {
    // Check if there is cycles remaining for the current instruction.
    // Consume cycles if there is wait time.
    if (wait) wait--;

    // Fetch the opcode.
    uint8_t opcode = read(pc++);

    // TODO: create lookup table for instructions.

    // Fetch addressing mode, operation and the cycles need to perform the opcode.
    // addrMode = opcodes[opcode].addrMode;
    // op = opcodes[opcode].op;
    // wait = opcodes[opcode].cycles;

    // Perform the operation.
    (this->*op)();

    // Add oops cycle if there was one.
    if (oops) wait++;
    oops = false;

    // The performed tick also consumed a wait cycle.
    wait--;
}

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

    // Interrupts takes time.
    wait = 7;
}

void CPU::nmi() {
    interrupt(0xFFFA, 0);

    // Interrupts takes time.
    wait = 7;
}

uint8_t CPU::read(uint16_t addr) {
    return bus->read(addr);
}

void CPU::write(uint16_t addr, uint8_t data) {
    bus->write(addr, data);
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

void CPU::branch() {
    // PC = PC + 2 + memory (signed)
    // +2 happens in relative addressing mode method.
    uint16_t mem = (this->*addrMode)();
    uint16_t res = pc + mem;

    // There is an extra cycle since the branch was taken.
    wait++;

    // If pc crossed a page there is an extra cycle.
    if (crossed(pc, res)) wait++;

    pc = res;
}

bool CPU::crossed(uint16_t arg, uint16_t addr) {
    return (arg & 0xFF00) != (addr & 0xFF00);
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
    uint16_t low = read(pc++);
    uint16_t high = read(pc++);
    uint16_t arg = (high << 8) | low;
    uint16_t addr = arg + x;

    // If a page is crossed there might be an oops cycle.
    oops = crossed(arg, addr);

    return addr;
}

uint16_t CPU::ABY() {
    uint16_t low = read(pc++);
    uint16_t high = read(pc++);
    uint16_t arg = (high << 8) | low;
    uint16_t addr = arg + y;

    // If a page is crossed there might be an oops cycle.
    oops = crossed(arg, addr);

    return addr;
}

uint16_t CPU::IDX() {
    uint16_t arg = read(pc++);
    uint16_t low = read((arg + x) & 0x00FF);
    uint16_t high = read((arg + x + 1) & 0x00FF);
    uint16_t addr = (high << 8) | low;
    return addr;
}

uint16_t CPU::IDY() {
    uint16_t arg = read(pc++);
    uint16_t low = read(arg);
    uint16_t high = read((arg + 1) & 0x00FF);
    uint16_t addr = (high << 8) | low + y;

    // If a page is crossed there might be an oops cycle.
    oops = crossed(high << 8, addr);

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

void CPU::ADC() {
    // A = A + memory + C
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint16_t res = a + mem + getFlag(C);

    // Set affected flags.
    setFlag(C, res > 0x00FF);
    setFlag(Z, res == 0x0000);
    setFlag(V, (res ^ a) & (res ^ mem) & 0x0080);
    setFlag(N, res & 0x0080);

    a = res & 0xFF;
}

void CPU::AND() {
    // A = A & memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = a & mem;

    // Set affected flags.
    setFlag(Z, res == 0x00);
    setFlag(N, res & 0x80);

    a = res;
}

void CPU::ASL() {
    // value = value << 1
    uint8_t val = 0x00;
    uint16_t addr = 0x0000;
    if (addrMode == &ACC) {
        val = a;
    } else {
        addr = (this->*addrMode)();
        val = read(addr);
    }

    uint8_t res = val << 1;

    // Set affected flags.
    setFlag(C, val & 0x80);
    setFlag(Z, res == 0x00);
    setFlag(N, res & 0x80);

    if (addrMode == &ACC) {
        a = res;
    } else {
        write(addr, res);
    }

    // Should not give an oops cycle.
    oops = false;
}

void CPU::BCC() {
    // Branch if C is clear.
    if (getFlag(C)) return;
    branch();
}

void CPU::BCS() {
    // Branch if C is set.
    if (!getFlag(C)) return;
    branch();
}

void CPU::BEQ() {
    // Branch if Z is set.
    if (!getFlag(Z)) return;
    branch();
}

void CPU::BIT() {
    // A & memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = a & mem;

    // Set affected flags.
    setFlag(Z, res == 0x00);
    setFlag(V, res & 0x40);
    setFlag(N, res & 0x80);
}

void CPU::BMI() {
    // Branch if N is set.
    if (!getFlag(N)) return;
    branch();
}

void CPU::BNE() {
    // Branch if Z is clear.
    if (getFlag(Z)) return;
    branch();
}

void CPU::BPL() {
    // Branch if N is clear.
    if (getFlag(N)) return;
    branch();
}

void CPU::BRK() {
    pc++; // break skips one address for some reason.
    interrupt(0xFFFE, 1);
}

void CPU::BVC() {
    // Branch if V is clear.
    if (getFlag(V)) return;
    branch();
}

void CPU::BVS() {
    // Branch if V is set.
    if (!getFlag(V)) return;
    branch();
}

void CPU::CLC() {
    // C = 0
    setFlag(C, 0);
}

void CPU::CLD() {
    // D = 0
    setFlag(D, 0);
}

void CPU::CLI() {
    // I = 0
    setFlag(I, 0);
}

void CPU::CLV() {
    // V = 0
    setFlag(V, 0);
}

void CPU::CMP() {
    // A - memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = a - mem;

    // Set affected flags.
    setFlag(C, a >= mem);
    setFlag(Z, a == mem);
    setFlag(N, res & 0x80);
}

void CPU::CPX() {
    // X - memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = x - mem;

    // Set affected flags.
    setFlag(C, x >= mem);
    setFlag(Z, x == mem);
    setFlag(N, res & 0x80);
}

void CPU::CPY() {
    // Y - memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = y - mem;

    // Set affected flags.
    setFlag(C, y >= mem);
    setFlag(Z, y == mem);
    setFlag(N, res & 0x80);
}

void CPU::DEC() {
    // memory = memory - 1
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = mem - 1;

    // Set affected flags.
    setFlag(Z, res == 0x00);
    setFlag(N, res & 0x80);

    write(addr, res);

    // Should not give an oops cycle.
    oops = false;
}

void CPU::DEX() {
    // X = X - 1
    uint8_t res = x - 1;

    // Set affected flags.
    setFlag(Z, res == 0x00);
    setFlag(N, res & 0x80);

    x = res;
}

void CPU::DEY() {
    // Y = Y - 1
    uint8_t res = y - 1;

    // Set affected flags.
    setFlag(Z, res == 0x00);
    setFlag(N, res & 0x80);

    y = res;
}

void CPU::EOR() {
    // A = A ^ memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = a ^ mem;

    // Set affected flags.
    setFlag(Z, res == 0x00);
    setFlag(N, res & 0x80);

    a = res;
}

void CPU::INC() {
    // memory = memory + 1
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = mem + 1;

    // Set affected flags.
    setFlag(Z, res == 0x00);
    setFlag(N, res & 0x80);

    write(addr, res);

    // Should not give an oops cycle.
    oops = false;
}

void CPU::INX() {
    // X = X + 1
    uint8_t res = x + 1;

    // Set affected flags.
    setFlag(Z, res == 0x00);
    setFlag(N, res & 0x80);

    x = res;
}

void CPU::INY() {
    // Y = Y + 1
    uint8_t res = y + 1;

    // Set affected flags.
    setFlag(Z, res == 0x00);
    setFlag(N, res & 0x80);

    y = res;
}

void CPU::JMP() {
    // PC = memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);

    pc = mem;
}

void CPU::JSR() {
    // Push program counter.
    push((pc >> 8) & 0x00FF);
    push(pc & 0x00FF);

    // PC = memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);

    pc = mem;
}

void CPU::LDA() {
    // A = memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);

    // Set affected flags.
    setFlag(Z, mem == 0x00);
    setFlag(N, mem & 0x80);

    a = mem;
}

void CPU::LDX() {
    // X = memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);

    // Set affected flags.
    setFlag(Z, mem == 0x00);
    setFlag(N, mem & 0x80);

    x = mem;
}

void CPU::LDY() {
    // Y = memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);

    // Set affected flags.
    setFlag(Z, mem == 0x00);
    setFlag(N, mem & 0x80);

    x = mem;
}

void CPU::LSR() {
    // value = value >> 1
    uint8_t val = 0x00;
    uint16_t addr = 0x0000;
    if (addrMode == &ACC) {
        val = a;
    } else {
        addr = (this->*addrMode)();
        val = read(addr);
    }

    uint8_t res = val >> 1;

    // Set affected flags.
    setFlag(C, val & 0x01);
    setFlag(Z, res == 0x00);
    setFlag(N, 0);

    if (addrMode == &ACC) {
        a = res;
    } else {
        write(addr, res);
    }

    // Should not give an oops cycle.
    oops = false;
}

void CPU::NOP() {
    // no effect
}

void CPU::ORA() {
    // A = A | memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = a | mem;

    // Set affected flags.
    setFlag(Z, res == 0x00);
    setFlag(N, res & 0x80);

    a = res;
}

void CPU::PHA() {
    // Push accumulator.
    push(a);
}

void CPU::PHP() {
    // Set affected flags.
    setFlag(B, 1);

    // Push status register.
    push(p);
}

void CPU::PLA() {
    // Pop into accumulator.
    a = pop();

    // Set affected flags.
    setFlag(Z, a == 0x00);
    setFlag(N, a & 0x80);
}

void CPU::PLP() {
    // Pop into status register.
    p = pop();
}

void CPU::ROL() {
    // value = value << 1 through C
    uint8_t val = 0x00;
    uint16_t addr = 0x0000;
    if (addrMode == &ACC) {
        val = a;
    } else {
        addr = (this->*addrMode)();
        val = read(addr);
    }

    uint8_t res = (val << 1) | getFlag(C);

    // Set affected flags.
    setFlag(C, val & 0x80);
    setFlag(Z, res == 0x00);
    setFlag(N, res & 0x80);

    if (addrMode == &ACC) {
        a = res;
    } else {
        write(addr, res);
    }

    // Should not give an oops cycle.
    oops = false;
}

void CPU::ROR() {
    // value = value >> 1 through C
    uint8_t val = 0x00;
    uint16_t addr = 0x0000;
    if (addrMode == &ACC) {
        val = a;
    } else {
        addr = (this->*addrMode)();
        val = read(addr);
    }

    uint8_t res = (getFlag(C) << 7) | (val >> 1);

    // Set affected flags.
    setFlag(C, val & 0x01);
    setFlag(Z, res == 0x00);
    setFlag(N, res & 0x80);

    if (addrMode == &ACC) {
        a = res;
    } else {
        write(addr, res);
    }

    // Should not give an oops cycle.
    oops = false;
}

void CPU::RTI() {
    // Pop status register.
    p = pop();

    // Pop program counter.
    uint16_t low = pop();
    uint16_t high = pop();
    pc = (high << 8) | low;
}

void CPU::RTS() {
    // Pop program counter.
    uint16_t low = pop();
    uint16_t high = pop();
    pc = (high << 8) | low;

    // Increment program counter.
    pc++;
}

void CPU::SBC() {
    // A = A - memory - ~C
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint16_t res = a - mem - ~getFlag(C);

    // Set affected flags.
    setFlag(C, res & 0xFF00);
    setFlag(Z, res == 0x0000);
    setFlag(V, (res ^ a) & (res ^ ~mem) & 0x0080);
    setFlag(N, res & 0x0080);

    a = res;
}

void CPU::SEC() {
    // C = 1
    setFlag(C, 1);
}

void CPU::SED() {
    // D = 1
    setFlag(D, 1);
}

void CPU::SEI() {
    // I = 1
    setFlag(I, 1);
}

void CPU::STA() {
    // memory = A
    uint16_t addr = (this->*addrMode)();
    write(addr, a);

    // Should not give an oops cycle.
    oops = false;
}

void CPU::STX() {
    // memory = X
    uint16_t addr = (this->*addrMode)();
    write(addr, x);
}

void CPU::STY() {
    // memory = Y
    uint16_t addr = (this->*addrMode)();
    write(addr, y);
}

void CPU::TAX() {
    // X = A
    x = a;

    // Set affected flags.
    setFlag(Z, x == 0x00);
    setFlag(N, x & 0x80);
}

void CPU::TAY() {
    // Y = A
    y = a;

    // Set affected flags.
    setFlag(Z, y == 0x00);
    setFlag(N, y & 0x80);
}

void CPU::TSX() {
    // X = SP
    x = s;

    // Set affected flags.
    setFlag(Z, x == 0x00);
    setFlag(N, x & 0x80);
}

void CPU::TXA() {
    // A = X
    a = x;

    // Set affected flags.
    setFlag(Z, a == 0x00);
    setFlag(N, a & 0x80);
}

void CPU::TXS() {
    // SP = X
    s = x;
}

void CPU::TYA() {
    // A = Y
    a = y;

    // Set affected flags.
    setFlag(Z, a == 0x00);
    setFlag(N, a & 0x80);
}

void CPU::AHX() {
    // Unimplemented.
}
void CPU::ALR() {
    // Unimplemented.
}
void CPU::ANC() {
    // Unimplemented.
}
void CPU::ARR() {
    // Unimplemented.
}
void CPU::AXS() {
    // Unimplemented.
}
void CPU::DCP() {
    // Unimplemented.
}
void CPU::ISC() {
    // Unimplemented.
}
void CPU::JAM() {
    // Unimplemented.
}
void CPU::LAS() {
    // Unimplemented.
}
void CPU::LAX() {
    // Unimplemented.
}
void CPU::RLA() {
    // Unimplemented.
}
void CPU::RRA() {
    // Unimplemented.
}
void CPU::SAX() {
    // Unimplemented.
}
void CPU::SHX() {
    // Unimplemented.
}
void CPU::SHY() {
    // Unimplemented.
}
void CPU::SLO() {
    // Unimplemented.
}
void CPU::SRE() {
    // Unimplemented.
}
void CPU::TAS() {
    // Unimplemented.
}
void CPU::XAA() {
    // Unimplemented.
}