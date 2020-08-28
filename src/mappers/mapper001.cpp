#include "rom.hpp"
#include "util.hpp"
#include "mappers/mapper001.hpp"

namespace sm = StreamManipulators;

Mapper001::Mapper001(Cart& m_cart) : Mapper(m_cart) {
    m_prgRam = new uint8_t[0x2000];
}

Mapper001::~Mapper001() {
    delete[] m_prgRam;
}

uint8_t Mapper001::readbCpu(uint16_t address) {
    if (address < 0x6000) {
        // Unmapped
        return 0;
    } else if (address < 0x8000) {
        // TODO PRG RAM might be disabled
        return m_prgRam[address & 0xe000];
    } else if (address < 0xc000) {
        // 16 KB PRG ROM bank, either switchable or fixed to the first bank
        return (*m_prgBanks[0])[address & 0x3fff];
    } else {
        // 16 KB PRG ROM bank, either fixed to the last bank or switchable
        return (*m_prgBanks[1])[address & 0x3fff];
    }
}

void Mapper001::updatePrgBanks() {
    switch (m_control.prgMode) {
        case 0: case 1:
            m_prgBanks[0] = &m_cart.m_prgBanks[(m_prgBankSelect & 0xfe) + 0];
            m_prgBanks[1] = &m_cart.m_prgBanks[(m_prgBankSelect & 0xfe) + 1];
            break;
        case 2:
            m_prgBanks[0] = &m_cart.m_prgBanks[0];
            m_prgBanks[1] = &m_cart.m_prgBanks[m_prgBankSelect];
            break;
        case 3:
            m_prgBanks[0] = &m_cart.m_prgBanks[m_prgBankSelect];
            m_prgBanks[1] = &m_cart.m_prgBanks[m_cart.prgSize() - 1];
            break;
    }
}

void Mapper001::writebCpu(uint16_t address, uint8_t value) {
    if (address < 0x6000) {
        // Nothing
    } else if (address < 0x8000) {
        // TODO PRG RAM might be disabled
        m_prgRam[address & 0xe000] = value;
    } else {
        if (0x80 & value) {
            m_shifter = 0;
            m_counter = 0;
        } else {
            m_shifter <<= 1;
            m_shifter |= 0x01 & value;
            m_counter++;

            if (m_counter == 5) {
                uint16_t a = address & 0x6000;
                // TODO writing to other registers
                switch (a) {
                    case 0x0000:
                        m_control.value = m_shifter;
                        updatePrgBanks();
                        break;
                    case 0x2000:
                    case 0x4000:
                    case 0x6000:
                        m_prgRamEnable = !(m_shifter & 0x10);
                        m_prgBankSelect = m_shifter & 0x0f;
                        updatePrgBanks();
                        break;
                }
                m_counter = 0;
                m_shifter = 0;
            }
        }
    }

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
