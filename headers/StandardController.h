#ifndef H_STANDARD_CONTROLLER
#define H_STANDARD_CONTROLLER

#include "BaseController.h"

class StandardController : BaseController {
    public:
        virtual uint8_t read() override;
        virtual void reload() override;
    private:
        uint8_t state = 0x00;
        uint8_t buffer = 0x00;
        uint8_t remaining = 0x00;
};

#endif // H_STANDARD_CONTROLLER