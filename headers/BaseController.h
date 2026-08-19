#ifndef H_BASE_CONTROLLER
#define H_BASE_CONTROLLER

#include <cstdint>

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
        // NOTE: The controller port has seven pins 2 output (OUT and CLK), 3 output (D0, D3 and D4) a 5V and GND.
        // NOTE: CLK is a falling edge which is set low when a read is done on the port.
        // NOTE: OUT is the D0 bit from the write bus.
        // TODO: Update documentation (https://www.nesdev.org/wiki/Controller_port_pinout) is a better reference.

        virtual uint8_t read(uint16_t) { return clk() & 0x19; }
        virtual void write(uint16_t, uint8_t data) { if (data & 0x01) out(); }
    private:
        virtual void out() {}
        virtual uint8_t clk() { return 0x00; }
};

#endif // H_BASE_CONTROLLER