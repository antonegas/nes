#ifndef T_SCREEN
#define T_SCREEN

#ifndef H_SCREEN
#error __FILE__ should only be included from Screen.h.
#endif // H_SCREEN

#include <utility>
#include <algorithm>

#include "../headers/Screen.h"

template <std::size_t W, std::size_t H>
void Screen<W, H>::put(std::size_t x, std::size_t y, uint8_t r, uint8_t g, uint8_t b) {
    if (x < 0 || x >= W) return;
    if (y < 0 || y >= H) return;

    buffers[1][x][y].r = r;
    buffers[1][x][y].g = g;
    buffers[1][x][y].b = b;
}

template <std::size_t W, std::size_t H>
void Screen<W, H>::swap() {
    std::swap(buffers[0], buffers[1]);
}

#endif // T_SCREEN