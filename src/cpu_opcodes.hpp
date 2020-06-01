#pragma once

#include <cstdint>

uint8_t constexpr OPC_BPL = 0x10;
uint8_t constexpr OPC_JSR = 0x20;
uint8_t constexpr OPC_BMI = 0x30;
uint8_t constexpr OPC_RTI = 0x40;
uint8_t constexpr OPC_JMP = 0x4c;
uint8_t constexpr OPC_BVC = 0x50;
uint8_t constexpr OPC_CLI = 0x58;
uint8_t constexpr OPC_RTS = 0x60;
uint8_t constexpr OPC_JMP_IND = 0x6c;
uint8_t constexpr OPC_BVS = 0x70;
uint8_t constexpr OPC_SEI = 0x78;
uint8_t constexpr OPC_STA_ABS = 0x8d;
uint8_t constexpr OPC_BCC = 0x90;
uint8_t constexpr OPC_TXS = 0x9a;
uint8_t constexpr OPC_LDX_IMD = 0xa2;
uint8_t constexpr OPC_LDA_IMD = 0xa9;
uint8_t constexpr OPC_LDA_ABS = 0xad;
uint8_t constexpr OPC_BCS = 0xb0;
uint8_t constexpr OPC_BNE = 0xd0;
uint8_t constexpr OPC_CLD = 0xd8;
uint8_t constexpr OPC_BEQ = 0xf0;
uint8_t constexpr OPC_SED = 0xf8;