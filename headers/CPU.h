#ifndef H_CPU
#define H_CPU

#include <cstdint>

using std::uint16_t;
using std::uint8_t;

class Bus;

class CPU {
    public:
        void tick();
        void power();
        void reset();
        void irq();
        void nmi();
    private:
        Bus *bus = nullptr;

        /**
         * HELPER MEMBER/FUNCTIONS
         * 
         * Some helper functions for logic which is used repeatedly.
         */
        uint8_t wait = 0x00;
        bool oops = false;
        uint16_t (CPU::*addrMode)();
        void (CPU::*op)();

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
         * There are 7 different flags stored in a byte: carry (C), zero (Z), interupt disable (I), 
         * decimal (D), b flag (B), overflow (V) and negative (N). The 6:th bit of the byte is not  
         * used and is always set to one. The b flag is not set by the ALU.
         *
         * CPU registers reference: https://www.nesdev.org/wiki/CPU_registers
         * CPU status flags reference: https://www.nesdev.org/wiki/Status_flags
         */
        enum StatusFlag {
            C = 1 << 0,
            Z = 1 << 1,
            I = 1 << 2,
            D = 1 << 3,
            B = 1 << 4,
            U = 1 << 5,
            V = 1 << 6,
            N = 1 << 7
        };

        uint8_t a = 0x00; // Accumulator
        uint8_t x = 0x00; // X index
        uint8_t y = 0x00; // Y index
        uint16_t pc = 0x00; // Program counter (set by power/reset)
        uint8_t s = 0xFD; // Stack 
        uint8_t p = I | U; // Status register P: NV1BDIZC


        uint8_t getFlag(StatusFlag flag);
        void setFlag(StatusFlag flag, bool value);

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
        void XXX(); // Unimplemented illegal instruction

        // void AHX();
        // void ALR();
        // void ANC();
        // void ARR();
        // void AXS();
        // void DCP();
        // void ISC();
        // void JAM();
        // void LAS();
        // void LAX();
        // void RLA();
        // void RRA();
        // void SAX();
        // void SHX();
        // void SHY();
        // void SLO();
        // void SRE();
        // void TAS();
        // void XAA();

        // TODO: Add opcodes table.
};

#endif // H_CPU