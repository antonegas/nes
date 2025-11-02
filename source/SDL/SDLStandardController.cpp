#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_events.h>

#include "../../headers/SDL/SDLStandardController.h"

void SDLStandardController::update(SDL_Event *event) {
    // TODO: Maybe support joystick.
    // Skip events which are not gamepad button events.
    if (
        event->type != SDL_EVENT_GAMEPAD_BUTTON_DOWN && 
        event->type != SDL_EVENT_GAMEPAD_BUTTON_UP
    ) return;
    
    SDL_GamepadButtonEvent *buttonEvent = (SDL_GamepadButtonEvent*)event;

    // Don't use wrong gamepad to get state.
    if (buttonEvent->which != id) return;

    // Update correct state bit.
    switch (buttonEvent->button) {
        case SDL_GAMEPAD_BUTTON_NORTH:
        case SDL_GAMEPAD_BUTTON_WEST:
            state.a = buttonEvent->down;
            break;
        case SDL_GAMEPAD_BUTTON_SOUTH:
        case SDL_GAMEPAD_BUTTON_EAST:
            state.b = buttonEvent->down;
            break;
        case SDL_GAMEPAD_BUTTON_BACK:
            state.select = buttonEvent->down;
            break;
        case SDL_GAMEPAD_BUTTON_START:
            state.start = buttonEvent->down;
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_UP:
            state.up = buttonEvent->down;
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
            state.down = buttonEvent->down;
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
            state.left = buttonEvent->down;
            break;
        case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
            state.right = buttonEvent->down;
            break;
    }
}