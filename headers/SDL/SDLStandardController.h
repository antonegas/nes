#ifndef H_SDLSTANDARD_CONTROLLER
#define H_SDLSTANDARD_CONTROLLER

#include <cstdint>
#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_events.h>

#include "../StandardController.h"

using std::uint8_t;

class SDLStandardController : public StandardController {
    public:
        // TODO: Support custom mappings
        virtual void update(SDL_Event *event);
        SDL_JoystickID id = 0x0000; // Default as invalid.
};

#endif //H_SDLSTANDARD_CONTROLLER