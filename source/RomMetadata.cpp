#include "../headers/RomMetadata.h"
#include <cstdio>
#include <cstring>

RomMetadata::RomMetadata(uint8_t metadata[16]) {
    memcpy(_metadata, metadata, sizeof(metadata));
}

bool RomMetadata::isSupported() {
    return _metadata[0] == 0x4E && _metadata[1] == 0x45 && _metadata[2] == 0x53 && _metadata[3] == 0x1A;
}

bool RomMetadata::isNES2() {
    return (3 << 3) & _metadata[7] == (1 << 3);
}