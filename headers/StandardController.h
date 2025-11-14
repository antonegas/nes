#ifndef H_STANDARD_CONTROLLER
#define H_STANDARD_CONTROLLER

#include <cstdint>

#include "BaseController.h"

using std::uint8_t;

class StandardController : public BaseController {
    public:
        virtual uint8_t read() override;
        virtual void reload() override;
    protected:
        union State {
            struct {
                bool a : 1;
                bool b : 1;
                bool select : 1;
                bool start : 1;
                bool up : 1;
                bool down : 1;
                bool left : 1;
                bool right : 1;
            };
            uint8_t data = 0x00;
        } state;
    private:
        uint8_t buffer = 0x00;
        uint8_t remaining = 0x00;
};

#endif // H_STANDARD_CONTROLLER