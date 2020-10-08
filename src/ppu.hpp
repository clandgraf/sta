#pragma once

#include <cstdint>
#include <functional>

#include "defs.hpp"

typedef void(*SetPixelFn)(unsigned int x, unsigned int y, unsigned int v);

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

    PACK(union OamAttributes {
        uint8_t field;
        struct {
            uint8_t palette : 2;
            uint8_t __unused : 3;
            uint8_t priority : 1;
            uint8_t hflip : 1;
            uint8_t vflip : 1;
        };
    });

    PACK(union OamEntry {
        struct {
            uint8_t y;
            uint8_t tileIndex;
            OamAttributes attributes;
            uint8_t x;
        };
        uint8_t fields[4];
    });

    PACK(union CtrlV {
        struct {
            uint8_t baseNtX       : 1;
            uint8_t baseNtY       : 1;
            uint8_t vramIncrement : 1;
            uint8_t sprPatternTbl : 1;
            uint8_t bkgPatternTbl : 1;
            uint8_t sprSize       : 1;
            uint8_t masterSlave   : 1;
            uint8_t vblankNmi     : 1;
        };
        uint8_t field;

        CtrlV() : field(0) {}
        CtrlV(uint8_t value) : field(value) {}
    });

    PACK(union MaskV {
        struct {
            uint8_t greyscale: 1;
            uint8_t bkgEnableLeft: 1;
            uint8_t sprEnableLeft: 1;
            uint8_t bkgEnable: 1;
            uint8_t sprEnable: 1;
            uint8_t emphRed: 1;
            uint8_t emphGreen: 1;
            uint8_t emphBlue: 1;
        };
        uint8_t field;

        MaskV() : field(0) {}
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
            uint8_t __unused: 5;
            uint8_t overflow: 1;
            uint8_t sprZero:  1;
            uint8_t vblank:   1;
        };
        uint8_t     field;
    });

    PACK(union NameTableAddress {
        struct {
            uint16_t ntAddress: 10;
            uint16_t ntIndex:    2;
            uint16_t __unused:   4;
        };
        uint16_t     field;

        NameTableAddress() : field(0) {}
        NameTableAddress(uint16_t value) : field(value) {}
    });

    PACK(union T {
        uint16_t word;

        struct {
            uint16_t coarseScrollX : 5;
            uint16_t coarseScrollY : 5;
            uint16_t baseNtX : 1;
            uint16_t baseNtY : 1;
            uint16_t fineScrollY : 3;
            uint16_t __unused : 1;
        };

        struct {
            uint16_t lo : 8;
            uint16_t hi : 8;
        };

        T() : word(0) {}
        T(uint16_t v) : word(v) {}
    });

    PPU(Emu& emu, std::shared_ptr<Cart> cart) : m_emu(emu), m_cart(cart) {}

    void setPixelFn(std::function<void(unsigned int, unsigned int, unsigned int)>);

    unsigned long getCycleCount() const { return m_cycleCount; }

    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t value);
    
    void reset();
    void run(unsigned int cycles);
    void cycle();

    uint16_t m_scanline = 261;
    uint16_t m_sl_cycle = 0;

    __forceinline bool isOddFrame() { return m_f_oddFrame; }

    const OamEntry* getSprites() { return m_oam.sprites; }

private:
    
    __forceinline bool isRenderingEnabled() { return m_r_mask.field & RENDERING_ENABLED; }

    Emu& m_emu;

    std::function<void(unsigned int, unsigned int, unsigned int)> m_setPixel;

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

    uint16_t m_oamPtr;
    uint8_t m_oamAddrExt;
    uint8_t m_oamAddrInt;

    union {
        OamEntry sprites[0x48];
        uint8_t data[0x121];
    } m_oam;
    
    bool m_ignoreWrites = true;  // true until the PPU is write-ready, after WARMUP_CYCLES cycles
    bool m_f_oddFrame  = false;  // indicates wether we are on an even or odd frame

    bool m_f_vblankNmi = false;

    MaskV      m_r_mask = 0;

    uint16_t m_sprPatternTbl;
    uint16_t m_bkgPatternTbl;
    bool m_f_sprSize;
    bool m_f_master;

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
    T          m_r_v;
    uint8_t    m_r_x;

    uint8_t    m_vram[0x0800];
    uint8_t    m_palette[0x20];

    // Rendering Background
    
    uint8_t    m_latch_ntByte;
    uint8_t    m_latch_atByte;
    uint8_t    m_latch_tileLo;
    uint8_t    m_latch_tileHi;

    uint16_t   m_shiftPatternHi;
    uint16_t   m_shiftPatternLo;
    uint16_t   m_shiftAttrHi;
    uint16_t   m_shiftAttrLo;

    // Rendering Sprites
        
    uint8_t m_sprTmp;  // temporary for copying between primary and secondary OAM

    bool m_sprZeroOnSl;

    uint8_t m_sprTileLo[8];
    uint8_t m_sprTileHi[8];
    uint8_t m_sprCounter[8];
    OamAttributes m_sprAttributes[8];
};
