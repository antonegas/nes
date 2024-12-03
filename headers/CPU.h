#ifndef H_CPU
#define H_CPU

#include <cstdint>

class CPU {
    public:
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
        void clock();
    private:
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

        // Status register P
        bool c : 1; // Carry flag
        bool z : 1; // Zero flag
        bool i : 1; // Interupt disable flag
        bool d : 1; // Decimal flag
        bool b : 1; // B flag
        bool u : 1; // Unused and always true
        bool v : 1; // Overflow flag
        bool n : 1; // Negative flag
};

#endif // H_CPU