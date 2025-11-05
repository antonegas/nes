#ifndef T_SDLSCREEN
#define T_SDLSCREEN

#ifndef H_SDLSCREEN
#error __FILE__ should only be included from SDLScreen.h.
#endif // H_SDLSCREEN

#include <SDL3/SDL_render.h>

#include "Screen.h"
#include "SDL/SDLScreen.h"

template <std::size_t W, std::size_t H>
void SDLScreen<W, H>::draw(SDL_Renderer *renderer) {
    typename Screen<W, H>::Buffer buffer = SDLScreen::buffers[0];

    // Set to screen to black.
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Get renderer pixel width and height.
    int w;
    int h;
    SDL_GetRenderOutputSize(renderer, &w, &h);

    // Set scale factor.
    float const SCALE_X = w / (float)W;
    float const SCALE_Y = h / (float)H;
    float const SCALE = std::min(SCALE_X, SCALE_Y);
    SDL_SetRenderScale(renderer, SCALE, SCALE);

    // Calculate black bar offsets.
    float const OFFSET_X = (w / SCALE - W) / 2.0;
    float const OFFSET_Y = (h / SCALE - H) / 2.0;

    // Draw pixels.
    SDL_FRect pixel = {0.0, 0.0, 1.0, 1.0};
    typename Screen<W, H>::RGB color;

    for (std::size_t y = 0; y < H; y++) {
        for (std::size_t x = 0; x < W; x++) {
            // Set pixel position.
            pixel.x = x + OFFSET_X;
            pixel.y = y + OFFSET_Y;

            // Draw pixel.
            color = buffer[x][y];
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &pixel);
        }
    }

    SDL_RenderPresent(renderer);
}

#endif // T_SDLSCREEN