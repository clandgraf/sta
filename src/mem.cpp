#include <iostream>

#include "nes/rom.hpp"
#include "mem.hpp"
#include "ppu.hpp"
#include "emu.hpp"
#include "core/util.hpp"
#include "controllers.hpp"

namespace sm = StreamManipulators;

Memory::Memory(Emu& emu, std::shared_ptr<Cart> cart, std::shared_ptr<PPU> ppu)
    : m_emu(emu), m_cart(cart), m_ppu(ppu)
{}

void Memory::setPort0(std::shared_ptr<Port> p) { m_port0 = p; }
void Memory::setPort1(std::shared_ptr<Port> p) { m_port1 = p; }

uint8_t Memory::readb(uint16_t addr) {
    // Internal RAM, mirrored
    if (addr < 0x2000) {
        uint16_t addr_lo = addr & 0x7ff;
        return m_internalRam[addr_lo];
    }
    // PPU Registers, mirrored
    else if (addr < 0x4000) {
        uint8_t addr_lo = addr & 0b00000111;
        return m_ppu->readRegister(addr_lo);
    }
    // OAM DMA
    else if (addr == 0x4014) {
        LOG_ERR << "readb on OAMDMA\n";
        return 0;
    }
    // Gamepad
    else if (addr == 0x4016) {
        return m_port0->read();
    }
    else if (addr == 0x4017) {
        return m_port1->read();
    }
    // APU/IO Registers
    else if (addr < 0x4018) {
        //LOG_ERR << "readb(" 
        //        << sm::hex(addr) 
        //        << ") Access to APU/IO\n";
        return 0;
    }
    // CPU Test Mode registers
    else if (addr < 0x4020) {
        LOG_ERR << "readb(" 
                << sm::hex(addr) 
                << ") Access to Test Mode registers\n";
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
        m_ppu->writeRegister(addr_lo, value);
    }
    // OAM DMA
    else if (addr == 0x4014) {
        m_emu.startDMA(value);
    }
    // Gamepad
    else if (addr == 0x4016) {
        m_port0->write(value);
    }
    // APU/IO Registers
    else if (addr < 0x4018) {
        //LOG_ERR << "writeb(" 
        //        << sm::hex(addr) << ", " << sm::hex(value) 
        //        << ") Access to APU/IO\n";
    }
    // CPU Test Mode registers
    else if (addr < 0x4020) {
        LOG_ERR << "writeb(" 
                << sm::hex(addr) << ", " << sm::hex(value) 
                << ") Access to Test Mode registers\n";
    }
    // Access Cartridge CPU Bus
    else {
        return m_cart->writeb_cpu(addr, value);
    }
}
