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

class Mapper;

class Cart {
    PACK(struct InesHeader {
        uint32_t             magic;
        uint8_t              prgSize;
        uint8_t              chrSize;
        union {
            struct {
                unsigned int mirroring : 1;
                unsigned int hasPrgRam : 1;
                unsigned int hasTrainer : 1;
                unsigned int hasVram : 1;
                unsigned int mapperLo : 4;
            };
            uint8_t          flags6;
        };
        union {
            struct {
                unsigned int isVsUnisystem : 1;
                unsigned int isPlaychoice10 : 1;
                unsigned int isHeader20 : 2;
                unsigned int mapperHi : 4;
            };
            uint8_t          flags7;
        };
        uint8_t              prg_ram_size;
        uint8_t              flags_9;
        uint8_t              flags_10;
        uint8_t              zero[5];
    });

public:
    static std::shared_ptr<Cart> fromFile(const std::filesystem::path& p);

    const std::string m_name;

    union {
        uint8_t* m_data;
        InesHeader* m_header;
    };

    uint8_t m_mapperId;
    bool m_useChrRam = false;

    trainer_bank* m_trainer = nullptr;
    prg_bank* m_prgBanks;
    chr_bank* m_chrBanks;
    // play choice inst-rom
    // play choice p-rom

    Cart(uint8_t*, std::string file = "<none>");
    ~Cart();

    inline uint8_t prgSize() const { return m_header->prgSize; }
    inline uint8_t chrSize() const { return m_chrSize; }

    inline prg_bank& prg(uint8_t bank) const { return m_prgBanks[bank]; };
    inline chr_bank& chr(uint8_t bank) const { return m_chrBanks[bank]; };

    uint8_t readb_cpu(uint16_t address);
    void translate_cpu(uint16_t addressIn, uint8_t& bankOut, uint16_t& addressOut);
    void writeb_cpu(uint16_t address, uint8_t value);

    uint8_t readb_ppu(uint16_t address);
    void writeb_ppu(uint16_t address, uint8_t value);

    uint16_t getNameTable(uint8_t index);

private:
    uint8_t m_chrSize = 0;

    std::shared_ptr<Mapper> m_mapper;
};

#endif
