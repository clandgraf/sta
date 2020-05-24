#include <fstream>
#include <cstdlib>
#include <iostream>
#include "rom.hpp"

const uint8_t FLAG_TRAINER = 1 << 2;

uint8_t* readFile(const char* path) {
    std::ifstream file(path, std::ios::binary);

    file.seekg(0, file.end);
    size_t length = file.tellg();
    file.seekg(0, file.beg);

    uint8_t* data = new uint8_t[length];
    file.read((char*)(data), length);
    
    return data;
}

cart::cart(const char* path)
{
    this->data = readFile(path);

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

cart::~cart()
{
    delete this->data;
}

uint8_t cart::readb_cpu(uint16_t addr)
{
    if (this->mapper_id == 0) {
        return this->readb_cpu_nrom(addr);
    }

    std::cerr << "ERROR: Unsupported Mapper " << this->mapper_id << std::endl;
    exit(1);
}

uint8_t cart::readb_cpu_nrom(uint16_t addr)
{
    return this->prg(0)[addr - 0x8000];
}