#ifndef H_SCREEN
#define H_SCREEN

#include <cstdint>
#include <array>


using std::uint8_t;

template <std::size_t W, std::size_t H>
class Screen {
    public:
        virtual void put(std::size_t x, std::size_t y, uint8_t r, uint8_t g, uint8_t b) {};
        virtual void swap() {};
};

#endif // H_SCREEN