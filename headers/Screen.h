#ifndef H_SCREEN
#define H_SCREEN

#include <cstdint>
#include <array>

using std::uint8_t;

template <std::size_t W, std::size_t H>
class Screen {
    public:
        virtual void put(std::size_t x, std::size_t y, uint8_t r, uint8_t g, uint8_t b);
        virtual void put(std::size_t x, std::size_t y, std::array<uint8_t, 3> color);
        virtual std::array<uint8_t, 3> get(std::size_t x, std::size_t y);
        virtual void swap();
    protected:
        typedef struct {
            uint8_t r = 0x00;
            uint8_t g = 0x00;
            uint8_t b = 0x00;
        } RGB;
        typedef std::array<std::array<RGB, H>, W> Buffer;

        std::array<Buffer, 2> buffers;
};

#include "../source/Screen.tpp"

#endif // H_SCREEN