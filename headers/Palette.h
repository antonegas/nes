#ifndef H_PALETTE
#define H_PALETTE


#include <cstdint>
#include <vector>

using std::uint16_t;
using std::uint8_t;

class Palette {
    public:
        Palette() = default;
        Palette(std::vector<uint8_t> data) : data{data} {};

        uint8_t getR(uint16_t entry);
        uint8_t getG(uint16_t entry);
        uint8_t getB(uint16_t entry);
        void setEmphasis(uint16_t emphasis);
        void setEmphasis(bool r, bool g, bool b);
    private:
        uint16_t emphasis = 0x00;
        std::vector<uint8_t> data;
};

#endif // H_PALETTE