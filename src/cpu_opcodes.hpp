#pragma once

#include <cstdint>

uint8_t constexpr OPC_BRK = 0x00;
uint8_t constexpr OPC_ORA_IND_X = 0x01;
uint8_t constexpr _OPC_NOP_ZPG__0 = 0x04;
uint8_t constexpr OPC_ORA_ZPG = 0x05;
uint8_t constexpr OPC_ASL_ZPG = 0x06;
uint8_t constexpr OPC_PHP = 0x08;
uint8_t constexpr OPC_ORA_IMD = 0x09;
uint8_t constexpr OPC_ASL_A = 0x0a;
uint8_t constexpr _OPC_NOP_ABS__0 = 0x0c;
uint8_t constexpr OPC_ORA_ABS = 0x0d;
uint8_t constexpr OPC_ASL_ABS = 0x0e;
uint8_t constexpr OPC_BPL = 0x10;
uint8_t constexpr OPC_ORA_IND_Y = 0x11;
uint8_t constexpr _OPC_NOP_ZPG_X__0 = 0x14;
uint8_t constexpr OPC_ORA_ZPG_X = 0x15;
uint8_t constexpr OPC_ASL_ZPG_X = 0x16;
uint8_t constexpr OPC_CLC = 0x18;
uint8_t constexpr OPC_ORA_ABS_Y = 0x19;
uint8_t constexpr _OPC_NOP__0 = 0x1a;
uint8_t constexpr _OPC_NOP_ABS_X__0 = 0x1c;
uint8_t constexpr OPC_ORA_ABS_X = 0x1d;
uint8_t constexpr OPC_ASL_ABS_X = 0x1e;
uint8_t constexpr OPC_JSR = 0x20;
uint8_t constexpr OPC_AND_IND_X = 0x21;
uint8_t constexpr OPC_BIT_ZPG = 0x24;
uint8_t constexpr OPC_AND_ZPG = 0x25;
uint8_t constexpr OPC_ROL_ZPG = 0x26;
uint8_t constexpr OPC_PLP = 0x28;
uint8_t constexpr OPC_AND_IMD = 0x29;
uint8_t constexpr OPC_ROL_A = 0x2a;
uint8_t constexpr OPC_BIT_ABS = 0x2c;
uint8_t constexpr OPC_AND_ABS = 0x2d;
uint8_t constexpr OPC_ROL_ABS = 0x2e;
uint8_t constexpr OPC_BMI = 0x30;
uint8_t constexpr OPC_AND_IND_Y = 0x31;
uint8_t constexpr _OPC_NOP_ZPG_X__1 = 0x34;
uint8_t constexpr OPC_AND_ZPG_X = 0x35;
uint8_t constexpr OPC_ROL_ZPG_X = 0x36;
uint8_t constexpr OPC_SEC = 0x38;
uint8_t constexpr OPC_AND_ABS_Y = 0x39;
uint8_t constexpr _OPC_NOP__1 = 0x3a;
uint8_t constexpr _OPC_NOP_ABS_X__1 = 0x3c;
uint8_t constexpr OPC_AND_ABS_X = 0x3d;
uint8_t constexpr OPC_ROL_ABS_X = 0x3e;
uint8_t constexpr OPC_RTI = 0x40;
uint8_t constexpr OPC_EOR_IND_X = 0x41;
uint8_t constexpr _OPC_NOP_ZPG__1 = 0x44;
uint8_t constexpr OPC_EOR_ZPG = 0x45;
uint8_t constexpr OPC_LSR_ZPG = 0x46;
uint8_t constexpr OPC_PHA = 0x48;
uint8_t constexpr OPC_EOR_IMD = 0x49;
uint8_t constexpr OPC_LSR_A = 0x4a;
uint8_t constexpr OPC_JMP = 0x4c;
uint8_t constexpr OPC_EOR_ABS = 0x4d;
uint8_t constexpr OPC_LSR_ABS = 0x4e;
uint8_t constexpr OPC_BVC = 0x50;
uint8_t constexpr OPC_EOR_IND_Y = 0x51;
uint8_t constexpr _OPC_NOP_ZPG_X__2 = 0x54;
uint8_t constexpr OPC_EOR_ZPG_X = 0x55;
uint8_t constexpr OPC_LSR_ZPG_X = 0x56;
uint8_t constexpr OPC_CLI = 0x58;
uint8_t constexpr OPC_EOR_ABS_Y = 0x59;
uint8_t constexpr _OPC_NOP__2 = 0x5a;
uint8_t constexpr _OPC_NOP_ABS_X__2 = 0x5c;
uint8_t constexpr OPC_EOR_ABS_X = 0x5d;
uint8_t constexpr OPC_LSR_ABS_X = 0x5e;
uint8_t constexpr OPC_RTS = 0x60;
uint8_t constexpr OPC_ADC_IND_X = 0x61;
uint8_t constexpr _OPC_NOP_ZPG__2 = 0x64;
uint8_t constexpr OPC_ADC_ZPG = 0x65;
uint8_t constexpr OPC_ROR_ZPG = 0x66;
uint8_t constexpr OPC_PLA = 0x68;
uint8_t constexpr OPC_ADC_ABS = 0x6d;
uint8_t constexpr OPC_ADC_IMD = 0x69;
uint8_t constexpr OPC_ROR_A = 0x6a;
uint8_t constexpr OPC_JMP_IND = 0x6c;
uint8_t constexpr OPC_ROR_ABS = 0x6e;
uint8_t constexpr OPC_BVS = 0x70;
uint8_t constexpr OPC_ADC_IND_Y = 0x71;
uint8_t constexpr _OPC_NOP_ZPG_X__3 = 0x74;
uint8_t constexpr OPC_ADC_ZPG_X = 0x75;
uint8_t constexpr OPC_ROR_ZPG_X = 0x76;
uint8_t constexpr OPC_SEI = 0x78;
uint8_t constexpr OPC_ADC_ABS_Y = 0x79;
uint8_t constexpr _OPC_NOP__3 = 0x7a;
uint8_t constexpr _OPC_NOP_ABS_X__3 = 0x7c;
uint8_t constexpr OPC_ADC_ABS_X = 0x7d;
uint8_t constexpr OPC_ROR_ABS_X = 0x7e;
uint8_t constexpr _OPC_NOP_IMD__0 = 0x80;
uint8_t constexpr OPC_STA_IND_X = 0x81;
uint8_t constexpr _OPC_NOP_IMD__1 = 0x82;
uint8_t constexpr OPC_STY_ZPG = 0x84;
uint8_t constexpr OPC_STA_ZPG = 0x85;
uint8_t constexpr OPC_STX_ZPG = 0x86;
uint8_t constexpr OPC_DEY = 0x88;
uint8_t constexpr _OPC_NOP_IMD__2 = 0x89;
uint8_t constexpr OPC_TXA = 0x8a;
uint8_t constexpr OPC_STY_ABS = 0x8c;
uint8_t constexpr OPC_STA_ABS = 0x8d;
uint8_t constexpr OPC_STX_ABS = 0x8e;
uint8_t constexpr OPC_BCC = 0x90;
uint8_t constexpr OPC_STA_IND_Y = 0x91;
uint8_t constexpr OPC_STY_ZPG_X = 0x94;
uint8_t constexpr OPC_STA_ZPG_X = 0x95;
uint8_t constexpr OPC_STX_ZPG_Y = 0x96;
uint8_t constexpr OPC_TYA = 0x98;
uint8_t constexpr OPC_STA_ABS_Y = 0x99;
uint8_t constexpr OPC_TXS = 0x9a;
uint8_t constexpr OPC_STA_ABS_X = 0x9d;
uint8_t constexpr OPC_LDY_IMD = 0xa0;
uint8_t constexpr OPC_LDA_IND_X = 0xa1;
uint8_t constexpr OPC_LDX_IMD = 0xa2;
uint8_t constexpr OPC_LDY_ZPG = 0xa4;
uint8_t constexpr OPC_LDA_ZPG = 0xa5;
uint8_t constexpr OPC_LDX_ZPG = 0xa6;
uint8_t constexpr OPC_TAY = 0xa8;
uint8_t constexpr OPC_LDA_IMD = 0xa9;
uint8_t constexpr OPC_TAX = 0xaa;
uint8_t constexpr OPC_LDY_ABS = 0xac;
uint8_t constexpr OPC_LDA_ABS = 0xad;
uint8_t constexpr OPC_LDX_ABS = 0xae;
uint8_t constexpr OPC_BCS = 0xb0;
uint8_t constexpr OPC_LDA_IND_Y = 0xb1;
uint8_t constexpr OPC_LDY_ZPG_X = 0xb4;
uint8_t constexpr OPC_LDA_ZPG_X = 0xb5;
uint8_t constexpr OPC_LDX_ZPG_Y = 0xb6;
uint8_t constexpr OPC_CLV = 0xb8;
uint8_t constexpr OPC_LDA_ABS_Y = 0xb9;
uint8_t constexpr OPC_TSX = 0xba;
uint8_t constexpr OPC_LDY_ABS_X = 0xbc;
uint8_t constexpr OPC_LDA_ABS_X = 0xbd;
uint8_t constexpr OPC_LDX_ABS_Y = 0xbe;
uint8_t constexpr OPC_CPY_IMD = 0xc0;
uint8_t constexpr OPC_CMP_IND_X = 0xc1;
uint8_t constexpr _OPC_NOP_IMD__3 = 0xc2;
uint8_t constexpr OPC_CPY_ZPG = 0xc4;
uint8_t constexpr OPC_CMP_ZPG = 0xc5;
uint8_t constexpr OPC_DEC_ZPG = 0xc6;
uint8_t constexpr OPC_INY = 0xc8;
uint8_t constexpr OPC_CMP_IMD = 0xc9;
uint8_t constexpr OPC_DEX = 0xca;
uint8_t constexpr OPC_CPY_ABS = 0xcc;
uint8_t constexpr OPC_CMP_ABS = 0xcd;
uint8_t constexpr OPC_DEC_ABS = 0xce;
uint8_t constexpr OPC_BNE = 0xd0;
uint8_t constexpr OPC_CMP_IND_Y = 0xd1;
uint8_t constexpr _OPC_NOP_ZPG_X__4 = 0xd4;
uint8_t constexpr OPC_CMP_ZPG_X = 0xd5;
uint8_t constexpr OPC_DEC_ZPG_X = 0xd6;
uint8_t constexpr OPC_CLD = 0xd8;
uint8_t constexpr OPC_CMP_ABS_Y = 0xd9;
uint8_t constexpr _OPC_NOP__4 = 0xda;
uint8_t constexpr _OPC_NOP_ABS_X__4 = 0xdc;
uint8_t constexpr OPC_CMP_ABS_X = 0xdd;
uint8_t constexpr OPC_DEC_ABS_X = 0xde;
uint8_t constexpr OPC_CPX_IMD = 0xe0;
uint8_t constexpr OPC_SBC_IND_X = 0xe1;
uint8_t constexpr _OPC_NOP_IMD__4 = 0xe2;
uint8_t constexpr OPC_CPX_ZPG = 0xe4;
uint8_t constexpr OPC_SBC_ZPG = 0xe5;
uint8_t constexpr OPC_INC_ZPG = 0xe6;
uint8_t constexpr OPC_INX = 0xe8;
uint8_t constexpr OPC_SBC_IMD = 0xe9;
uint8_t constexpr OPC_NOP = 0xea;
uint8_t constexpr _OPC_SBC_IMD__0 = 0xeb;
uint8_t constexpr OPC_CPX_ABS = 0xec;
uint8_t constexpr OPC_SBC_ABS = 0xed;
uint8_t constexpr OPC_INC_ABS = 0xee;
uint8_t constexpr OPC_BEQ = 0xf0;
uint8_t constexpr OPC_SBC_IND_Y = 0xf1;
uint8_t constexpr _OPC_NOP_ZPG_X__5 = 0xf4;
uint8_t constexpr OPC_SBC_ZPG_X = 0xf5;
uint8_t constexpr OPC_INC_ZPG_X = 0xf6;
uint8_t constexpr OPC_SED = 0xf8;
uint8_t constexpr OPC_SBC_ABS_Y = 0xf9;
uint8_t constexpr _OPC_NOP__5 = 0xfa;
uint8_t constexpr _OPC_NOP_ABS_X__5 = 0xfc;
uint8_t constexpr OPC_SBC_ABS_X = 0xfd;
uint8_t constexpr OPC_INC_ABS_X = 0xfe;

const int OPC_CYCLES[0x100] = {

    /*      0  1  2  3  4  5  6  7    8  9  a  b  c  d  e  f */

    /* 0 */ 7, 6, 0, 0, 3, 3, 5, 0,   3, 2, 2, 0, 4, 4, 6, 0,
    /* 1 */ 2, 5, 0, 0, 4, 4, 6, 0,   2, 4, 0, 0, 4, 4, 7, 0,
    /* 2 */ 6, 6, 0, 0, 3, 3, 5, 0,   4, 2, 2, 0, 4, 4, 6, 0,
    /* 3 */ 2, 5, 0, 0, 4, 4, 6, 0,   2, 4, 0, 0, 4, 4, 7, 0,
    /* 4 */ 6, 6, 0, 0, 3, 3, 5, 0,   3, 2, 2, 0, 3, 4, 6, 0,
    /* 5 */ 2, 5, 0, 0, 4, 4, 6, 0,   2, 4, 0, 0, 4, 4, 7, 0,
    /* 6 */ 6, 6, 0, 0, 3, 3, 5, 0,   4, 2, 2, 0, 5, 4, 6, 0,
    /* 7 */ 2, 5, 0, 0, 4, 4, 6, 0,   2, 4, 0, 0, 4, 4, 7, 0,

    /* 8 */ 0, 6, 0, 0, 3, 3, 3, 0,   2, 0, 2, 0, 4, 4, 4, 0,
    /* 9 */ 2, 6, 0, 0, 4, 4, 4, 0,   2, 5, 2, 0, 0, 5, 0, 0,
    /* a */ 2, 6, 2, 0, 3, 3, 3, 0,   2, 2, 2, 0, 4, 4, 4, 0,
    /* b */ 2, 5, 0, 0, 4, 4, 4, 0,   2, 4, 2, 0, 4, 4, 4, 0,
    /* c */ 2, 6, 0, 0, 3, 3, 5, 0,   2, 2, 2, 0, 4, 4, 6, 0,
    /* d */ 2, 5, 0, 0, 4, 4, 6, 0,   2, 4, 0, 0, 4, 4, 7, 0,
    /* e */ 2, 6, 0, 0, 3, 3, 5, 0,   2, 2, 2, 2, 4, 4, 6, 0,
    /* f */ 2, 5, 0, 0, 4, 4, 6, 0,   2, 4, 0, 0, 4, 4, 7, 0,
};
