#include <cstdint>

using std::int32_t;
using std::uint16_t;
using std::uint8_t;

/**
 * NAMETABLE LAYOUT
 * 
 * The nametables can be mapped in different ways to allow scrolling. A mapper can
 * hardwire this layout, switch between layouts or support an entirely different layout.
 * 
 * Reference: https://www.nesdev.org/wiki/NES_2.0#Nametable_layout
 */

enum NametableLayout : uint8_t {
    HORIZONTAL = 0x00,
    VERTICAL = 0x01,
    ALTERNATIVE = 0x02,
    FOUR = 0x03
};

/**
 * CONSOLE TYPE
 * 
 * The header also supports different kinds of console hardware, NES, Vs. System, 
 * Playchoice and extended console types. The Vs. System inturn also requires different 
 * hardware depending on the ROM.
 * 
 * Reference: https://www.nesdev.org/wiki/NES_2.0#File_Structure
 * Vs. System Reference: https://www.nesdev.org/wiki/NES_2.0#Vs._System_Type
 */

enum ConsoleType : uint8_t {
    NES = 0x00,
    VS_SYSTEM = 0x01,
    PLAYCHOICE = 0x02,
    EXTENDED = 0x03,
    UNSUPPORTED = 0xFF
};

/**
 * CONSOLE TIMING
 * 
 * Since the console was released in regions with different TV standards. This resulted
 * in the CPU and PPU having to run at different clock speeds depending on region.
 * 
 * Reference: https://www.nesdev.org/wiki/NES_2.0#CPU/PPU_Timing
 */

enum ConsoleTiming : uint8_t {
    NTSC = 0x00,
    PAL = 0x01,
    DENDY = 0x02,
    MULTIREGION = 0x03,
    UNSUPPORTED = 0xFF
};

/**
 * EXPANSION DEVICE
 * 
 * The console supported differnent types of input devices. The header can specify 
 * which one of these should be used when playing the game.
 * 
 * Reference: www.nesdev.org/wiki/NES_2.0#Default_Expansion_Device
 */

 // TODO: Add missing expansion devices.
enum ExpansionDevice : uint8_t {
    UNSPECIFIED = 0x00,
    STANDARD = 0x01,
    FOURSCORE = 0x02,
    FAMICOM_FOUR = 0x03,
    VS_4016 = 0x04,
    VS_4017 = 0x05,
    VS_ZAPPER = 0x07,
    ZAPPER_4017 = 0x08,
    ZAPPER_4016 = 0x49,
    TWO_ZAPPERS = 0x09,
    SNES_CONTROLLER = 0x2B,
    UNSUPPORTED = 0xFF
};