#pragma once

#ifndef NES_MEM_H
#define NES_MEM_H_

#include <cstdint>
#include <memory>

class Cart;
class PPU;
class Emu;

class Memory {
public:
    static bool isCartSpace(uint16_t addr);

    Memory(Emu&, std::shared_ptr<Cart>, std::shared_ptr<PPU>);

    uint8_t readb(uint16_t addr);
    void writeb(uint16_t addr, uint8_t value);

    uint8_t m_internalRam[0x800];

private:
    Emu& m_emu;

    std::shared_ptr<Cart> m_cart;
    std::shared_ptr<PPU> m_ppu;
};

#endif
