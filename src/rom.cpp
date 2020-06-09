#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <miniz.h>
#include "rom.hpp"

namespace fs = std::filesystem;

constexpr uint8_t FLAG_TRAINER = 1 << 2;

uint8_t* nesFromZip(const fs::path& p, size_t& len) {
    LOG_MSG << "Unzipping " << p << "\n";

    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));
    bool success = mz_zip_reader_init_file(&zip, p.string().c_str(), 0);
    if (!success) {
        LOG_ERR << "Failed unzipping " << p << "\n";
        return nullptr;
    }

    mz_uint fileCount = mz_zip_reader_get_num_files(&zip);
    mz_zip_archive_file_stat pStat;
    mz_uint i = 0;
    for (; i < fileCount; i++) {
        mz_zip_reader_file_stat(&zip, i, &pStat);
        fs::path p(pStat.m_filename);
        if (p.extension() == ".nes") {
            LOG_MSG << "Found ROM " << pStat.m_filename << "\n";
            break;
        }
    }

    if (i == fileCount) {
        LOG_ERR << "No ROM found in " << p << "\n";
        return nullptr;
    }

    uint8_t* data = (uint8_t*)mz_zip_reader_extract_to_heap(&zip, i, &len, 0);
    mz_zip_reader_end(&zip);
    return data;
}

Cart* Cart::fromFile(const fs::path& p) {
    LOG_MSG << "Loading " << p << "\n";

    uint8_t* data;
    size_t len;
    if (p.extension() == ".zip") {
        data = nesFromZip(p, len);
        if (data == nullptr) {
            return nullptr;
        }
    } else {
        std::ifstream in(p);
        data = readFile(in, &len);
        in.close();
    }
    
    // Check if we have a valid .nes rom
    if (INES_MAGIC != HEADER_AS_UINT32(((ines_header*)data)->magic)) {
        LOG_ERR << "ROM is not a valid .nes rom\n";
        return nullptr;
    }

    return new Cart(data);
}

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

    LOG_ERR << "Unsupported Mapper " << this->mapper_id << "\n";
    exit(1);
}

void Cart::writeb_cpu(uint16_t addr, uint8_t value) {
    if (this->mapper_id == 0) {
        return this->writeb_cpu_nrom(addr, value);
    }

    LOG_ERR << "Unsupported Mapper"  << this->mapper_id << "\n";
    exit(1);
}

uint8_t Cart::readb_cpu_nrom(uint16_t addr)
{
    return this->prg(0)[addr - 0x8000];
}

uint8_t Cart::readb_ppu(uint16_t addr)
{
    if (this->mapper_id == 0) {
        return this->readb_ppu_nrom(addr);
    }

    LOG_ERR << "Unsupported Mapper " << this->mapper_id << "\n";
    exit(1);
}

uint8_t Cart::readb_ppu_nrom(uint16_t addr) {
    return this->chr(0)[addr];
}

void Cart::writeb_cpu_nrom(uint16_t addr, uint8_t value) {
    LOG_ERR << "Write to NROM address " << addr << "\n";
}
