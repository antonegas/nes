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
        uint8_t priority = 0x00; // triggered interrupt priority.
        uint16_t (CPU::*addrMode)() = nullptr;
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
        uint16_t pc = 0x00; // Program counter (set by power/reset)
        uint8_t s = 0xFD; // Stack 
        union Status {
            struct {
                uint8_t C : 1; // Carry
                uint8_t Z : 1; // Zero
                uint8_t I : 1; // Interrupt disable
                uint8_t D : 1; // Decimal mode
                uint8_t B : 1; //  B-flag
                uint8_t U : 1; // Unused set to 1
                uint8_t V : 1; // Overflow
                uint8_t N : 1; // Negative
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

        uint16_t ZPX(); // Zero page indexed by X
        uint16_t ZPY(); // Zero page indexed by Y
        uint16_t ABX(); // Absolute indexed by X
        uint16_t ABY(); // Absolute indexed by Y
        uint16_t IDX(); // Indexed indirect by X
        uint16_t IDY(); // Indirect Indexed by Y

        uint16_t IMP(); // Implicit addressing
        uint16_t ACC(); // Accumulator addressing
        uint16_t IMM(); // Immediate addressing
        uint16_t ZP0(); // Zero page addressing
        uint16_t ABS(); // Absolute adressing
        uint16_t REL(); // Relative addressing
        uint16_t IND(); // Indirect addressing

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
            uint16_t (CPU::*addrMode)() = nullptr;
            void (CPU::*op)() = nullptr;
            uint8_t cycles = 0;
        } Lookup;

        std::array<Lookup, 256> opcodes = {{
        //                    X0               X1               X2               X3               X4               X5               X6               X7               X8               X9               XA               XB               XC               XD               XE               XF
        /* 0X */ {&IMP, &BRK, 7}, {&IDX, &ORA, 6}, {&IMP, &KIL, 0}, {&IDX, &SLO, 8}, {&ZP0, &NOP, 3}, {&ZP0, &ORA, 3}, {&ZP0, &ASL, 5}, {&ZP0, &SLO, 5}, {&IMP, &PHP, 3}, {&IMM, &ORA, 2}, {&ACC, &ASL, 2}, {&IMM, &ANC, 2}, {&ABS, &NOP, 4}, {&ABS, &ORA, 4}, {&ABS, &ASL, 6}, {&ABS, &SLO, 6},
        /* 1X */ {&REL, &BPL, 2}, {&IDY, &ORA, 5}, {&IMP, &KIL, 0}, {&IDY, &SLO, 8}, {&ZPX, &NOP, 4}, {&ZPX, &ORA, 4}, {&ZPX, &ASL, 6}, {&ZPX, &SLO, 6}, {&IMP, &CLC, 2}, {&ABY, &ORA, 4}, {&ACC, &NOP, 2}, {&ABY, &SLO, 7}, {&ABX, &NOP, 4}, {&ABX, &ORA, 4}, {&ABX, &ASL, 7}, {&ABX, &SLO, 7},
        /* 2X */ {&ABS, &JSR, 6}, {&IDX, &AND, 6}, {&IMP, &KIL, 0}, {&IDX, &RLA, 8}, {&ZP0, &BIT, 3}, {&ZP0, &AND, 3}, {&ZP0, &ROL, 5}, {&ZP0, &RLA, 5}, {&IMP, &PLP, 4}, {&IMM, &AND, 2}, {&ACC, &ROL, 2}, {&IMM, &ANC, 2}, {&ABS, &BIT, 4}, {&ABS, &AND, 4}, {&ABS, &ROL, 6}, {&ABS, &RLA, 6},
        /* 3X */ {&REL, &BMI, 2}, {&IDY, &AND, 5}, {&IMP, &KIL, 0}, {&IDY, &RLA, 8}, {&ZPX, &NOP, 4}, {&ZPX, &AND, 4}, {&ZPX, &ROL, 6}, {&ZPX, &RLA, 6}, {&IMP, &SEC, 2}, {&ABY, &AND, 4}, {&ACC, &NOP, 2}, {&ABY, &RLA, 7}, {&ABX, &NOP, 4}, {&ABX, &AND, 4}, {&ABX, &ROL, 7}, {&ABX, &RLA, 7},
        /* 4X */ {&IMP, &RTI, 6}, {&IDX, &EOR, 6}, {&IMP, &KIL, 0}, {&IDX, &SRE, 8}, {&ZP0, &NOP, 3}, {&ZP0, &EOR, 3}, {&ZP0, &LSR, 5}, {&ZP0, &SRE, 5}, {&IMP, &PHA, 3}, {&IMM, &EOR, 2}, {&ACC, &LSR, 2}, {&IMM, &ALR, 2}, {&ABS, &JMP, 3}, {&ABS, &EOR, 4}, {&ABS, &LSR, 6}, {&ABS, &SRE, 6},
        /* 5X */ {&REL, &BVC, 2}, {&IDY, &EOR, 5}, {&IMP, &KIL, 0}, {&IDY, &SRE, 8}, {&ZPX, &NOP, 4}, {&ZPX, &EOR, 4}, {&ZPX, &LSR, 6}, {&ZPX, &SRE, 6}, {&IMP, &CLI, 2}, {&ABY, &EOR, 4}, {&ACC, &NOP, 2}, {&ABY, &SRE, 7}, {&ABX, &NOP, 4}, {&ABX, &EOR, 4}, {&ABX, &LSR, 7}, {&ABX, &SRE, 7},
        /* 6X */ {&IMP, &RTS, 6}, {&IDX, &ADC, 6}, {&IMP, &KIL, 0}, {&IDX, &RRA, 8}, {&ZP0, &NOP, 3}, {&ZP0, &ADC, 3}, {&ZP0, &ROR, 5}, {&ZP0, &RRA, 5}, {&IMP, &PLA, 4}, {&IMM, &ADC, 2}, {&ACC, &ROR, 2}, {&IMM, &ARR, 2}, {&IND, &JMP, 5}, {&ABS, &ADC, 4}, {&ABS, &ROR, 6}, {&ABS, &RRA, 6},
        /* 7X */ {&REL, &BVS, 2}, {&IDY, &ADC, 5}, {&IMP, &KIL, 0}, {&IDY, &RRA, 8}, {&ZPX, &NOP, 4}, {&ZPX, &ADC, 4}, {&ZPX, &ROR, 6}, {&ZPX, &RRA, 6}, {&IMP, &SEI, 2}, {&ABY, &ADC, 4}, {&ACC, &NOP, 2}, {&ABY, &RRA, 7}, {&ABX, &NOP, 4}, {&ABX, &ADC, 4}, {&ABX, &ROR, 7}, {&ABX, &RRA, 7},
        /* 8X */ {&IMM, &NOP, 2}, {&IDX, &STA, 6}, {&IMM, &NOP, 2}, {&IDX, &SAX, 6}, {&ZP0, &STY, 3}, {&ZP0, &STA, 3}, {&ZP0, &STX, 3}, {&ZP0, &SAX, 3}, {&IMP, &DEY, 2}, {&IMM, &NOP, 2}, {&IMP, &TXA, 2}, {&IMM, &XAA, 2}, {&ABS, &STY, 4}, {&ABS, &STA, 4}, {&ABS, &STX, 4}, {&ABS, &SAX, 4},
        /* 9X */ {&REL, &BCC, 2}, {&IDY, &STA, 6}, {&IMP, &KIL, 0}, {&IDY, &AHX, 6}, {&ZPX, &STY, 4}, {&ZPX, &STA, 4}, {&ZPY, &STX, 4}, {&ZPY, &SAX, 4}, {&IMP, &TYA, 2}, {&ABY, &STA, 5}, {&IMP, &TXS, 2}, {&ABY, &TAS, 5}, {&ABX, &SHY, 5}, {&ABX, &STA, 5}, {&ABY, &SHX, 5}, {&ABY, &AHX, 5},
        /* AX */ {&IMM, &LDY, 2}, {&IDX, &LDA, 6}, {&IMM, &LDX, 2}, {&IDX, &LAX, 6}, {&ZP0, &LDY, 3}, {&ZP0, &LDA, 3}, {&ZP0, &LDX, 3}, {&ZP0, &LAX, 3}, {&IMP, &TAY, 2}, {&IMM, &LDA, 2}, {&IMP, &TAX, 2}, {&IMM, &LAX, 2}, {&ABS, &LDY, 4}, {&ABS, &LDA, 4}, {&ABS, &LDX, 4}, {&ABS, &LAX, 4},
        /* BX */ {&REL, &BCS, 2}, {&IDY, &LDA, 5}, {&IMP, &KIL, 0}, {&IDY, &LAX, 5}, {&ZPX, &LDY, 4}, {&ZPX, &LDA, 4}, {&ZPY, &LDX, 4}, {&ZPY, &LAX, 4}, {&IMP, &CLV, 2}, {&ABY, &LDA, 4}, {&IMP, &TSX, 2}, {&ABY, &LAS, 4}, {&ABX, &LDY, 4}, {&ABX, &LDA, 4}, {&ABY, &LDX, 4}, {&ABY, &LAX, 4},
        /* CX */ {&IMM, &CPY, 2}, {&IDX, &CMP, 6}, {&IMM, &NOP, 2}, {&IDX, &DCP, 8}, {&ZP0, &CPY, 3}, {&ZP0, &CMP, 3}, {&ZP0, &DEC, 5}, {&ZP0, &DCP, 5}, {&IMP, &INY, 2}, {&IMM, &CMP, 2}, {&IMP, &DEX, 2}, {&IMM, &AXS, 2}, {&ABS, &CPY, 4}, {&ABS, &CMP, 4}, {&ABS, &DEC, 6}, {&ABS, &DCP, 6},
        /* DX */ {&REL, &BNE, 2}, {&IDY, &CMP, 5}, {&IMP, &KIL, 0}, {&IDY, &DCP, 8}, {&ZPX, &NOP, 4}, {&ZPX, &CMP, 4}, {&ZPX, &DEC, 6}, {&ZPX, &DCP, 6}, {&IMP, &CLD, 2}, {&ABY, &CMP, 4}, {&IMP, &NOP, 2}, {&ABY, &DCP, 7}, {&ABX, &NOP, 4}, {&ABX, &CMP, 4}, {&ABX, &DEC, 7}, {&ABX, &DCP, 7},
        /* EX */ {&IMM, &CPX, 2}, {&IDX, &SBC, 6}, {&IMM, &NOP, 2}, {&IDX, &ISC, 8}, {&ZP0, &CPX, 3}, {&ZP0, &SBC, 3}, {&ZP0, &INC, 5}, {&ZP0, &ISC, 5}, {&IMP, &INX, 2}, {&IMM, &SBC, 2}, {&IMP, &NOP, 2}, {&IMM, &SBC, 2}, {&ABS, &CPX, 4}, {&ABS, &SBC, 4}, {&ABS, &INC, 6}, {&ABS, &ISC, 6},
        /* FX */ {&REL, &BEQ, 2}, {&IDY, &SBC, 5}, {&IMP, &KIL, 0}, {&IDY, &ISC, 8}, {&ZPX, &NOP, 4}, {&ZPX, &SBC, 4}, {&ZPX, &INC, 6}, {&ZPX, &ISC, 6}, {&IMP, &SED, 2}, {&ABY, &SBC, 4}, {&IMP, &NOP, 2}, {&ABY, &ISC, 7}, {&ABX, &NOP, 4}, {&ABX, &SBC, 4}, {&ABX, &INC, 7}, {&ABX, &ISC, 7}
        }};
};

#endif // H_CPU