#pragma once

#ifndef NES_CART_H
#define NES_CART_H

#include <cstdlib>
#include <cstdint>
#include <filesystem>

#include "defs.hpp"
#include "util.hpp"

constexpr uint32_t HEADER_AS_UINT32(uint8_t* h) {
    return ((h[0]) | ((h[1]) << 8) | ((h[2]) << 16) | ((h[3]) << 24));
}

uint32_t constexpr ZIP_MAGIC = 0x04034b50;
uint32_t constexpr INES_MAGIC = 0x1a53454e;

size_t constexpr PRG_BANK_SIZE = 0x4000;
size_t constexpr CHR_BANK_SIZE = 0x2000;
size_t constexpr TRAINER_BANK_SIZE = 0x200;

typedef uint8_t prg_bank[PRG_BANK_SIZE];
typedef uint8_t chr_bank[CHR_BANK_SIZE];
typedef uint8_t trainer_bank[TRAINER_BANK_SIZE];

PACK(
struct ines_header {
    uint8_t magic[4];
    uint8_t prg_size;
    uint8_t chr_size;
    uint8_t flags_6;
    uint8_t flags_7;
    uint8_t prg_ram_size;
    uint8_t flags_9;
    uint8_t flags_10;
    uint8_t zero[5];
};
)

class Cart {
public:
    static Cart* fromFile(const std::filesystem::path& p);

    union {
        uint8_t* data;
        ines_header* header;
    };

    uint8_t mapper_id;

    trainer_bank* trainer;
    prg_bank* prg_banks;
    chr_bank* chr_banks;
    // play choice inst-rom
    // play choice p-rom

    Cart(uint8_t*);
    ~Cart();

    inline uint8_t prg_size() const { return this->header->prg_size; }
    inline uint8_t chr_size() const { return this->header->chr_size; }

    inline prg_bank& prg(uint8_t bank) const { return this->prg_banks[bank]; };
    inline chr_bank& chr(uint8_t bank) const { return this->chr_banks[bank]; };

    uint8_t readb_cpu(uint16_t addr);
    void writeb_cpu(uint16_t addr, uint8_t value);

    uint8_t readb_ppu(uint16_t addr);

private:
    uint8_t readb_cpu_nrom(uint16_t addr);
    void writeb_cpu_nrom(uint16_t addr, uint8_t value);

    uint8_t readb_ppu_nrom(uint16_t addr);
};

#endif
