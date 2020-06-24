#include <iostream>

#include "rom.hpp"
#include "mem.hpp"
#include "rom.hpp"
#include "ppu.hpp"
#include "emu.hpp"
#include "util.hpp"

Memory::Memory(Emu* emu, Cart* cart, PPU* ppu)
    : m_emu(emu), m_cart(cart), m_ppu(ppu)
{}

uint8_t Memory::readb(uint16_t addr) {
    // Internal RAM, mirrored
    if (addr < 0x2000) {
        uint16_t addr_lo = addr & 0x7ff;
        return m_internalRam[addr_lo];
    }
    // PPU Registers, mirrored
    else if (addr < 0x4000) {
        uint8_t addr_lo = addr & 0b00000111;
        return m_ppu->read_register(addr_lo);
    }
    // OAM DMA
    else if (addr == 0x4014) {
        LOG_ERR << "readb on OAMDMA\n";
        return 0;
    }
    // APU/IO Registers
    else if (addr < 0x4018) {
        LOG_ERR << "readb(" << std::hex << std::setw(4) << std::setfill('0') <<  addr << ")" 
                << "Access to APU/IO\n";
        return 0;
    }
    // CPU Test Mode registers
    else if (addr < 0x4020) {
        LOG_ERR << "readb(" << std::hex << std::setw(4) << std::setfill('0') << addr << ")" 
                << "Access to Test Mode registers\n";
        return 0;
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
        uint16_t addr_lo = addr & 0x7ff;
        m_internalRam[addr_lo] = value;
    }
    else if (addr < 0x4000) {
        uint8_t addr_lo = addr & 0b00000111;
        m_ppu->write_register(addr_lo, value);
    }
    // OAM DMA
    else if (addr == 0x4014) {
        m_emu->startDMA(value);
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
