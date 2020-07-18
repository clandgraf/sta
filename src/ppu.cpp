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

    m_bkgPatternTbl = 0;
    m_sprPatternTbl = 0;

    m_oamAddress = 0;
}

void PPU::run(unsigned int cycles) {
    auto incScrollX = [&]() {
        if (isRenderingEnabled()) {
            if (m_r_v.coarseScrollX == 31) {
                m_r_v.coarseScrollX = 0;
                m_r_v.baseNtX = ~m_r_v.baseNtX;
            } else {
                m_r_v.coarseScrollX++;
            }
        }
    };

    auto incScrollY = [&]() {
        if (isRenderingEnabled()) {
            if (m_r_v.fineScrollY < 7) {
                m_r_v.fineScrollY++;
            } else {
                m_r_v.fineScrollY = 0;
                switch (m_r_v.coarseScrollY) {
                case 29:
                    m_r_v.baseNtY = ~m_r_v.baseNtY;
                case 31:
                    m_r_v.coarseScrollY = 0;
                    break;
                default:
                    m_r_v.coarseScrollY++;
                    break;
                }
            }
        }
    };

    auto resScrollX = [&]() {
        if (isRenderingEnabled()) {
            m_r_v.coarseScrollX = m_r_t.coarseScrollX;
            m_r_v.baseNtX       = m_r_t.baseNtX;
        }
    };

    auto resScrollY = [&]() {
        if (isRenderingEnabled()) {
            m_r_v.fineScrollY   = m_r_t.fineScrollY;
            m_r_v.coarseScrollY = m_r_t.coarseScrollY;
            m_r_v.baseNtY       = m_r_t.baseNtY;
        }
    };
    
    auto loadShiftReg = [&](uint16_t& reg, uint8_t value) {
        reg &= 0xff00;
        reg |= value;  
    };

    auto loadShiftRegs = [&]() {
        loadShiftReg(m_shiftPatternLo, m_latch_tileLo);
        loadShiftReg(m_shiftPatternHi, m_latch_tileHi);

        loadShiftReg(m_shiftAttrLo, m_latch_atByte & 0b01 ? 0xff : 0x00);
        loadShiftReg(m_shiftAttrHi, m_latch_atByte & 0b10 ? 0xff : 0x00);
    };

    auto updateShiftRegs = [&]() {
        m_shiftPatternLo <<= 1;
        m_shiftPatternHi <<= 1;
        m_shiftAttrLo <<= 1;
        m_shiftAttrHi <<= 1;
    };

    for (unsigned int i = 0; i < cycles; i++) {
        
        if (m_scanline < 240 || m_scanline == 261) {

            // Filling latches for bkg rendering
        
            if ((m_sl_cycle > 0 && m_sl_cycle < 256)  // < 256 cuts of last case 7 (since this does incScrollY)
                || (m_sl_cycle > 320 && m_sl_cycle < 337)) {

                updateShiftRegs();

                // Regular Fetches
                switch ((m_sl_cycle - 1) % 8) {
                case 0: // Fetch Nametable Byte
                    loadShiftRegs();
                    m_latch_ntByte = readVram(0x2000 | (m_r_v.word.field & 0xfff)); 
                    break;  
                case 2: // Fetch Attribute Byte
                    m_latch_atByte = readVram(0x23c0 
                                            | (m_r_v.baseNtAddress << 10) 
                                            | ((m_r_v.coarseScrollY >> 2) << 3)
                                            | (m_r_v.coarseScrollX >> 2)); 
                    // Index into attribute byte
                    if (m_r_v.coarseScrollY & 0x02) m_latch_atByte >>= 4;
                    if (m_r_v.coarseScrollX & 0x02) m_latch_atByte >>= 2;
                    m_latch_atByte &= 0b11;
                    break;
                case 4: // Fetch Lo Tile Byte
                    m_latch_tileLo = readVram(m_bkgPatternTbl 
                                            + ((uint16_t)m_latch_ntByte << 4) 
                                            + m_r_v.fineScrollY 
                                            + 0);
                    break; 
                case 6: // Fetch Hi Tile Byte
                    m_latch_tileHi = readVram(m_bkgPatternTbl 
                                            + ((uint16_t)m_latch_ntByte << 4) 
                                            + m_r_v.fineScrollY 
                                            + 8);
                    break; 
                case 7: // Increase V horizontally
                    incScrollX(); 
                    break; 
                }
            }

            if (m_sl_cycle == 256) {
                incScrollY();
            }

            if (m_sl_cycle == 257) {
                loadShiftRegs();
                resScrollX();
            }

            if (m_sl_cycle == 338 || m_sl_cycle == 340) {
                m_latch_ntByte = readVram(0x2000 | (m_r_v.word.field & 0xfff));
            }
        }

        if (m_scanline == 261) {
            
            if (m_sl_cycle > 279 && m_sl_cycle < 305) {
                resScrollY();
            }

        }

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

        // Rendering a Pixel
        if (m_r_mask.bkgEnable && m_scanline >= 0 && m_scanline < 240) {
            if (m_sl_cycle > 0 && m_sl_cycle < 257) {
                uint16_t bit = 0x8000 >> m_r_x;
                
                uint8_t palIdx = ((m_shiftPatternLo & bit) ? 0b0001 : 0) 
                               | ((m_shiftPatternHi & bit) ? 0b0010 : 0)
                               | ((m_shiftAttrLo    & bit) ? 0b0100 : 0)
                               | ((m_shiftAttrHi    & bit) ? 0b1000 : 0);

                uint8_t value = readVram(palIdx);
            }
        }

        // Update Counters for next scanline
        if (++m_sl_cycle > 340) {
            m_sl_cycle = 0;
            
            bool skipTick = m_f_oddFrame
                         && m_scanline == 261
                         && isRenderingEnabled();
            if (skipTick) {
                m_sl_cycle++;
            }

            m_scanline++;
            // Next Frame
            if (m_scanline > 261) {
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
        m_r_v.word.field = m_r_t.word.field;
    }
}

void PPU::writeData(uint8_t v) {
    writeVram(m_r_v.word.field, v);
    m_r_v.word.field += m_r_addressIncrement;
}

uint8_t PPU::readData() {
    m_r_dataReadBuffer = readVram(m_r_v.word.field, true);
    return (
        m_r_v.word.field < 0x3f00 ? 
        m_r_dataReadBuffer : 
        readVram(m_r_v.word.field)
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
        address &= 0x001F;
        switch (address) {
        case 0x00: case 0x04: case 0x08: case 0x0C:
        case 0x10: case 0x14: case 0x18: case 0x1C:
            return m_palette[0x00];
        default:
            return m_palette[address];
        }
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
        address &= 0x001F;
        switch (address) {
        case 0x00: case 0x04: case 0x08: case 0x0C:
        case 0x10: case 0x14: case 0x18: case 0x1C:
            m_palette[0x00] = value;
            break;
        default:
            m_palette[address] = value;
            break;
        }
    } else {
        LOG_ERR << "Illegal VRAM Write @ " << sm::hex(address) << "\n";
    }
}