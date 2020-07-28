#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <miniz.h>
#include <memory>

#include "rom.hpp"
#include "util.hpp"
#include "mappers/mapper000.hpp"

namespace fs = std::filesystem;
namespace sm = StreamManipulators;

constexpr uint8_t FLAG_TRAINER = 1 << 2;

uint8_t* nesFromZip(const fs::path& p, size_t& len, std::string& out_name) {
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
            out_name = pStat.m_filename;
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

std::shared_ptr<Cart> Cart::fromFile(const fs::path& p) {
    LOG_MSG << "Loading " << p << "\n";
    
    std::string name;
    uint8_t* data;
    size_t len;
    if (p.extension() == ".zip") {
        data = nesFromZip(p, len, name);
        if (data == nullptr) {
            return nullptr;
        }
    } else {
        std::ifstream in(p, std::ifstream::binary);
        data = readFile(in, &len);
        in.close();

        name = p.filename().string();
    }
    
    // Check if we have a valid .nes rom
    if (INES_MAGIC != ((InesHeader*)data)->magic) {
        LOG_ERR << "ROM is not a valid .nes rom\n";
        return nullptr;
    }

    return std::make_shared<Cart>(data, name);
}

Cart::Cart(uint8_t* data, std::string name) : m_data(data), m_name(name) {
    // Now go through each entity in image and setup cart struct
    uint8_t* cart_off = m_data;
    cart_off += 0x10;  // After header

    // Cart has trainer?
    if (m_header->hasTrainer) {
        m_trainer = (trainer_bank*)cart_off;
        cart_off += TRAINER_BANK_SIZE;
    }

    // PRG ROM, always available
    m_prgBanks = (prg_bank*)cart_off;
    cart_off += PRG_BANK_SIZE * this->prgSize();

    // CHR ROM, available?
    if (m_header->chrSize != 0) {
        m_chrBanks = (chr_bank*)cart_off;
        m_chrSize = m_header->chrSize;
        cart_off += CHR_BANK_SIZE * this->chrSize();
    } else {
        // CHR RAM
        m_chrSize = 1;
        m_useChrRam = true;
        m_chrBanks = new chr_bank[1];
    }

    // Setup mapper id from flags fields from hi nybble of flags 6, 7
    m_mapperId = (m_header->mapperHi << 4) | (m_header->mapperLo);
    switch (m_mapperId) {
    case 0: m_mapper = std::make_shared<Mapper000>(*this); break;
    default:
        LOG_ERR << "Mapper " << m_mapperId << " is not supported.\n";
        exit(1);
    }
}

Cart::~Cart()
{
    if (m_useChrRam) {
        delete[] m_chrBanks;
    }
    delete m_data;
}

const uint16_t nametableOffsets[2][4] = {
    { 0x0000, 0x0000, 0x0400, 0x0400 },  // Horizontal
    { 0x0000, 0x0400, 0x0000, 0x0400 },  // Vertical
};

uint16_t Cart::getNameTable(uint8_t index) {
    return nametableOffsets[m_header->mirroring][index];
}

uint8_t Cart::readb_cpu(uint16_t address)
{
    return m_mapper->readbCpu(address);
}

void Cart::translate_cpu(uint16_t addressIn, uint8_t& bankOut, uint16_t& addressOut) {
    return m_mapper->translateCpu(addressIn, bankOut, addressOut);
}

void Cart::writeb_cpu(uint16_t addr, uint8_t value) {
    m_mapper->writebCpu(addr, value);
}

uint8_t Cart::readb_ppu(uint16_t addr)
{
    return m_mapper->readbPpu(addr);
}

void Cart::writeb_ppu(uint16_t addr, uint8_t value)
{
    m_mapper->writebPpu(addr, value);
}
