#include <iostream>

#include "emu.hpp"
#include "ppu.hpp"
#include "util.hpp"

/*
 * Notes:
 * - According to https://wiki.nesdev.com/w/index.php/NMI#Operation
 *   multiple NMIs can be triggered, during vblank, this does not
 *   work in here, as we can not detect toggling of nmi.
 *   Instead we should check 
 */

namespace sm = StreamManipulators;

// Ignore writes to sev. registers till ~29658 CPU cycles = 88974 PPU cycles. 
// See https://wiki.nesdev.com/w/index.php/PPU_power_up_state
#define CHECK_WRITE { \
    if (m_ignoreWrites) { \
        if (m_cycleCount >= 88974) { \
            m_ignoreWrites = false; \
        } else { \
            LOG_ERR << "Write ignored at cycle " << m_cycleCount; \
            return; \
        } \
    } \
}

void PPU::reset() {
    m_ignoreWrites = true;

    m_cycleCount = 0;
    m_scanline = 0;
    m_sl_cycle = 0;
    m_f_odd_frame = false;

    m_r_ppuctrl.field = 0;
    
    // Initialize PPUMASK to 0
    m_f_sprites_enable = false;
    m_f_background_enable = false;

    m_r_addressLatch = false;
}

void PPU::run(unsigned int cycles) {
    for (unsigned int i = 0; i < cycles; i++) {
        
        // Here be rendering
        
        if (m_scanline == 241 && m_sl_cycle == 1) {
            m_f_vblank = true;
            if (m_r_ppuctrl.vblankNmi) {
                m_emu.m_nmi_request = true;
            }
        }

        if (m_scanline == 261 && m_sl_cycle == 1) {
            m_f_vblank = false;
            // TODO clear sprite 0
            // TODO clear overflow
        }

        // Update Counters for next scanline
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

    LOG_ERR << sm::hex(m_emu.getOpcodeAddress())
            << " PPU::read_register(" 
            << sm::hex(reg) 
            << "): Not implemented\n";

    return 0;
}

void PPU::write_register(uint8_t reg, uint8_t value) {
    switch (reg) {
    case PPUCTRL: CHECK_WRITE; m_r_ppuctrl.field = value; break;
    }

    LOG_ERR << sm::hex(m_emu.getOpcodeAddress())
            << " PPU::write_register(" 
            << sm::hex(reg)
            << ", " 
            << sm::hex(value) 
            << "): Not implemented\n";
}

uint8_t PPU::readPPUSTATUS() {
    uint8_t v = 0;
    if (m_f_vblank) { v |= 0b10000000; }
    // TODO add missing flags

    m_f_vblank = false;
    m_r_addressLatch = false;

    return v;
}
