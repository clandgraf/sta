#include <iostream>

#include "emu.hpp"
#include "ppu.hpp"
#include "util.hpp"
#include "rom.hpp"

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
        if (m_cycleCount >= WARMUP_CYCLES) { \
            m_ignoreWrites = false; \
        } else { \
            LOG_ERR << "Write ignored at cycle " << m_cycleCount << ": (" << sm::hex(reg) \
                    << ", " << sm::hex(value) \
                    << ")\n"; \
            return; \
        } \
    } \
}

void PPU::reset() {
    m_ignoreWrites = true;

    m_cycleCount = 0;
    m_scanline = 0;
    m_sl_cycle = 0;
    m_f_oddFrame = false;

    m_f_vblankNmi = 0;
    m_r_t.word.field = 0;
    m_r_mask.field = 0; 

    m_r_dataReadBuffer = 0x00;
    m_r_addressLatch = false;

    m_oamAddress = 0;
}

void PPU::run(unsigned int cycles) {
    uint8_t tmp8;
    for (unsigned int i = 0; i < cycles; i++) {
        
        // Filling shift registers for bkg rendering
        if (m_scanline < 240 || m_scanline == 261) {
            switch (m_sl_cycle) {
            //case 1: tmp8 = 
            }
        }

        // TODO Here be rendering
        
        // Update Status Flags and raise NMI
        if (m_scanline == 241 && m_sl_cycle == 1) {
            m_f_statusVblank = true;
            if (m_f_vblankNmi) {
                m_emu.m_nmi_request = true;
            }
        }

        if (m_scanline == 261 && m_sl_cycle == 1) {
            m_f_statusVblank = false;
            m_f_statusOverflow = false;
            m_f_statusSprZero = false;
        }

        // Update Counters for next scanline
        ++m_sl_cycle;
        bool skipTick = m_f_oddFrame 
                      && m_scanline == 261 
                      && (m_r_mask.field & RENDERING_ENABLED);
        bool nextScanline = m_sl_cycle >= (skipTick ? 340 : 341);
        // Next Scanline
        if (nextScanline) {
            m_sl_cycle = 0;
            m_scanline++;
            // Next Frame
            if (m_scanline >= 262) {
                m_scanline = 0;
                m_f_oddFrame = !m_f_oddFrame;
            }
        }
        
        m_cycleCount++;
    }
}

uint8_t PPU::readRegister(uint8_t reg) {
    switch (reg) {
    case PPUSTATUS: return readStatus();
    case PPUDATA:   return readData();
    case OAMDATA:   return m_oam.data[m_oamAddress];
    default:
        LOG_ERR << sm::hex(m_emu.getOpcodeAddress())
            << " PPU::read_register("
            << sm::hex(reg)
            << "): Not implemented\n";
        return 0;
    }
}

void PPU::writeRegister(uint8_t reg, uint8_t value) {
    m_r_status.field = value;

    switch (reg) {
    case PPUCTRL:   CHECK_WRITE; writeCtrl(value); break;
    case PPUSCROLL: CHECK_WRITE; writeScroll(value); break;
    case PPUADDR:   CHECK_WRITE; writeAddr(value); break;
    case PPUMASK:   CHECK_WRITE; m_r_mask.field = value; break;
    case PPUDATA:   writeData(value); break;
    case OAMADDR:   m_oamAddress = value; break;
    case OAMDATA:   m_oam.data[m_oamAddress++] = value; break;
    default:
        LOG_ERR << sm::hex(m_emu.getOpcodeAddress())
            << " PPU::write_register("
            << sm::hex(reg)
            << ", "
            << sm::hex(value)
            << "): Not implemented\n";
        break;
    }
}

void PPU::writeCtrl(CtrlV v) {
    m_f_vblankNmi = v.vblankNmi;
    m_f_sprSize = v.sprSize;
    m_f_master = v.masterSlave;
    m_bkgPatternTbl = v.bkgPatternTbl ? 0x1000 : 0x0000;
    m_sprPatternTbl = v.sprPatternTbl ? 0x1000 : 0x0000;
    m_r_addressIncrement = v.vramIncrement ? 32 : 1;
    m_r_t.baseNtAddress = v.baseNtAddress;
}

void PPU::writeScroll(ScrollV v) {
    if (m_r_addressLatch) {
        m_r_addressLatch = !m_r_addressLatch;
        m_r_t.coarseScrollY = v.coarseScroll;
        m_r_t.fineScrollY = v.fineScroll;
    } else {
        m_r_addressLatch = !m_r_addressLatch;
        m_r_x = v.fineScroll;
        m_r_t.coarseScrollX = v.coarseScroll;
    }
}

void PPU::writeAddr(uint8_t v) {
    if (m_r_addressLatch) {
        m_r_addressLatch = !m_r_addressLatch;
        m_r_t.word.lo = v;
    } else {
        m_r_addressLatch = !m_r_addressLatch;
        m_r_t.word.hi = v & 0x3f;
        m_r_v = m_r_t.word.field;
    }
}

void PPU::writeData(uint8_t v) {
    writeVram(m_r_v, v);
    m_r_v += m_r_addressIncrement;
}

uint8_t PPU::readData() {
    m_r_dataReadBuffer = readVram(m_r_v, true);
    return (
        m_r_v < 0x3f00 ? 
        m_r_dataReadBuffer : 
        readVram(m_r_v)
    );
}

uint8_t PPU::readStatus() {
    m_r_status.vblank   = m_f_statusVblank;
    m_r_status.overflow = m_f_statusOverflow;
    m_r_status.sprZero  = m_f_statusSprZero;

    m_f_statusVblank = false;
    m_r_addressLatch = false;

    return m_r_status.field;
}

uint8_t PPU::readVram(uint16_t address, bool ignorePalette) {
    if (address < 0x2000) {
        return m_cart->readb_ppu(address);
    } else if ((ignorePalette && address < 0x4000) || address < 0x3f00) {
        NameTableAddress a = address;
        return m_vram[m_cart->getNameTable(a.ntIndex) | a.ntAddress];
    } else if (address < 0x4000) {
        // Palette
    } else {
        LOG_ERR << "Illegal VRAM Read @ " << sm::hex(address) << "\n";
    }

    return 0;
}

void PPU::writeVram(uint16_t address, uint8_t value) {
    if (address < 0x2000) {
        m_cart->writeb_ppu(address, value);
    } else if (address < 0x3f00) {
        NameTableAddress a = address;
        m_vram[m_cart->getNameTable(a.ntIndex) | a.ntAddress] = value;
    } else if (address < 0x4000) {
        // Palette
    } else {
        LOG_ERR << "Illegal VRAM Write @ " << sm::hex(address) << "\n";
    }
}