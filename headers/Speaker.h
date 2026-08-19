#ifndef H_SPEAKER
#define H_SPEAKER

#include <cstdint>

using std::uint16_t;
using std::uint64_t;

class Speaker {
    public:
        virtual void put(uint32_t sample, uint64_t rate) {};
};

#endif // H_SPEAKER