#include <cstdint>

#include "../headers/CPU.h"
#include "../headers/Bus.h"

using std::uint16_t;
using std::uint8_t;

void CPU::tick() {
    // If a KIL instruction is called the CPU should halt.
    if (op == &KIL) return;

    // CPU switches being allowing DMA to read or write each cycle.
    dmaRead = !dmaRead;

    if (suspended) return; // If the CPU is suspended only update DMA read/write.

    // Check if there is cycles remaining for the current instruction.
    // Consume cycles if there is wait time.
    if (wait) {
        wait--;
        return;
    }

    // Trigger delayed interrupts.
    if (delayed != nullptr) {
        (this->*delayed)();
        delayed = nullptr;
        priority = 0x00;
        wait--;
        return;
    }

    // Fetch the opcode.
    uint8_t opcode = read(pc++);

    // Fetch addressing mode, operation and the cycles need to perform the opcode.
    addrMode = opcodes[opcode].addrMode;
    op = opcodes[opcode].op;
    wait = opcodes[opcode].cycles;

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
    p.status = 0b00100100;

    // Clean up helper members.
    wait = 0x00;
    oops = false;
    addrMode = nullptr;
    op = nullptr;
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
    p.I = 1;

    // Clean up helper members.
    wait = 0x00;
    oops = false;
    addrMode = nullptr;
    op = nullptr;

    // Reset takes time.
    wait = 7;
}

void CPU::irq() {
    if (p.I) return; // Ignore interrupt requests if interrupts are disabled.
    interrupt(0xFFFE, 0);

    // Interrupts takes time.
    wait = 7;
}

void CPU::nmi() {
    interrupt(0xFFFA, 0);

    // Interrupts takes time.
    wait = 7;
}

void CPU::delay(void (CPU::*interrupt)()) {
    if (priority >= 0x03) return;

    if (interrupt == &reset) {
        delayed = interrupt;
        priority = 0x03;
    } else if (interrupt == &nmi && priority < 0x02) {
        delayed = interrupt;
        priority = 0x02;
    } else if (interrupt == &irq && priority < 0x01 && !p.I) {
        delayed = interrupt;
        priority = 0x01;
    }
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
    if (brk) {
        push(p.status | 0x10);
    } else {
        push(p.status);
    }

    // Disable interrupts
    p.I = 1;

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
    uint16_t res = a + mem + p.C;

    // Set affected flags.
    p.C = res > 0x00FF;
    p.Z = res == 0x0000;
    p.V = (res ^ a) & (res ^ mem) & 0x0080;
    p.N = res & 0x0080;

    a = res & 0xFF;
}

void CPU::AND() {
    // A = A & memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = a & mem;

    // Set affected flags.
    p.Z = res == 0x00;
    p.N = res & 0x80;

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
    p.C = val & 0x80;
    p.Z, res == 0x00;
    p.N, res & 0x80;

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
    if (p.C) return;
    branch();
}

void CPU::BCS() {
    // Branch if C is set.
    if (!p.C) return;
    branch();
}

void CPU::BEQ() {
    // Branch if Z is set.
    if (!p.Z) return;
    branch();
}

void CPU::BIT() {
    // A & memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = a & mem;

    // Set affected flags.
    p.Z = res == 0x00;
    p.V = res & 0x40;
    p.N = res & 0x80;
}

void CPU::BMI() {
    // Branch if N is set.
    if (!p.N) return;
    branch();
}

void CPU::BNE() {
    // Branch if Z is clear.
    if (p.Z) return;
    branch();
}

void CPU::BPL() {
    // Branch if N is clear.
    if (p.N) return;
    branch();
}

void CPU::BRK() {
    pc++; // break skips one address for some reason.
    interrupt(0xFFFE, 1);
}

void CPU::BVC() {
    // Branch if V is clear.
    if (p.V) return;
    branch();
}

void CPU::BVS() {
    // Branch if V is set.
    if (!p.V) return;
    branch();
}

void CPU::CLC() {
    // C = 0
    p.C = 0;
}

void CPU::CLD() {
    // D = 0
    p.D = 0;
}

void CPU::CLI() {
    // I = 0
    p.I = 0;
}

void CPU::CLV() {
    // V = 0
    p.V = 0;
}

void CPU::CMP() {
    // A - memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = a - mem;

    // Set affected flags.
    p.C = a >= mem;
    p.Z = a == mem;
    p.N = res & 0x80;
}

void CPU::CPX() {
    // X - memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = x - mem;

    // Set affected flags.
    p.C = x >= mem;
    p.Z = x == mem;
    p.N = res & 0x80;
}

void CPU::CPY() {
    // Y - memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = y - mem;

    // Set affected flags.
    p.C = y >= mem;
    p.Z = y == mem;
    p.N = res & 0x80;
}

void CPU::DEC() {
    // memory = memory - 1
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = mem - 1;

    // Set affected flags.
    p.Z = res == 0x00;
    p.N = res & 0x80;

    write(addr, res);

    // Should not give an oops cycle.
    oops = false;
}

void CPU::DEX() {
    // X = X - 1
    uint8_t res = x - 1;

    // Set affected flags.
    p.Z = res == 0x00;
    p.N = res & 0x80;

    x = res;
}

void CPU::DEY() {
    // Y = Y - 1
    uint8_t res = y - 1;

    // Set affected flags.
    p.Z = res == 0x00;
    p.N = res & 0x80;

    y = res;
}

void CPU::EOR() {
    // A = A ^ memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = a ^ mem;

    // Set affected flags.
    p.Z = res == 0x00;
    p.N = res & 0x80;

    a = res;
}

void CPU::INC() {
    // memory = memory + 1
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = mem + 1;

    // Set affected flags.
    p.Z = res == 0x00;
    p.N = res & 0x80;

    write(addr, res);

    // Should not give an oops cycle.
    oops = false;
}

void CPU::INX() {
    // X = X + 1
    uint8_t res = x + 1;

    // Set affected flags.
    p.Z = res == 0x00;
    p.N = res & 0x80;

    x = res;
}

void CPU::INY() {
    // Y = Y + 1
    uint8_t res = y + 1;

    // Set affected flags.
    p.Z = res == 0x00;
    p.N = res & 0x80;

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
    p.Z = mem == 0x00;
    p.N = mem & 0x80;

    a = mem;
}

void CPU::LDX() {
    // X = memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);

    // Set affected flags.
    p.Z = mem == 0x00;
    p.N = mem & 0x80;

    x = mem;
}

void CPU::LDY() {
    // Y = memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);

    // Set affected flags.
    p.Z = mem == 0x00;
    p.N = mem & 0x80;

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
    p.C = val & 0x01;
    p.Z = res == 0x00;
    p.N = 0;

    if (addrMode == &ACC) {
        a = res;
    } else {
        write(addr, res);
    }

    // Should not give an oops cycle.
    oops = false;
}

void CPU::NOP() {
    // No effect, but might still add oops cycles.
    (this->*addrMode)();
}

void CPU::ORA() {
    // A = A | memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = a | mem;

    // Set affected flags.
    p.Z = res == 0x00;
    p.N = res & 0x80;

    a = res;
}

void CPU::PHA() {
    // Push accumulator.
    push(a);
}

void CPU::PHP() {
    // Push status register.
    push(p.status | 0x10);
}

void CPU::PLA() {
    // Pop into accumulator.
    a = pop();

    // Set affected flags.
    p.Z = a == 0x00;
    p.N = a & 0x80;
}

void CPU::PLP() {
    // Pop into status register.
    p.status = pop();
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

    uint8_t res = (val << 1) | p.C;

    // Set affected flags.
    p.C = val & 0x80;
    p.Z = res == 0x00;
    p.N = res & 0x80;

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

    uint8_t res = (p.C << 7) | (val >> 1);

    // Set affected flags.
    p.C = val & 0x01;
    p.Z = res == 0x00;
    p.N = res & 0x80;

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
    p.status = pop();

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
    uint16_t res = a - mem - ~p.C;

    // Set affected flags.
    p.C = res & 0xFF00;
    p.Z = res == 0x0000;
    p.V = (res ^ a) & (res ^ ~mem) & 0x0080;
    p.N = res & 0x0080;

    a = res;
}

void CPU::SEC() {
    // C = 1
    p.C = 1;
}

void CPU::SED() {
    // D = 1
    p.D = 1;
}

void CPU::SEI() {
    // I = 1
    p.I = 1;
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
    p.Z = x == 0x00;
    p.N = x & 0x80;
}

void CPU::TAY() {
    // Y = A
    y = a;

    // Set affected flags.
    p.Z = y == 0x00;
    p.N = y & 0x80;
}

void CPU::TSX() {
    // X = SP
    x = s;

    // Set affected flags.
    p.Z = x == 0x00;
    p.N = x & 0x80;
}

void CPU::TXA() {
    // A = X
    a = x;

    // Set affected flags.
    p.Z = a == 0x00;
    p.N = a & 0x80;
}

void CPU::TXS() {
    // SP = X
    s = x;
}

void CPU::TYA() {
    // A = Y
    a = y;

    // Set affected flags.
    p.Z = a == 0x00;
    p.N = a & 0x80;
}

void CPU::AHX() {
    // memory = A & X & (address >> 8)
    // NOTE: in real hardware this behavior is not stable.
    uint16_t addr = (this->*addrMode)();
    uint16_t res = a & x & (addr >> 8);

    write(addr, res);

    // Should not give an oops cycle.
    oops = false;
}

void CPU::ALR() {
    // A = A & memory, then value = value >> 1
    AND();
    LSR();
}

void CPU::ANC() {
    // A = A & memory
    AND();

    // Set affected flags.
    p.C = a & 0x80;
}

void CPU::ARR() {
    // A = A & memory, then value = value >> 1 through C
    AND();
    ROR();

    // Set affected flags.
    p.V = (a ^ (a << 1)) & 0x40;
}

void CPU::AXS() {
    // X = (A & X) - memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint16_t res = (a & x) - mem;

    // Set affected flags.
    p.C = (a & x) >= mem;
    p.Z = (a & x) == mem;
    p.N = res & 0x80;
    
    x = res;
}

void CPU::DCP() {
    // memory = memory - 1, then A - memory
    DEC();
    CMP();

    // Should not give an oops cycle.
    oops = false;
}

void CPU::ISC() {
    // memory = memory + 1, then A = A - memory - ~C
    INC();
    SBC();

    // Should not give an oops cycle.
    oops = false;
}

void CPU::KIL() {
    // Freezes the CPU.
}

void CPU::LAS() {
    // A = S & memory, X = S & memory, S = S & memory
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint8_t res = s & mem;

    // Set affected flags.
    p.Z = res == 0x00;
    p.N = res & 0x80;

    a = res;
    x = res;
    s = res;
}

void CPU::LAX() {
    // A = memory, then X = memory or X = A
    LDA();
    if (addrMode == &IMM) {
        TAX();
    } else {
        LDX();
    }
}

void CPU::RLA() {
    // value = value << 1 through C, then A = A & memory
    ROL();
    AND();

    // Should not give an oops cycle.
    oops = false;
}

void CPU::RRA() {
    // value = value >> 1 through C, then A = A + memory + C
    ROR();
    ADC();

    // Should not give an oops cycle.
    oops = false;
}

void CPU::SAX() {
    // memory = A & X
    uint16_t addr = (this->*addrMode)();
    write(addr, a & x);
}

void CPU::SHX() {
    // memory = X & (address >> 8)
    // NOTE: in real hardware this behavior is not stable.
    uint16_t addr = (this->*addrMode)();
    uint16_t res = x & (addr >> 8);

    write(addr, res);

    // Should not give an oops cycle.
    oops = false;
}

void CPU::SHY() {
    // memory = Y & (address >> 8)
    // NOTE: in real hardware this behavior is not stable.
    uint16_t addr = (this->*addrMode)();
    uint16_t res = y & (addr >> 8);

    write(addr, res);

    // Should not give an oops cycle.
    oops = false;
}

void CPU::SLO() {
    // value = value << 1, then A = A | memory
    ASL();
    ORA();

    // Should not give an oops cycle.
    oops = false;
}

void CPU::SRE() {
    // value = value >> 1, then A = A ^ memory
    LSR();
    EOR();

    // Should not give an oops cycle.
    oops = false;
}

void CPU::TAS() {
    // S = A & X, memory = A & X & (address >> 8)
    // NOTE: in real hardware this behavior is not stable.
    uint16_t addr = (this->*addrMode)();
    uint8_t mem = read(addr);
    uint16_t res = a & x;

    s = res;

    res = res & (addr >> 8);

    write(addr, res);

    // Should not give an oops cycle.
    oops = false;
}

void CPU::XAA() {
    // A = X, then A = A & memory
    TXA();
    AND();
}