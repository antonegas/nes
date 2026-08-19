#ifndef H_RAM
#define H_RAM

#include <cstdint>
#include <array>

using std::uint16_t;
using std::uint8_t;

template <uint16_t S>
class RAM {
    public:
        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t data);

    private:
        std::array<uint8_t, S> memory{};
};

#endif // H_RAM