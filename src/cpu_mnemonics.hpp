#pragma once

#include <cstdint>

const char* opc_mnemonics[0x100] = {
 //          0                1                 2             3   4               5                6               7   8      9                 a        b   c                  d                 e                 f
 /* 0 */    "BRK",           "ORA ($%02x, X)", "",           "", "",             "ORA $%02x",     "ASL $%02x",    "", "PHP", "ORA #$%02x",     "ASL A", "", "",                "ORA $%02x00",    "ASL $%02x00",    "",
 /* 1 */    "BPL %02x",      "ORA ($%02x), Y", "",           "", "",             "ORA $%02x, X",  "ASL $%02x, X", "", "CLC", "ORA $%02x00, Y", "",      "", "",                "ORA $%02x00, X", "ASL $%02x00, X", "",
 /* 2 */    "JSR $%02x%02x", "AND ($%02x, X)", "",           "", "BIT $%02x",    "AND $%02x",     "ROL $%02x",    "", "PLP", "AND #$%02x",     "ROL A", "", "BIT $%02x00",     "AND $%02x00",    "ROL $%02x00",    "",
 /* 3 */    "BMI %02x",      "AND ($%02x), Y", "",           "", "",             "AND $%02x, X",  "ROL $%02x, X", "", "SEC", "AND $%02x00, Y", "",      "", "",                "AND $%02x00, X", "ROL $%02x00, X", "",
 /* 4 */    "RTI",           "EOR ($%02x, X)", "",           "", "",             "EOR $%02x",     "LSR $%02x",    "", "PHA", "EOR #$%02x",     "LSR A", "", "JMP $%02x%02x",   "EOR $%02x00",    "LSR $%02x00",    "",
 /* 5 */    "BVC %02x",      "EOR ($%02x), Y", "",           "", "",             "EOR $%02x, X",  "LSR $%02x, X", "", "CLI", "EOR $%02x00, Y", "",      "", "",                "EOR $%02x00, X", "LSR $%02x00, X", "",
 /* 6 */    "RTS",           "ADC ($%02x, X)", "",           "", "",             "ADC $%02x",     "ROR $%02x",    "", "PLA", "ADC #$%02x",     "ROR A", "", "JMP ($%02x%02x)", "ADC $%02x00",    "ROR $%02x00",    "",
 /* 7 */    "BVS %02x",      "ADC ($%02x), Y", "",           "", "",             "ADC $%02x, X",  "ROR $%02x, X", "", "SEI", "ADC $%02x00, Y", "",      "", "",                "ADC $%02x00, X", "ROR $%02x00, X", "",
 /* 8 */    "",              "STA ($%02x, X)", "",           "", "STY $%02x",    "STA $%02x",     "STX $%02x",    "", "DEY", "",               "TXA",   "", "STY $%02x00",     "STA $%02x00",    "STX $%02x00",    "",
 /* 9 */    "BCC %02x",      "STA ($%02x) ,Y", "",           "", "STY $%02x, X", "STA $%02x, X",  "STX $%02x, Y", "", "TYA", "STA $%02x00, Y", "TXS",   "", "",                "STA $%02x00, X", "",               "",
 /* A */    "LDY #$%02x",    "LDA ($%02x, X)", "LDX #$%02x", "", "LDY $%02x",    "LDA $%02x",     "LDX $%02x",    "", "TAY", "LDA #$%02x",     "TAX",   "", "LDY $%02x00",     "LDA $%02x00",    "LDX $%02x00",    "",
 /* B */    "BCS %02x",      "LDA ($%02x), Y", "",           "", "LDY $%02x, X", "LDA $%02x, X",  "LDX $%02x, Y", "", "CLV", "LDA $%02x00, Y", "TSX",   "", "",                "LDA $%02x00, X", "LDX $%02x00, Y", "",
 /* C */    "CPY #$%02x",    "CMP ($%02x, X)", "",           "", "CPY $%02x",    "CMP $%02x",     "DEC $%02x",    "", "INY", "CMP #$%02x",     "DEX",   "", "CPY $%02x00",     "CMP $%02x00",    "DEC $%02x00",    "",
 /* D */    "BNE %02x",      "CMP ($%02x), Y", "",           "", "",             "CMP $%02x, X",  "DEC $%02x, X", "", "CLD", "CMP $%02x00, Y", "",      "", "",                "CMP $%02x00, X", "DEC $%02x00, X", "",
 /* E */    "CPX #$%02x",    "SBC ($%02x, X)", "",           "", "CPX $%02x",    "SBC $%02x",     "INC $%02x",    "", "INX", "SBC #$%02x",     "NOP",   "", "CPX $%02x00",     "SBC $%02x00",    "INC $%02x00",    "",
 /* F */    "BEQ %02x",      "SBC ($%02x), Y", "",           "", "",             "SBC $%02x, X",  "INC $%02x, X", "", "SED", "SBC $%02x00, Y", "",      "", "",                "SBC $%02x00, X", "INC $%02x00, X", "",
};

// Mnemonic Params

uint8_t constexpr END = 0;
uint8_t constexpr IMB = 1;

// IMB - Immediate Byte

const char opc_mnemonic_params[0x100][4] = {
    /* 00 BRK */          { END },
    /* 01 ORA ($44, X) */ { IMB, END },
    /* 02 */              {},
    /* 03 */              {},
    /* 04 */              {},
    /* 05 ORA $44 */      { IMB, END },
}