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
    using Sptr = std::shared_ptr<Cart>;

    static Sptr fromFile(const std::filesystem::path& p);

    union {
        uint8_t* m_data;
        ines_header* m_header;
    };

    uint8_t m_mapperId;
    bool m_useChrRam = false;

    trainer_bank* m_trainer = nullptr;
    prg_bank* m_prgBanks;
    chr_bank* m_chrBanks;
    // play choice inst-rom
    // play choice p-rom

    Cart(uint8_t*);
    ~Cart();

    inline uint8_t prg_size() const { return m_header->prg_size; }
    inline uint8_t chr_size() const { return m_chrSize; }

    inline prg_bank& prg(uint8_t bank) const { return m_prgBanks[bank]; };
    inline chr_bank& chr(uint8_t bank) const { return m_chrBanks[bank]; };

    uint8_t readb_cpu(uint16_t address);
    void translate_cpu(uint16_t addressIn, uint8_t& bankOut, uint16_t& addressOut);
    void writeb_cpu(uint16_t address, uint8_t value);

    uint8_t readb_ppu(uint16_t address);

private:
    uint8_t m_chrSize = 0;

    uint8_t readb_cpu_nrom(uint16_t address);
    void translate_cpu_nrom(uint16_t addressIn, uint8_t& bankOut, uint16_t& addressOut);
    void writeb_cpu_nrom(uint16_t address, uint8_t value);

    uint8_t readb_ppu_nrom(uint16_t address);
};

#endif
