#pragma once

#include <cstdint>

#include "defs.hpp"

class Cart;
class Emu;

class PPU {
private:
    static unsigned int constexpr WARMUP_CYCLES = 88974;

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

        union CtrlV {
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

            CtrlV(uint8_t value) : field(value) {}
        };

        union ScrollV {
            struct {
                unsigned int fineScroll: 3;
                unsigned int coarseScroll: 5;
            };
            uint8_t field;

            ScrollV(uint8_t value) : field(value) {}
        };

        union StatusV {
            struct {
                unsigned int __unused: 5;
                unsigned int overflow: 1;
                unsigned int sprZero: 1;
                unsigned int vblank: 1;
            };
            uint8_t field;
        };

        union T {
            struct {
                unsigned int coarseScrollX : 5;
                unsigned int coarseScrollY : 5;
                unsigned int baseNtAddress : 2;
                unsigned int fineScrollY : 3;
                unsigned int __unused: 1;
            };
            Word word;
        };
    )

    PPU(Emu& emu) : m_emu(emu) {}

    unsigned long getCycleCount() const { return m_cycleCount; }

    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t value);
    
    void reset();
    void run(unsigned int cycles);

    uint16_t m_scanline = 261;
    uint16_t m_sl_cycle = 0;

    __forceinline bool isOddFrame() { return m_f_oddFrame; }

    bool m_f_vblank_nmi = false;

    // PPUMASK Flags
    bool m_f_sprEnable = false;
    bool m_f_bkgEnable = false;

private:
    Emu& m_emu;

    uint8_t readStatus();
    void writeCtrl(CtrlV);
    void writeScroll(ScrollV);
    void writeAddr(uint8_t);

    unsigned long m_cycleCount = 0;

    OamEntry m_oam[64];
    OamEntry m_soam[8];

    bool m_ignoreWrites = true;  // true until the PPU is write-ready, after WARMUP_CYCLES cycles
    bool m_f_oddFrame  = false;  // indicates wether we are on an even or odd frame

    //// Status Register
    // This stores the value last written to a PPU register
    // We use this to simulate unset bits on read, see https://wiki.nesdev.com/w/index.php/PPU_registers#PPUSTATUS
    // On Read we update bits 5-7
    StatusV    m_r_status;
    bool       m_f_statusVblank = false;
    bool       m_f_statusOverflow = false;
    bool       m_f_statusSprZero = false;

    bool       m_r_addressLatch = false;
    uint8_t    m_r_addressIncrement = 1;
    T          m_r_t;
    uint16_t   m_r_v = 0x0000;
    uint8_t    m_r_x;
};
