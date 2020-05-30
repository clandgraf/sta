#pragma once

#include <vector>
#include <map>

class Emu;

struct DisasmLine {
    uint16_t offset;
    std::string repr;
};

struct DisasmSegment {
    uint16_t m_start;
    uint16_t m_length;

    std::map<uint16_t, DisasmLine> m_lines;

    DisasmSegment(uint16_t start) : m_start(start) {}
};

const char* disasmNextOpcode(Emu& emu, bool* end = nullptr, uint8_t* next = nullptr);
std::shared_ptr<DisasmSegment> disasmSegment(Emu& emu, uint16_t addr);