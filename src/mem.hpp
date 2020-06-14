#pragma once

#ifndef NES_MEM_H
#define NES_MEM_H_

#include <cstdint>

class Cart;
class PPU;

class Memory {
public:
    static bool isCartSpace(uint16_t addr);

    Memory(Cart*, PPU* m_ppu);

    uint8_t readb(uint16_t addr);
    void writeb(uint16_t addr, uint8_t value);

    uint8_t m_internalRam[0x800];

private:
    Cart* m_cart;
    PPU* m_ppu;
};

#endif
