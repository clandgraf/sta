#include "rom.hpp"
#include "util.hpp"
#include "mappers/mapper001.hpp"

namespace sm = StreamManipulators;

Mapper001::Mapper001(Cart& m_cart) : Mapper(m_cart) {}
Mapper001::~Mapper001() {}

uint8_t Mapper001::readbCpu(uint16_t address) {
    address -= 0x8000;
    if (m_cart.prgSize() == 1) {
        address &= 0x3fff;
    }
    return m_cart.prg(0)[address];
}

void Mapper001::writebCpu(uint16_t address, uint8_t value) {
    LOG_ERR << "Write to NROM address " << address << "\n";
}

void Mapper001::translateCpu(uint16_t addressIn, uint8_t& bankOut, uint16_t& addressOut) {
    bankOut = 0;
    addressOut = addressIn - 0x8000;
    if (m_cart.prgSize() == 1) {
        addressOut &= 0x3fff;
    }
}

uint8_t Mapper001::readbPpu(uint16_t address) {
    return m_cart.chr(0)[address];
}

void Mapper001::writebPpu(uint16_t address, uint8_t value) {
    if (m_cart.m_useChrRam) {
        m_cart.chr(0)[address] = value;
    }
    else {
        LOG_ERR << "Illegal write to CHR ROM @ " << sm::hex(address) << "\n";
    }
}
