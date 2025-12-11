#ifndef T_SCREEN
#define T_SCREEN

#ifndef H_SCREEN
#error __FILE__ should only be included from Screen.h.
#endif // H_SCREEN

#include <utility>
#include <algorithm>

#include "Screen.h"

template <std::size_t W, std::size_t H>
void Screen<W, H>::put(std::size_t x, std::size_t y, uint8_t r, uint8_t g, uint8_t b) {
    if (x < 0 || x >= W) return;
    if (y < 0 || y >= H) return;

    buffers[1][x + y * W].r = r;
    buffers[1][x + y * W].g = g;
    buffers[1][x + y * W].b = b;
}

template <std::size_t W, std::size_t H>
void Screen<W, H>::put(std::size_t x, std::size_t y, std::array<uint8_t, 3> color) {
    if (x < 0 || x >= W) return;
    if (y < 0 || y >= H) return;

    buffers[1][x + y * W].r = color[0];
    buffers[1][x + y * W].g = color[1];
    buffers[1][x + y * W].b = color[2];
}

template <std::size_t W, std::size_t H>
std::array<uint8_t, 3> Screen<W, H>::get(std::size_t x, std::size_t y) {
    std::array<uint8_t, 3> color{0, 0, 0};

    if (x < 0 || x >= W) return color;
    if (y < 0 || y >= H) return color;

    color[0] = buffers[1][x + y * W].r;
    color[1] = buffers[1][x + y * W].g;
    color[2] = buffers[1][x + y * W].b;

    return color;
}

template <std::size_t W, std::size_t H>
void Screen<W, H>::swap() {
    std::swap(buffers[0], buffers[1]);
}

#endif // T_SCREEN