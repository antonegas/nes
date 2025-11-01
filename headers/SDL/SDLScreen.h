#ifndef H_SDLSCREEN
#define H_SDLSCREEN

#include <cstdint>
#include <array>
#include <SDL3/SDL_render.h>

#include "../Screen.h"

using std::uint8_t;

template <std::size_t W, std::size_t H>
class SDLScreen : public Screen<W, H> {
    public:
        void draw(SDL_Renderer *renderer);
};

#include "../../source/SDL/SDLScreen.tpp"

#endif // H_SDLSCREEN