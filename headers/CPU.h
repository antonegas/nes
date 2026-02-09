#ifndef H_CPU
#define H_CPU

#include <cstdint>
#include <array>

using std::uint16_t;
using std::uint8_t;

class Bus;

class CPU {
    public:
        void tick();
        // TODO: move to private and update delay to use enum
        void power();
        void reset();
        void irq();
        void nmi();
        void delay(void (CPU::*interrupt)()); // Trigger an interrupt after current instruction is done.
        
        bool suspended = false;
        bool dmaRead = true; // Is the CPU allowing DMA to read/not write.
        Bus *bus = nullptr;
    private:

        /**
         * HELPER MEMBER/FUNCTIONS
         */
        
        uint8_t wait = 0x00;
        bool oops = false;
        uint8_t opcode = 0x00;
        uint16_t opAddr = 0x0000;
        uint8_t priority = 0x00; // triggered interrupt priority.
        void (CPU::*addrMode)() = nullptr;
        void (CPU::*op)() = nullptr;
        void (CPU::*delayed)() = nullptr;

        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t data);
        uint8_t pop();
        void push(uint8_t data);
        void interrupt(uint16_t addr, bool brk);
        void branch();
        bool crossed(uint16_t arg, uint16_t addr);
        
        /**
         * CPU REGISTERS
         *
         * The CPU has six different registers: accumulator (A), indexes (X and Y), program counter
         * (PC), stack pointer (S) and status flags (P).
         * 
         * There are 7 different flags stored in the P byte: carry (C), zero (Z), interupt disable (I), 
         * decimal (D), b flag (B), overflow (V) and negative (N). The 6:th bit of the byte is not  
         * used and is always set to one. The b flag is not set by the ALU.
         *
         * CPU registers reference: https://www.nesdev.org/wiki/CPU_registers
         * CPU status flags reference: https://www.nesdev.org/wiki/Status_flags
         */

        uint8_t a = 0x00; // Accumulator
        uint8_t x = 0x00; // X index
        uint8_t y = 0x00; // Y index
        uint16_t pc = 0x0000; // Program counter (set by power/reset)
        uint8_t s = 0xFD; // Stack 
        union Status {
            struct {
                bool C : 1; // Carry
                bool Z : 1; // Zero
                bool I : 1; // Interrupt disable
                bool D : 1; // Decimal mode
                bool B : 1; //  B-flag
                bool U : 1; // Unused set to 1
                bool V : 1; // Overflow
                bool N : 1; // Negative
            };
            uint8_t status = 0b00100100;
        } p; // Status register P: NV1BDIZC

        /**
         * CPU ADDRESSING MODES
         * 
         * The CPU has 12 different addressing modes.
         *
         * Reference: https://www.nesdev.org/wiki/CPU_addressing_modes
         */

        void ZPX(); // Zero page indexed by X
        void ZPY(); // Zero page indexed by Y
        void ABX(); // Absolute indexed by X
        void ABY(); // Absolute indexed by Y
        void IDX(); // Indexed indirect by X
        void IDY(); // Indirect indexed by Y

        void IMP(); // Implicit addressing
        void ACC(); // Accumulator addressing
        void IMM(); // Immediate addressing
        void ZP0(); // Zero page addressing
        void ABS(); // Absolute adressing
        void REL(); // Relative addressing
        void IND(); // Indirect addressing

        /**
         * CPU INSTRUCTIONS
         * 
         * The CPU has 56 official instructions and 21 additional illegal opcodes.
         *
         * Reference: https://www.nesdev.org/wiki/Instruction_reference
         * Illegal opcodes reference: https://www.nesdev.org/wiki/CPU_unofficial_opcodes
         * Tick-by-tick instruction description: https://www.nesdev.org/6502_cpu.txt
         */

        void ADC(); // A = A + memory + C
        void AND(); // A = A & memory
        void ASL(); // value = value << 1
        void BCC(); // PC = PC + 2 + memory (signed)
        void BCS(); // PC = PC + 2 + memory (signed)
        void BEQ(); // PC = PC + 2 + memory (signed)
        void BIT(); // A & memory
        void BMI(); // PC = PC + 2 + memory (signed)
        void BNE(); // PC = PC + 2 + memory (signed)
        void BPL(); // PC = PC + 2 + memory (signed)
        void BRK(); // push PC + 2 to stack, push NV11DIZC flags to stack, PC = ($FFFE)
        void BVC(); // PC = PC + 2 + memory (signed)
        void BVS(); // PC = PC + 2 + memory (signed)
        void CLC(); // C = 0
        void CLD(); // D = 0
        void CLI(); // I = 0
        void CLV(); // V = 0
        void CMP(); // A - memory
        void CPX(); // X - memory
        void CPY(); // Y - memory
        void DEC(); // memory = memory - 1
        void DEX(); // X = X - 1
        void DEY(); // Y = Y - 1
        void EOR(); // A = A ^ memory
        void INC(); // memory = memory + 1
        void INX(); // X = X + 1
        void INY(); // Y = Y + 1
        void JMP(); // PC = memory
        void JSR(); // push PC + 2 to stack, PC = memory
        void LDA(); // A = memory
        void LDX(); // X = memory
        void LDY(); // Y = memory
        void LSR(); // value = value >> 1
        void NOP(); // no effect
        void ORA(); // A = A | memory
        void PHA(); // ($0100 + SP) = A, SP = SP - 1
        void PHP(); // ($0100 + SP) = NV11DIZC, SP = SP - 1
        void PLA(); // SP = SP + 1, A = ($0100 + SP)
        void PLP(); // SP = SP + 1, NVxxDIZC = ($0100 + SP)
        void ROL(); // value = value << 1 through C
        void ROR(); // value = value >> 1 through C
        void RTI(); // pull NVxxDIZC flags from stack, pull PC from stack
        void RTS(); // pull PC from stack, PC = PC + 1
        void SBC(); // A = A - memory - ~C
        void SEC(); // C = 1
        void SED(); // D = 1
        void SEI(); // I = 1
        void STA(); // memory = A
        void STX(); // memory = X
        void STY(); // memory = Y
        void TAX(); // X = A
        void TAY(); // Y = A
        void TSX(); // X = SP
        void TXA(); // A = X
        void TXS(); // SP = X
        void TYA(); // A = Y

        /**
         * ILLEGAL OPCODES
         *
         * Some opcodes although unintended have useful effects.
         *
         * Reference: https://www.oxyron.de/html/opcodes02.html
         */

        void AHX(); // memory = A & X & (address >> 8)
        void ALR(); // A = A & memory, then value = value >> 1
        void ANC(); // A = A & memory
        void ARR(); // A = A & memory, then value = value >> 1 through C
        void AXS(); // X = (A & X) - memory
        void DCP(); // memory = memory - 1, then A - memory
        void ISC(); // memory = memory + 1, then A = A - memory - ~C
        void KIL(); // Freezes the CPU.
        void LAS(); // A = S & memory, X = S & memory, S = S & memory
        void LAX(); // A = memory, then X = memory or A = memory, then X = A
        void RLA(); // value = value << 1 through C, then A = A & memory
        void RRA(); // value = value >> 1 through C, then A = A + memory + C
        void SAX(); // memory = A & X
        void SHX(); // memory = X & (address >> 8)
        void SHY(); // memory = Y & (address >> 8)
        void SLO(); // value = value << 1, then A = A | memory
        void SRE(); // value = value >> 1, then A = A ^ memory
        void TAS(); // S = A & X, memory = A & X & (address >> 8)
        void XAA(); // A = X, then A = A & memory

        /**
         * LOOKUP TABLE
         * 
         * The lookup table for all 256 opcodes.
         * 
         * Reference: https://www.oxyron.de/html/opcodes02.html
         */

        typedef struct {
            void (CPU::*addrMode)() = nullptr;
            void (CPU::*op)() = nullptr;
            uint8_t cycles = 0;
        } Lookup;

        std::array<Lookup, 256> opcodes = {{
        //                    X0               X1               X2               X3               X4               X5               X6               X7               X8               X9               XA               XB               XC               XD               XE               XF
        /* 0X */ {&CPU::IMP, &CPU::BRK, 7}, {&CPU::IDX, &CPU::ORA, 6}, {&CPU::IMP, &CPU::KIL, 0}, {&CPU::IDX, &CPU::SLO, 8}, {&CPU::ZP0, &CPU::NOP, 3}, {&CPU::ZP0, &CPU::ORA, 3}, {&CPU::ZP0, &CPU::ASL, 5}, {&CPU::ZP0, &CPU::SLO, 5}, {&CPU::IMP, &CPU::PHP, 3}, {&CPU::IMM, &CPU::ORA, 2}, {&CPU::ACC, &CPU::ASL, 2}, {&CPU::IMM, &CPU::ANC, 2}, {&CPU::ABS, &CPU::NOP, 4}, {&CPU::ABS, &CPU::ORA, 4}, {&CPU::ABS, &CPU::ASL, 6}, {&CPU::ABS, &CPU::SLO, 6},
        /* 1X */ {&CPU::REL, &CPU::BPL, 2}, {&CPU::IDY, &CPU::ORA, 5}, {&CPU::IMP, &CPU::KIL, 0}, {&CPU::IDY, &CPU::SLO, 8}, {&CPU::ZPX, &CPU::NOP, 4}, {&CPU::ZPX, &CPU::ORA, 4}, {&CPU::ZPX, &CPU::ASL, 6}, {&CPU::ZPX, &CPU::SLO, 6}, {&CPU::IMP, &CPU::CLC, 2}, {&CPU::ABY, &CPU::ORA, 4}, {&CPU::ACC, &CPU::NOP, 2}, {&CPU::ABY, &CPU::SLO, 7}, {&CPU::ABX, &CPU::NOP, 4}, {&CPU::ABX, &CPU::ORA, 4}, {&CPU::ABX, &CPU::ASL, 7}, {&CPU::ABX, &CPU::SLO, 7},
        /* 2X */ {&CPU::ABS, &CPU::JSR, 6}, {&CPU::IDX, &CPU::AND, 6}, {&CPU::IMP, &CPU::KIL, 0}, {&CPU::IDX, &CPU::RLA, 8}, {&CPU::ZP0, &CPU::BIT, 3}, {&CPU::ZP0, &CPU::AND, 3}, {&CPU::ZP0, &CPU::ROL, 5}, {&CPU::ZP0, &CPU::RLA, 5}, {&CPU::IMP, &CPU::PLP, 4}, {&CPU::IMM, &CPU::AND, 2}, {&CPU::ACC, &CPU::ROL, 2}, {&CPU::IMM, &CPU::ANC, 2}, {&CPU::ABS, &CPU::BIT, 4}, {&CPU::ABS, &CPU::AND, 4}, {&CPU::ABS, &CPU::ROL, 6}, {&CPU::ABS, &CPU::RLA, 6},
        /* 3X */ {&CPU::REL, &CPU::BMI, 2}, {&CPU::IDY, &CPU::AND, 5}, {&CPU::IMP, &CPU::KIL, 0}, {&CPU::IDY, &CPU::RLA, 8}, {&CPU::ZPX, &CPU::NOP, 4}, {&CPU::ZPX, &CPU::AND, 4}, {&CPU::ZPX, &CPU::ROL, 6}, {&CPU::ZPX, &CPU::RLA, 6}, {&CPU::IMP, &CPU::SEC, 2}, {&CPU::ABY, &CPU::AND, 4}, {&CPU::ACC, &CPU::NOP, 2}, {&CPU::ABY, &CPU::RLA, 7}, {&CPU::ABX, &CPU::NOP, 4}, {&CPU::ABX, &CPU::AND, 4}, {&CPU::ABX, &CPU::ROL, 7}, {&CPU::ABX, &CPU::RLA, 7},
        /* 4X */ {&CPU::IMP, &CPU::RTI, 6}, {&CPU::IDX, &CPU::EOR, 6}, {&CPU::IMP, &CPU::KIL, 0}, {&CPU::IDX, &CPU::SRE, 8}, {&CPU::ZP0, &CPU::NOP, 3}, {&CPU::ZP0, &CPU::EOR, 3}, {&CPU::ZP0, &CPU::LSR, 5}, {&CPU::ZP0, &CPU::SRE, 5}, {&CPU::IMP, &CPU::PHA, 3}, {&CPU::IMM, &CPU::EOR, 2}, {&CPU::ACC, &CPU::LSR, 2}, {&CPU::IMM, &CPU::ALR, 2}, {&CPU::ABS, &CPU::JMP, 3}, {&CPU::ABS, &CPU::EOR, 4}, {&CPU::ABS, &CPU::LSR, 6}, {&CPU::ABS, &CPU::SRE, 6},
        /* 5X */ {&CPU::REL, &CPU::BVC, 2}, {&CPU::IDY, &CPU::EOR, 5}, {&CPU::IMP, &CPU::KIL, 0}, {&CPU::IDY, &CPU::SRE, 8}, {&CPU::ZPX, &CPU::NOP, 4}, {&CPU::ZPX, &CPU::EOR, 4}, {&CPU::ZPX, &CPU::LSR, 6}, {&CPU::ZPX, &CPU::SRE, 6}, {&CPU::IMP, &CPU::CLI, 2}, {&CPU::ABY, &CPU::EOR, 4}, {&CPU::ACC, &CPU::NOP, 2}, {&CPU::ABY, &CPU::SRE, 7}, {&CPU::ABX, &CPU::NOP, 4}, {&CPU::ABX, &CPU::EOR, 4}, {&CPU::ABX, &CPU::LSR, 7}, {&CPU::ABX, &CPU::SRE, 7},
        /* 6X */ {&CPU::IMP, &CPU::RTS, 6}, {&CPU::IDX, &CPU::ADC, 6}, {&CPU::IMP, &CPU::KIL, 0}, {&CPU::IDX, &CPU::RRA, 8}, {&CPU::ZP0, &CPU::NOP, 3}, {&CPU::ZP0, &CPU::ADC, 3}, {&CPU::ZP0, &CPU::ROR, 5}, {&CPU::ZP0, &CPU::RRA, 5}, {&CPU::IMP, &CPU::PLA, 4}, {&CPU::IMM, &CPU::ADC, 2}, {&CPU::ACC, &CPU::ROR, 2}, {&CPU::IMM, &CPU::ARR, 2}, {&CPU::IND, &CPU::JMP, 5}, {&CPU::ABS, &CPU::ADC, 4}, {&CPU::ABS, &CPU::ROR, 6}, {&CPU::ABS, &CPU::RRA, 6},
        /* 7X */ {&CPU::REL, &CPU::BVS, 2}, {&CPU::IDY, &CPU::ADC, 5}, {&CPU::IMP, &CPU::KIL, 0}, {&CPU::IDY, &CPU::RRA, 8}, {&CPU::ZPX, &CPU::NOP, 4}, {&CPU::ZPX, &CPU::ADC, 4}, {&CPU::ZPX, &CPU::ROR, 6}, {&CPU::ZPX, &CPU::RRA, 6}, {&CPU::IMP, &CPU::SEI, 2}, {&CPU::ABY, &CPU::ADC, 4}, {&CPU::ACC, &CPU::NOP, 2}, {&CPU::ABY, &CPU::RRA, 7}, {&CPU::ABX, &CPU::NOP, 4}, {&CPU::ABX, &CPU::ADC, 4}, {&CPU::ABX, &CPU::ROR, 7}, {&CPU::ABX, &CPU::RRA, 7},
        /* 8X */ {&CPU::IMM, &CPU::NOP, 2}, {&CPU::IDX, &CPU::STA, 6}, {&CPU::IMM, &CPU::NOP, 2}, {&CPU::IDX, &CPU::SAX, 6}, {&CPU::ZP0, &CPU::STY, 3}, {&CPU::ZP0, &CPU::STA, 3}, {&CPU::ZP0, &CPU::STX, 3}, {&CPU::ZP0, &CPU::SAX, 3}, {&CPU::IMP, &CPU::DEY, 2}, {&CPU::IMM, &CPU::NOP, 2}, {&CPU::IMP, &CPU::TXA, 2}, {&CPU::IMM, &CPU::XAA, 2}, {&CPU::ABS, &CPU::STY, 4}, {&CPU::ABS, &CPU::STA, 4}, {&CPU::ABS, &CPU::STX, 4}, {&CPU::ABS, &CPU::SAX, 4},
        /* 9X */ {&CPU::REL, &CPU::BCC, 2}, {&CPU::IDY, &CPU::STA, 6}, {&CPU::IMP, &CPU::KIL, 0}, {&CPU::IDY, &CPU::AHX, 6}, {&CPU::ZPX, &CPU::STY, 4}, {&CPU::ZPX, &CPU::STA, 4}, {&CPU::ZPY, &CPU::STX, 4}, {&CPU::ZPY, &CPU::SAX, 4}, {&CPU::IMP, &CPU::TYA, 2}, {&CPU::ABY, &CPU::STA, 5}, {&CPU::IMP, &CPU::TXS, 2}, {&CPU::ABY, &CPU::TAS, 5}, {&CPU::ABX, &CPU::SHY, 5}, {&CPU::ABX, &CPU::STA, 5}, {&CPU::ABY, &CPU::SHX, 5}, {&CPU::ABY, &CPU::AHX, 5},
        /* AX */ {&CPU::IMM, &CPU::LDY, 2}, {&CPU::IDX, &CPU::LDA, 6}, {&CPU::IMM, &CPU::LDX, 2}, {&CPU::IDX, &CPU::LAX, 6}, {&CPU::ZP0, &CPU::LDY, 3}, {&CPU::ZP0, &CPU::LDA, 3}, {&CPU::ZP0, &CPU::LDX, 3}, {&CPU::ZP0, &CPU::LAX, 3}, {&CPU::IMP, &CPU::TAY, 2}, {&CPU::IMM, &CPU::LDA, 2}, {&CPU::IMP, &CPU::TAX, 2}, {&CPU::IMM, &CPU::LAX, 2}, {&CPU::ABS, &CPU::LDY, 4}, {&CPU::ABS, &CPU::LDA, 4}, {&CPU::ABS, &CPU::LDX, 4}, {&CPU::ABS, &CPU::LAX, 4},
        /* BX */ {&CPU::REL, &CPU::BCS, 2}, {&CPU::IDY, &CPU::LDA, 5}, {&CPU::IMP, &CPU::KIL, 0}, {&CPU::IDY, &CPU::LAX, 5}, {&CPU::ZPX, &CPU::LDY, 4}, {&CPU::ZPX, &CPU::LDA, 4}, {&CPU::ZPY, &CPU::LDX, 4}, {&CPU::ZPY, &CPU::LAX, 4}, {&CPU::IMP, &CPU::CLV, 2}, {&CPU::ABY, &CPU::LDA, 4}, {&CPU::IMP, &CPU::TSX, 2}, {&CPU::ABY, &CPU::LAS, 4}, {&CPU::ABX, &CPU::LDY, 4}, {&CPU::ABX, &CPU::LDA, 4}, {&CPU::ABY, &CPU::LDX, 4}, {&CPU::ABY, &CPU::LAX, 4},
        /* CX */ {&CPU::IMM, &CPU::CPY, 2}, {&CPU::IDX, &CPU::CMP, 6}, {&CPU::IMM, &CPU::NOP, 2}, {&CPU::IDX, &CPU::DCP, 8}, {&CPU::ZP0, &CPU::CPY, 3}, {&CPU::ZP0, &CPU::CMP, 3}, {&CPU::ZP0, &CPU::DEC, 5}, {&CPU::ZP0, &CPU::DCP, 5}, {&CPU::IMP, &CPU::INY, 2}, {&CPU::IMM, &CPU::CMP, 2}, {&CPU::IMP, &CPU::DEX, 2}, {&CPU::IMM, &CPU::AXS, 2}, {&CPU::ABS, &CPU::CPY, 4}, {&CPU::ABS, &CPU::CMP, 4}, {&CPU::ABS, &CPU::DEC, 6}, {&CPU::ABS, &CPU::DCP, 6},
        /* DX */ {&CPU::REL, &CPU::BNE, 2}, {&CPU::IDY, &CPU::CMP, 5}, {&CPU::IMP, &CPU::KIL, 0}, {&CPU::IDY, &CPU::DCP, 8}, {&CPU::ZPX, &CPU::NOP, 4}, {&CPU::ZPX, &CPU::CMP, 4}, {&CPU::ZPX, &CPU::DEC, 6}, {&CPU::ZPX, &CPU::DCP, 6}, {&CPU::IMP, &CPU::CLD, 2}, {&CPU::ABY, &CPU::CMP, 4}, {&CPU::IMP, &CPU::NOP, 2}, {&CPU::ABY, &CPU::DCP, 7}, {&CPU::ABX, &CPU::NOP, 4}, {&CPU::ABX, &CPU::CMP, 4}, {&CPU::ABX, &CPU::DEC, 7}, {&CPU::ABX, &CPU::DCP, 7},
        /* EX */ {&CPU::IMM, &CPU::CPX, 2}, {&CPU::IDX, &CPU::SBC, 6}, {&CPU::IMM, &CPU::NOP, 2}, {&CPU::IDX, &CPU::ISC, 8}, {&CPU::ZP0, &CPU::CPX, 3}, {&CPU::ZP0, &CPU::SBC, 3}, {&CPU::ZP0, &CPU::INC, 5}, {&CPU::ZP0, &CPU::ISC, 5}, {&CPU::IMP, &CPU::INX, 2}, {&CPU::IMM, &CPU::SBC, 2}, {&CPU::IMP, &CPU::NOP, 2}, {&CPU::IMM, &CPU::SBC, 2}, {&CPU::ABS, &CPU::CPX, 4}, {&CPU::ABS, &CPU::SBC, 4}, {&CPU::ABS, &CPU::INC, 6}, {&CPU::ABS, &CPU::ISC, 6},
        /* FX */ {&CPU::REL, &CPU::BEQ, 2}, {&CPU::IDY, &CPU::SBC, 5}, {&CPU::IMP, &CPU::KIL, 0}, {&CPU::IDY, &CPU::ISC, 8}, {&CPU::ZPX, &CPU::NOP, 4}, {&CPU::ZPX, &CPU::SBC, 4}, {&CPU::ZPX, &CPU::INC, 6}, {&CPU::ZPX, &CPU::ISC, 6}, {&CPU::IMP, &CPU::SED, 2}, {&CPU::ABY, &CPU::SBC, 4}, {&CPU::IMP, &CPU::NOP, 2}, {&CPU::ABY, &CPU::ISC, 7}, {&CPU::ABX, &CPU::NOP, 4}, {&CPU::ABX, &CPU::SBC, 4}, {&CPU::ABX, &CPU::INC, 7}, {&CPU::ABX, &CPU::ISC, 7}
        }};
};

#endif // H_CPU