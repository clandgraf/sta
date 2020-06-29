#pragma once

#include <cstdint>

#include "defs.hpp"

class Cart;
class Emu;

class PPU {
private:
    static unsigned int constexpr WARMUP_CYCLES = 88974;
    static uint8_t constexpr RENDERING_ENABLED = 0b00011000;

public:
    static uint8_t constexpr PPUCTRL = 0x0;
    static uint8_t constexpr PPUMASK = 0x1;
    static uint8_t constexpr PPUSTATUS = 0x2;
    static uint8_t constexpr OAMADDR = 0x3;
    static uint8_t constexpr OAMDATA = 0x4;
    static uint8_t constexpr PPUSCROLL = 0x5;
    static uint8_t constexpr PPUADDR = 0x6;
    static uint8_t constexpr PPUDATA = 0x7;

    PACK(union OamEntry {
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
    });

    PACK(union CtrlV {
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
    });

    PACK(union MaskV {
        struct {
            unsigned int greyscale: 1;
            unsigned int bkgEnableLeft: 1;
            unsigned int sprEnableLeft: 1;
            unsigned int bkgEnable: 1;
            unsigned int sprEnable: 1;
            unsigned int emphRed: 1;
            unsigned int emphGreen: 1;
            unsigned int emphBlue: 1;
        };
        uint8_t field;

        MaskV(uint8_t value) : field(value) {}
    });

    PACK(union ScrollV {
        struct {
            unsigned int fineScroll: 3;
            unsigned int coarseScroll: 5;
        };
        uint8_t field;

        ScrollV(uint8_t value) : field(value) {}
    });

    PACK(union StatusV {
        struct {
            unsigned int __unused: 5;
            unsigned int overflow: 1;
            unsigned int sprZero: 1;
            unsigned int vblank: 1;
        };
        uint8_t          field;
    });

    PACK(union NameTableAddress {
        struct {
            unsigned int ntAddress: 10;
            unsigned int ntIndex:    2;
            unsigned int __unused:   4;
        };
        uint16_t         field;

        NameTableAddress(uint16_t value) : field(value) {}
    });

    PACK(union T {
        struct {
            unsigned int coarseScrollX : 5;
            unsigned int coarseScrollY : 5;
            unsigned int baseNtAddress : 2;
            unsigned int fineScrollY : 3;
            unsigned int __unused: 1;
        };
        Word word;
    });

    PPU(Emu& emu, std::shared_ptr<Cart> cart) : m_emu(emu), m_cart(cart) {}

    unsigned long getCycleCount() const { return m_cycleCount; }

    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t value);
    
    void reset();
    void run(unsigned int cycles);

    uint16_t m_scanline = 261;
    uint16_t m_sl_cycle = 0;

    __forceinline bool isOddFrame() { return m_f_oddFrame; }

private:
    Emu& m_emu;

    std::shared_ptr<Cart> m_cart;

    uint8_t readVram(uint16_t address, bool ignorePalette = false);
    void writeVram(uint16_t address, uint8_t value);

    uint8_t readStatus();
    uint8_t readData();
    void writeCtrl(CtrlV);
    void writeScroll(ScrollV);
    void writeAddr(uint8_t);
    void writeData(uint8_t);

    unsigned long m_cycleCount = 0;

    uint8_t m_oamAddress;
    union {
        OamEntry sprites[64];
        uint8_t data[256];
    } m_oam;
    
    union {
        OamEntry sprites[8];
        uint8_t data[32];
    } m_soam;

    bool m_ignoreWrites = true;  // true until the PPU is write-ready, after WARMUP_CYCLES cycles
    bool m_f_oddFrame  = false;  // indicates wether we are on an even or odd frame

    bool m_f_vblankNmi = false;

    MaskV      m_r_mask = 0;

    //// Data Read Buffer
    // When PPUDATA two things happen:
    // 1. A Read Buffer is refreshed. This ignores Palette Data and updates the mirrored nametable part
    // 2. a) If palette memory -> is accessed return memory directly
    //    b) Else              -> return Read Buffer
    uint8_t    m_r_dataReadBuffer = 0;

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

    uint8_t    m_vram[0x0800];
};
