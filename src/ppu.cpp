#include <iostream>

#include "ppu.hpp"

void PPU::reset() {
    m_cycleCount = 0;
    m_scanline = 0;
    m_sl_cycle = 0;
    m_f_odd_frame = false;

    // Initialize PPUCTRL to 0
    m_f_vblank_nmi = false;
    
    // Initialize PPUMASK to 0
    m_f_sprites_enable = false;
    m_f_background_enable = false;
}

void PPU::run(unsigned int cycles) {
    for (unsigned int i = 0; i < cycles; i++) {
        // Here be rendering
        
        if (m_scanline == 241 && m_sl_cycle == 1) {
            m_f_vblank = true;
            // TODO invoke nmi if m_f_vblank_nmi is not set
        }

        ++m_sl_cycle;
        bool skipTick = (m_f_odd_frame && m_scanline == 261 && (m_f_background_enable || m_f_sprites_enable));
        bool nextScanline = m_sl_cycle >= (skipTick ? 340 : 341);
        // Next Scanline
        if (nextScanline) {
            m_sl_cycle = 0;
            m_scanline++;
            // Next Frame
            if (m_scanline >= 262) {
                m_scanline = 0;
                m_f_odd_frame = !m_f_odd_frame;
            }
        }
        
        m_cycleCount++;
    }
}

uint8_t PPU::read_register(uint8_t reg) {
    switch (reg) {
    case PPUSTATUS: return readPPUSTATUS();
    }

    std::cerr << "Not implemented\n";

    return 0;
}

void PPU::write_register(uint8_t reg, uint8_t value) {
    switch (reg) {
    case PPUCTRL: writePPUCTRL(value); break;
    }
}

uint8_t PPU::readPPUSTATUS() {
    uint8_t v = 0;
    if (m_f_vblank) { v |= 0b10000000; }
    // TODO add missing flags

    m_f_vblank = false;

    return v;
}

void PPU::writePPUCTRL(uint8_t value) {
    m_f_vblank_nmi = value & 0b10000000;
    // Add missing flags
}
