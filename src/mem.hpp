#pragma once

#ifndef NES_MEM_H
#define NES_MEM_H_

#include <cstdint>

class cart;

class mem {
public:
    mem(cart* cart_);

    uint8_t readb(uint16_t addr);

private:
    cart* m_cart;

    uint8_t internal_ram[0x800];
};

#endif
