#include <fstream>
#include <cstdlib>
#include <iostream>
#include "rom.hpp"

constexpr uint8_t FLAG_TRAINER = 1 << 2;

Cart::Cart(uint8_t* data) {
    this->data = data;

    // Now go through each entity in image and setup cart struct
    uint8_t* cart_off = this->data;
    cart_off += 0x10;  // After header

    // Cart has trainer?
    if (this->header->flags_6 & FLAG_TRAINER) {
        this->trainer = (trainer_bank*)cart_off;
        cart_off += TRAINER_BANK_SIZE;
    }
    else {
        this->trainer = nullptr;
    }

    // PRG ROM, always available
    this->prg_banks = (prg_bank*)cart_off;
    cart_off += PRG_BANK_SIZE * this->prg_size();

    // CHR ROM, available?
    if (this->header->chr_size != 0) {
        this->chr_banks = (chr_bank*)cart_off;
        cart_off += CHR_BANK_SIZE * this->chr_size();
    }
    else {
        this->chr_banks = nullptr;
    }

    // Setup mapper id from flags fields from hi nybble of flags 6, 7
    this->mapper_id = (this->header->flags_7 & 0xf0) & (this->header->flags_6 >> 4);
}

Cart::~Cart()
{
    delete this->data;
}

uint8_t Cart::readb_cpu(uint16_t addr)
{
    if (this->mapper_id == 0) {
        return this->readb_cpu_nrom(addr);
    }

    std::cerr << "ERROR: Unsupported Mapper " << this->mapper_id << std::endl;
    exit(1);
}

uint8_t Cart::readb_cpu_nrom(uint16_t addr)
{
    return this->prg(0)[addr - 0x8000];
}
