#include <iostream>

#include "core/util.hpp"
#include "emu.hpp"
#include "ppu.hpp"
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

void PPU::setPixelFn(std::function<void(unsigned int, unsigned int, unsigned int)> fn) {
    m_setPixel = fn;
}

void PPU::reset() {
    m_ignoreWrites = true;

    m_cycleCount = 0;
    m_scanline = 0;
    m_sl_cycle = 0;
    m_f_oddFrame = false;

    m_f_vblankNmi = 0;
    m_r_t.word = 0;
    m_r_mask.field = 0; 

    m_r_dataReadBuffer = 0x00;
    m_r_addressLatch = false;

    m_bkgPatternTbl = 0;
    m_sprPatternTbl = 0;

    m_oamAddrExt = 0;
    m_oamAddrInt = 0;

    m_oam.data[0x120] = 0xff;
}

enum SpriteEvalState {
    // Copy and check Sprite's Y
    SPR_EVAL_COPY_Y = 0,
    // ... If in Range, copy rest of sprite's bytes
    SPR_EVAL_COPY_REST = 1,
    // If OAM is full (8 sprites found on scanline)
    SPR_EVAL_FULL = 2,
    // If OAM is overflown (9th sprite found on scanline)
    SPR_EVAL_OVERFLOW = 3,
    // If all 64 sprites have been found
    SPR_EVAL_DONE = 4,
};

void PPU::cycle() {
    if (!isRenderingEnabled()) {
        return;
    }

    auto incScrollX = [&]() {
        if (isRenderingEnabled()) {
            if (m_r_v.coarseScrollX == 31) {
                m_r_v.coarseScrollX = 0;
                m_r_v.baseNtX = ~m_r_v.baseNtX;
            }
            else {
                m_r_v.coarseScrollX++;
            }
        }
    };

    auto incScrollY = [&]() {
        if (isRenderingEnabled()) {
            if (m_r_v.fineScrollY < 7) {
                m_r_v.fineScrollY++;
            }
            else {
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
            m_r_v.baseNtX = m_r_t.baseNtX;
        }
    };

    auto resScrollY = [&]() {
        if (isRenderingEnabled()) {
            m_r_v.fineScrollY = m_r_t.fineScrollY;
            m_r_v.coarseScrollY = m_r_t.coarseScrollY;
            m_r_v.baseNtY = m_r_t.baseNtY;
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

    auto sprOnScanline = [&](int spriteY) {
        return (
            m_scanline >= (spriteY) &&
            m_scanline <= (spriteY + (m_f_sprSize ? 0xf : 0x7))
            );
    };

    // ----------- Setting up OAM ------------
    // https://wiki.nesdev.com/w/index.php/PPU_sprite_evaluation#Details
    if (m_scanline < 240) {
        // New Scanline:
        // - Reset both internal and external OAM Pointers
        // - Reset sprite 0 on sl indicator
        if (m_sl_cycle == 0) {
            m_sprZeroOnSl = false;

            m_oamPtr = 0;
            m_oamAddrInt = 0;
            m_oamAddrExt = 0;
        }
        // Clear Secondary OAM
        else if (m_sl_cycle > 0 && m_sl_cycle <= 64) {
            if (m_sl_cycle & 1) {
                m_sprTmp = m_oam.data[m_oamPtr = 0x120];
            }
            else {
                m_oam.data[0x100 | m_oamAddrInt] = m_sprTmp;
                m_oamAddrInt = (m_oamAddrInt + 1) & 0x1f;
            }
        }
        // Sprite Evaluation
        else if (m_sl_cycle <= 256) {
            static int sprEvalState;
            if (m_sl_cycle == 65) {
                sprEvalState = SPR_EVAL_COPY_Y;
            }

            switch (sprEvalState) {
            case SPR_EVAL_COPY_Y:
                if (m_sl_cycle & 1) {
                    m_sprTmp = m_oam.data[m_oamPtr = m_oamAddrExt];
                }
                else {
                    m_oam.data[m_oamPtr = (0x100 | m_oamAddrInt)] = m_sprTmp;
                    if (sprOnScanline(m_sprTmp)) {
                        sprEvalState = SPR_EVAL_COPY_REST;
                        if (m_oamAddrExt == 0) {
                            // Sprite Zero is rendered on this scanline
                            m_sprZeroOnSl = true;
                        }
                        m_oamAddrExt += 1; m_oamAddrExt &= 0xff;
                        m_oamAddrInt += 1; m_oamAddrInt &= 0x1f;
                    }
                    else {
                        m_oamAddrExt += 4; m_oamAddrExt &= 0xff;
                        if (m_oamAddrExt == 0) {  // Check if we wrapped to 0 (all 64 sprites done)
                            sprEvalState = SPR_EVAL_DONE;
                        }
                    }
                }
                break;
            case SPR_EVAL_COPY_REST:
                if (m_sl_cycle & 1) {
                    m_sprTmp = m_oam.data[m_oamPtr = m_oamAddrExt];
                }
                else {
                    m_oam.data[m_oamPtr = (0x100 | m_oamAddrInt)] = m_sprTmp;
                    m_oamAddrExt += 1; m_oamAddrExt &= 0xff;
                    m_oamAddrInt += 1; m_oamAddrInt &= 0x1f;
                    if (m_oamAddrExt == 0) {  // Check if we wrapped to 0 (all 64 sprites done)
                        sprEvalState = SPR_EVAL_DONE;
                    }
                    else if ((m_oamAddrExt & 0x3) == 0) {  // ... or we're starting the next sprite
                        if (m_oamAddrInt == 0) { // ... but secondary oam is full
                            sprEvalState = SPR_EVAL_FULL;
                        }
                        else {
                            sprEvalState = SPR_EVAL_COPY_Y;
                        }
                    }
                }
                break;
            case SPR_EVAL_FULL:
                break;
            case SPR_EVAL_OVERFLOW:
                break;
            case SPR_EVAL_DONE:
                break;
            }
        }
    }

    // ------------- Tile Fetching -----------
    if (m_scanline < 240 || m_scanline == 261) {

        // Filling latches for bkg rendering

        if ((m_sl_cycle > 0 && m_sl_cycle < 337)) {

            static bool doSprites;
            static uint16_t sprIndex;

            if (doSprites = (m_sl_cycle > 256 && m_sl_cycle <= 320)) {
                sprIndex = (m_sl_cycle - 257) / 8;
            }

            updateShiftRegs();

            // Regular Fetches
            switch ((m_sl_cycle - 1) % 8) {
            case 0: // Fetch Nametable Byte
                loadShiftRegs();
                m_latch_ntByte = readVram(0x2000 | (m_r_v.word & 0xfff));
                break;
            case 2: // Fetch BG Attribute Byte / Fetch Sprite Attribute Byte
                if (doSprites) {
                    m_sprAttributes[sprIndex] = m_oam.sprites[64 + sprIndex].attributes;
                }

                m_latch_atByte = readVram(0x23c0
                    | (m_r_v.baseNtY << 11)
                    | (m_r_v.baseNtX << 10)
                    | ((m_r_v.coarseScrollY >> 2) << 3)
                    | (m_r_v.coarseScrollX >> 2));
                // Index into attribute byte
                if (m_r_v.coarseScrollY & 0x02) m_latch_atByte >>= 4;
                if (m_r_v.coarseScrollX & 0x02) m_latch_atByte >>= 2;
                m_latch_atByte &= 0b11;
                break;
            case 3: // Fetch Sprite X Coordinate
                if (doSprites) {
                    m_sprCounter[sprIndex] = m_oam.sprites[64 + sprIndex].x;
                }
                break;
            case 4: // Fetch BG / Sprite Lo Tile Byte
                if (doSprites) {
                    // TODO Support for 8x16 Sprites
                    if (m_oam.sprites[64 + sprIndex].attributes.field == 0xff) {
                        // If tile == 0xff render transparently
                        m_sprTileLo[sprIndex] = 0;
                    }
                    else {
                        unsigned int tile = m_oam.sprites[64 + sprIndex].tileIndex;
                        unsigned int offset = m_sprAttributes[sprIndex].vflip ? 
                            (m_oam.sprites[64 + sprIndex].y + 7 - m_scanline) :
                            (m_scanline - m_oam.sprites[64 + sprIndex].y);
                        m_sprTileLo[sprIndex] = readVram(m_sprPatternTbl
                            + ((uint16_t)tile << 4)
                            + offset
                            + 0);
                    }
                }
                else {
                    m_latch_tileLo = readVram(m_bkgPatternTbl
                        + ((uint16_t)m_latch_ntByte << 4)
                        + m_r_v.fineScrollY
                        + 0);
                }
                break;
            case 6: // Fetch BG / Sprite Hi Tile Byte
                if (doSprites) {
                    // TODO Support for 8x16 Sprites
                    if (m_oam.sprites[64 + sprIndex].attributes.field == 0xff) {
                        // If tile == 0xff render transparently
                        m_sprTileHi[sprIndex] = 0;
                    } else {
                        unsigned int tile = m_oam.sprites[64 + sprIndex].tileIndex;
                        unsigned int offset = m_sprAttributes[sprIndex].vflip ?
                            (m_oam.sprites[64 + sprIndex].y + 7 - m_scanline) : 
                            (m_scanline - m_oam.sprites[64 + sprIndex].y);
                        m_sprTileHi[sprIndex] = readVram(m_sprPatternTbl
                            + ((uint16_t)tile << 4)
                            + offset
                            + 8);
                    }
                }
                else {
                    m_latch_tileHi = readVram(m_bkgPatternTbl
                        + ((uint16_t)m_latch_ntByte << 4)
                        + m_r_v.fineScrollY
                        + 8);
                }
                break;
            case 7: // Increase V horizontally
                if (!doSprites) {
                    incScrollX();
                }
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
            m_latch_ntByte = readVram(0x2000 | (m_r_v.word & 0xfff));
        }
    }

    // ------------- Reset V --------------------
    if (m_scanline == 261) {

        if (m_sl_cycle > 279 && m_sl_cycle < 305) {
            resScrollY();
        }

    }
}

uint8_t BG_LUT[] = {
    0x00, 0x01, 0x02, 0x03,
    0x00, 0x05, 0x06, 0x07,
    0x00, 0x09, 0x0a, 0x0b,
    0x00, 0x0d, 0x0e, 0x0f,
};

uint8_t FG_LUT[] = {
    0x00, 0x11, 0x12, 0x13,
    0x00, 0x15, 0x16, 0x17,
    0x00, 0x19, 0x1a, 0x1b,
    0x00, 0x1d, 0x1e, 0x1f,
};

void PPU::run(unsigned int cycles) {
    for (unsigned int i = 0; i < cycles; i++) {
        // Prepare everything for rendering a pixel
        cycle();

        // ----- Update Status Flags and raise NMI ---
        if (m_scanline == 241 && m_sl_cycle == 1) {
            m_f_statusVblank = true;
            if (m_f_vblankNmi) {
                m_emu.m_nmi_request = true;
            }
        }

        // ----------- Reset Status Flags ------------
        if (m_scanline == 261 && m_sl_cycle == 1) {
            m_f_statusVblank = false;
            m_f_statusOverflow = false;
            m_f_statusSprZero = false;
        }

        // ----------- Rendering a Pixel --------------
        if (m_scanline >= 0 && m_scanline < 240) {
            if (m_sl_cycle > 0 && m_sl_cycle < 257) {
                // Background Value
                uint8_t bgPalIdx = 0;
                if (m_r_mask.bkgEnable) {
                    uint16_t bit = 0x8000 >> m_r_x;
                
                    bgPalIdx = ((m_shiftPatternLo & bit) ? 0b0001 : 0)
                             | ((m_shiftPatternHi & bit) ? 0b0010 : 0)
                             | ((m_shiftAttrLo    & bit) ? 0b0100 : 0)
                             | ((m_shiftAttrHi    & bit) ? 0b1000 : 0);
                }

                uint8_t fgPalIdx = 0;
                int sprIndex = 0;
                if (m_r_mask.sprEnable) {
                    for (sprIndex = 0; sprIndex < 8; sprIndex++) {
                        if (m_sprCounter[sprIndex] > 0) continue;
    
                        if (m_sprAttributes[sprIndex].hflip) {
                            fgPalIdx = ((m_sprTileLo[sprIndex] & 0x01) ? 0b0001 : 0)
                                     | ((m_sprTileHi[sprIndex] & 0x01) ? 0b0010 : 0)
                                     | (m_sprAttributes[sprIndex].palette << 2);
                        } else {
                            fgPalIdx = ((m_sprTileLo[sprIndex] & 0x80) ? 0b0001 : 0)
                                     | ((m_sprTileHi[sprIndex] & 0x80) ? 0b0010 : 0)
                                     | (m_sprAttributes[sprIndex].palette << 2);
                        }

                        if ((fgPalIdx & 0x3) != 0) {
                            break;
                        } else {
                            fgPalIdx = 0;
                        }
                    }

                    for (int i = 0; i < 8; i++) {
                        if (m_sprCounter[i] > 0) continue;
    
                        if (m_sprAttributes[i].hflip) {
                            m_sprTileLo[i] >>= 1;
                            m_sprTileHi[i] >>= 1;
                        } else {
                            m_sprTileLo[i] <<= 1;
                            m_sprTileHi[i] <<= 1;
                        }
                    }
                }

                if (isRenderingEnabled()) {
                    uint8_t value;
                    if ((fgPalIdx & 0x3) == 0) {
                        value = m_palette[BG_LUT[bgPalIdx]];
                    } else if ((bgPalIdx & 0x3) == 0) {
                        value = m_palette[FG_LUT[fgPalIdx]];
                    } else if (m_sprAttributes[sprIndex].priority) {
                        if (m_sprZeroOnSl && sprIndex == 0) {
                            m_f_statusSprZero = true;
                        }
                        value = m_palette[BG_LUT[bgPalIdx]];
                    } else {
                        value = m_palette[FG_LUT[fgPalIdx]];
                    }

                    if (m_setPixel) {
                        m_setPixel(m_sl_cycle - 1, m_scanline, value);
                    }
                }
            }
        }

        // ----- Update Sprite Counters -----------------
        if (isRenderingEnabled() && m_sl_cycle > 0 && m_sl_cycle <= 256) {
            for (int i = 0; i < 8; i++)
                if (m_sprCounter[i] > 0)
                    m_sprCounter[i]--;
        }

        // ----- Update Counters for next scanline ------
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
    case OAMDATA:   
        // TODO While Rendering this leaks internal OAM
        return m_oam.data[m_oamAddrExt];
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
    case OAMADDR:   m_oamAddrExt = value; break;
    case OAMDATA:   m_oam.data[m_oamAddrExt++] = value; break;
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
    m_r_t.baseNtX = v.baseNtX;
    m_r_t.baseNtY = v.baseNtY;
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
        m_r_t.lo = v;
        m_r_v.word = m_r_t.word;
    } else {
        m_r_addressLatch = !m_r_addressLatch;
        m_r_t.hi = v & 0x3f;
    }
}

void PPU::writeData(uint8_t v) {
    writeVram(m_r_v.word, v);
    m_r_v.word += m_r_addressIncrement;
}

uint8_t PPU::readData() {
    uint8_t value = (m_r_v.word < 0x3f00) ?
        m_r_dataReadBuffer :
        readVram(m_r_v.word);
    m_r_dataReadBuffer = readVram(m_r_v.word, true);
    m_r_v.word += m_r_addressIncrement;
    return value;
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
        case 0x00: case 0x10: 
            return m_palette[0x00];
        case 0x04: case 0x14:
            return m_palette[0x04];
        case 0x08: case 0x18:
            return m_palette[0x08];
        case 0x0C: case 0x1C:
            return m_palette[0x0C];
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
        case 0x00: case 0x10: 
            m_palette[0x00] = value;
            break;
        case 0x04: case 0x14:
            m_palette[0x04] = value;
            break;
        case 0x08: case 0x18:
            m_palette[0x08] = value;
            break;
        case 0x0C: case 0x1C:
            m_palette[0x0C] = value;
            break;
        default:
            m_palette[address] = value;
            break;
        }
    } else {
        LOG_ERR << "Illegal VRAM Write @ " << sm::hex(address) << "\n";
    }
}