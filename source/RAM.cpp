#include <cstdint>
#include <cassert>

#include "RAM.h"

using std::uint16_t;
using std::uint8_t;

template <uint16_t S>
uint8_t RAM<S>::read(uint16_t addr) {
    assert(addr < S);
    return memory[addr];
}

template <uint16_t S>
void RAM<S>::write(uint16_t addr, uint8_t data) {
    assert(addr < S);
    memory[addr] = data;
}
