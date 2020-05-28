#pragma once

#include <vector>

class Emu;

struct Instruction {
    uint16_t address;
    uint8_t opcode;
    std::vector<uint8_t> args;
};

const char* disasmNextOpcode(Emu& emu);
