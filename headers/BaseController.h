#ifndef H_BASE_CONTROLLER
#define H_BASE_CONTROLLER

#include <cstdint>

using std::uint16_t;
using std::uint8_t;

class BaseController {
    public:
        /**
         * READ/RELOAD
         * 
         * A controller can be signaled to store the current state of a controller by setting 
         * a strobe high, which is done when the CPU writes the 0x4016 address. This state can 
         * then be read, for the standard controller this is done serially with each of the first 
         * eight reads giving a corresponding button state.
         * 
         * Reference: https://www.nesdev.org/wiki/Standard_controller
         */

        virtual uint8_t read() {};
        virtual void reload() {};

        /**
         * UPDATE STATE
         * 
         * Used to signal to the controllers that they should update their states.
         */

        virtual void update() {};
};

#endif // H_BASE_CONTROLLER