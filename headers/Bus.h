#ifndef H_BUS
#define H_BUS

#include <cstdint>

class Bus {
    public:
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
        void clock();
    
    private:
        uint8_t ram[2048];
};

#endif // H_BUS