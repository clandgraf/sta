#include <iostream>

#include "ppu.hpp"

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
}

void PPU::writePPUCTRL(uint8_t value) {
    m_f_vblank_nmi = value & 0b10000000;
    // Add missing flags
}

void PPU::run(unsigned int cycles) {
    for (int i = 0; i < cycles; i++) {
        // Do PPU Stuff
    }
}
