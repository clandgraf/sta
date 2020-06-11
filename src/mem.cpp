#include <iostream>

#include "rom.hpp"
#include "mem.hpp"
#include "rom.hpp"
#include "ppu.hpp"
#include "util.hpp"

Memory::Memory(Cart* cart, PPU* ppu)
    : m_cart(cart), m_ppu(ppu)
{}

uint8_t Memory::readb(uint16_t addr) {
    // Internal RAM, mirrored
    if (addr < 0x2000) {
        uint8_t addr_lo = addr & 0x7ff;
        return internal_ram[addr_lo];
    }
    // PPU Registers, mirrored
    else if (addr < 0x4000) {
        uint8_t addr_lo = addr & 0b00000111;
        return m_ppu->read_register(addr_lo);
    }
    // APU/IO Registers
    else if (addr < 0x4018) {
        LOG_ERR << "Access to APU/IO\n";
        exit(1);
    }
    // CPU Test Mode registers
    else if (addr < 0x4020) {
        LOG_ERR << "Access to Test Mode registers\n";
        exit(1);
    }
    // Access Cartridge CPU Bus
    else {
        return m_cart->readb_cpu(addr);
    }
}

bool Memory::isCartSpace(uint16_t addr) {
    return addr > 0x4020;
}

void Memory::writeb(uint16_t addr, uint8_t value) {
    if (addr < 0x2000) {
        uint8_t addr_lo = addr & 0x7ff;
        internal_ram[addr_lo] = value;
    }
    else if (addr < 0x4000) {
        uint8_t addr_lo = addr & 0b00000111;
        m_ppu->write_register(addr_lo, value);
    }
    // APU/IO Registers
    else if (addr < 0x4018) {
        LOG_ERR << "ERROR: Access to APU/IO\n";
        exit(1);
    }
    // CPU Test Mode registers
    else if (addr < 0x4020) {
        LOG_ERR << "ERROR:  Access to Test Mode registers\n";
        exit(1);
    }
    // Access Cartridge CPU Bus
    else {
        return m_cart->writeb_cpu(addr, value);
    }
}
