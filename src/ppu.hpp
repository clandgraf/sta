#pragma once

#include <cstdint>

class Cart;

uint8_t constexpr PPUCTRL   = 0x0;
uint8_t constexpr PPUMASK   = 0x1;
uint8_t constexpr PPUSTATUS = 0x2;
uint8_t constexpr OAMADDR   = 0x3;
uint8_t constexpr OAMDATA   = 0x4;
uint8_t constexpr PPUSCROLL = 0x5;
uint8_t constexpr PPUADDR   = 0x6;
uint8_t constexpr PPUDATA   = 0x7;

class PPU {
public:
    PPU(Cart* cart) : m_cart(cart) {}

    uint8_t read_register(uint8_t reg);
    void write_register(uint8_t reg, uint8_t value);

private:
    Cart* m_cart;
};
