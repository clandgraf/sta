#pragma once

#include <cstdint>

#include "defs.hpp"

class Cart;
class Emu;

class PPU {
public:
    static uint8_t constexpr PPUCTRL = 0x0;
    static uint8_t constexpr PPUMASK = 0x1;
    static uint8_t constexpr PPUSTATUS = 0x2;
    static uint8_t constexpr OAMADDR = 0x3;
    static uint8_t constexpr OAMDATA = 0x4;
    static uint8_t constexpr PPUSCROLL = 0x5;
    static uint8_t constexpr PPUADDR = 0x6;
    static uint8_t constexpr PPUDATA = 0x7;

    PACK(
        union OamEntry {
            struct {
                uint8_t y;
                uint8_t tileIndex;
                union {
                    uint8_t attributes;
                    struct {
                        unsigned int palette : 2;
                        unsigned int __unused : 3;
                        unsigned int priority : 1;
                        unsigned int hflip : 1;
                        unsigned int vflip : 1;
                    };
                };
                uint8_t x;
            };
            uint8_t fields[4];
        };

        union Controller {
            struct {
                unsigned int baseNtAddress: 2;  // bit 0-1
                unsigned int vramIncrement: 1;  // bit 2
                unsigned int sprPatternTbl: 1;  // bit 3
                unsigned int bkgPatternTbl: 1;  // bit 4
                unsigned int sprSize: 1;        // bit 5
                unsigned int masterSlave: 1;    // bit 6
                unsigned int vblankNmi: 1;      // bit 7
            };
            uint8_t field;
        };
    )

    PPU(Emu& emu) : m_emu(emu) {}

    unsigned long getCycleCount() const { return m_cycleCount; }

    uint8_t read_register(uint8_t reg);
    void write_register(uint8_t reg, uint8_t value);
    
    void reset();
    void run(unsigned int cycles);

    uint16_t m_scanline = 261;
    uint16_t m_sl_cycle = 0;

    bool m_f_odd_frame = false;

    // PPUMASK Flags
    bool m_f_sprites_enable = false;
    bool m_f_background_enable = false;

    // PPUSTATUS Flags
    bool m_f_vblank = false;

private:
    Emu& m_emu;

    uint8_t readPPUSTATUS();

    unsigned long m_cycleCount = 0;

    OamEntry m_oam[64];
    OamEntry m_soam[8];

    bool m_ignoreWrites = true;

    bool m_r_addressLatch = false;

    Controller m_r_ppuctrl;

    uint16_t   m_r_t = 0x0000;
    uint16_t   m_r_v = 0x0000;
};
