#include <iostream>

#include "rom.hpp"
#include "mem.hpp"

mem::mem(cart* cart_)
    : m_cart(cart_)
{}

uint8_t mem::readb(uint16_t addr) {
    // Internal RAM, mirrored
    if (addr < 0x2000) {
        uint8_t addr_lo = addr & 0x7ff;
        return internal_ram[addr_lo];
    }
    // PPU Registers, mirrored
    else if (addr < 0x4000) {
        uint8_t addr_lo = addr & 0x8;
        // return m_ppu.read_register(addr_lo);
        return 0;
    }
    // APU/IO Registers
    else if (addr < 0x4018) {
        std::cerr << "ERROR: Access to APU/IO" << std::endl;
        exit(1);
    }
    // CPU Test Mode registers
    else if (addr < 0x4020) {
        std::cerr << "ERROR " << __FILE__ << ":" << __LINE__ << ": Access to Test Mode registers" << std::endl;
        exit(1);
    }
    // Access Cartridge CPU Bus
    else {
        return m_cart->readb_cpu(addr);
    }
}
