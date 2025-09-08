#ifndef H_CPU
#define H_CPU

#include <cstdint>

using std::uint16_t;
using std::uint8_t;

class Bus;

class CPU {
    public:
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
        void clock();
    private:
        Bus *bus = nullptr;

        /**
         * CPU REGISTERS
         *
         * The CPU has six different registers: accumulator (A), indexes (X and Y), program counter
         * (PC), stack pointer (S) and status flags (P).
         * 
         * There are 7 different flags stored in a byte: carry (C), zero (Z), interupt disable (I), 
         * decimal (D), b flag (B), overflow (V) and negative (N). The 6:th bit of the byte is not  
         * used and is always set to one. The b flag is not set by the ALU.
         *
         * CPU registers reference: https://www.nesdev.org/wiki/CPU_registers
         * CPU status flags reference: https://www.nesdev.org/wiki/Status_flags
         */
        uint8_t a; // Accumulator
        uint8_t x; // X index
        uint8_t y; // Y index
        uint16_t pc; // Program counter
        uint8_t s; // Stack
        uint8_t p; // Status register P: NV1BDIZC

        /**
         * CPU ADDRESSING MODES
         * 
         * The CPU has 12 different addressing modes.
         *
         * Reference: https://www.nesdev.org/wiki/CPU_addressing_modes
         */
        uint8_t ZPX(); // Zero page indexed by X
        uint8_t ZPY(); // Zero page indexed by Y
        uint8_t ABX(); // Absolute indexed by X
        uint8_t ABY(); // Absolute indexed by Y
        uint8_t INX(); // Indexed indirect by X
        uint8_t INY(); // Indirect Indexed by Y

        uint8_t ACC(); // Accumulator addressing
        uint8_t IMM(); // Immediate addressing
        uint8_t ZP0(); // Zero page addressing
        uint8_t ABS(); // Absolute adressing
        uint8_t REL(); // Relative addressing
        uint8_t IND(); // Indirect addressing

        /**
         * CPU INSTRUCTIONS
         * 
         * The CPU has 56 official instructions and 56 additional illegal opcodes.
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

        void XXX(); // Illegal instruction
};

#endif // H_CPU