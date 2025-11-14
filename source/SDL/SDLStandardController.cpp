#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_events.h>

#include "SDL/SDLStandardController.h"

void SDLStandardController::update(SDL_GamepadButtonEvent *event) {
    // TODO: Maybe support joystick.
    // Skip events which are not gamepad button events.

    // Don't use the wrong gamepad to get state.
    if (event->which != id) return;

    // Update correct state bit.
    switch (event->button) {
        // case SDL_GAMEPAD_BUTTON_NORTH:
        case SDL_GAMEPAD_BUTTON_EAST:
            state.a = event->down;
            break;
        // case SDL_GAMEPAD_BUTTON_WEST:
        case SDL_GAMEPAD_BUTTON_SOUTH:
            state.b = event->down;
            break;
        case SDL_GAMEPAD_BUTTON_BACK:
            state.select = event->down;
            break;
        case SDL_GAMEPAD_BUTTON_START:
            state.start = event->down;
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_UP:
            state.up = event->down;
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
            state.down = event->down;
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
            state.left = event->down;
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
            state.right = event->down;
            break;
    }
}