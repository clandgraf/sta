#pragma once

#include <cstdint>

class Cart;
class Emu;

uint8_t constexpr PPUCTRL   = 0x0;
uint8_t constexpr PPUMASK   = 0x1;
uint8_t constexpr PPUSTATUS = 0x2;
uint8_t constexpr OAMADDR   = 0x3;
uint8_t constexpr OAMDATA   = 0x4;
uint8_t constexpr PPUSCROLL = 0x5;
uint8_t constexpr PPUADDR   = 0x6;
uint8_t constexpr PPUDATA   = 0x7;

class PPU {
public:
    PPU(Emu* emu, Cart* cart) : m_emu(emu), m_cart(cart) {}

    unsigned long getCycleCount() const { return m_cycleCount; }

    uint8_t read_register(uint8_t reg);
    void write_register(uint8_t reg, uint8_t value);
    
    void reset();
    void run(unsigned int cycles);

    uint16_t m_scanline = 261;
    uint16_t m_sl_cycle = 0;

    bool m_f_odd_frame = false;

    // PPUCTRL Flags
    bool m_f_vblank_nmi = false;

    // PPUMASK Flags
    bool m_f_sprites_enable = false;
    bool m_f_background_enable = false;

    // PPUSTATUS Flags
    bool m_f_vblank = false;

private:
    Emu* m_emu;
    Cart* m_cart;

    uint8_t readPPUSTATUS();

    void writePPUCTRL(uint8_t value);

    unsigned long m_cycleCount = 0;
};
