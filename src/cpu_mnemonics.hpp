#pragma once

#include <cstdint>

const char* opc_mnemonics[0x100] = {
 //          0                1                 2             3   4               5                6               7   8      9                   a        b   c                  d                   e                   f
 /* 0 */    "BRK",           "ORA ($%02x, X)", "",           "", "",             "ORA $%02x",     "ASL $%02x",    "", "PHP", "ORA #$%02x",       "ASL A", "", "",                "ORA $%02x%02x",    "ASL $%02x%02x",    "",
 /* 1 */    "BPL %02x",      "ORA ($%02x), Y", "",           "", "",             "ORA $%02x, X",  "ASL $%02x, X", "", "CLC", "ORA $%02x%02x, Y", "",      "", "",                "ORA $%02x%02x, X", "ASL $%02x%02x, X", "",
 /* 2 */    "JSR $%02x%02x", "AND ($%02x, X)", "",           "", "BIT $%02x",    "AND $%02x",     "ROL $%02x",    "", "PLP", "AND #$%02x",       "ROL A", "", "BIT $%02x%02x",   "AND $%02x%02x",    "ROL $%02x%02x",    "",
 /* 3 */    "BMI %02x",      "AND ($%02x), Y", "",           "", "",             "AND $%02x, X",  "ROL $%02x, X", "", "SEC", "AND $%02x%02x, Y", "",      "", "",                "AND $%02x%02x, X", "ROL $%02x%02x, X", "",
 /* 4 */    "RTI",           "EOR ($%02x, X)", "",           "", "",             "EOR $%02x",     "LSR $%02x",    "", "PHA", "EOR #$%02x",       "LSR A", "", "JMP $%02x%02x",   "EOR $%02x%02x",    "LSR $%02x%02x",    "",
 /* 5 */    "BVC %02x",      "EOR ($%02x), Y", "",           "", "",             "EOR $%02x, X",  "LSR $%02x, X", "", "CLI", "EOR $%02x%02x, Y", "",      "", "",                "EOR $%02x%02x, X", "LSR $%02x%02x, X", "",
 /* 6 */    "RTS",           "ADC ($%02x, X)", "",           "", "",             "ADC $%02x",     "ROR $%02x",    "", "PLA", "ADC #$%02x",       "ROR A", "", "JMP ($%02x%02x)", "ADC $%02x%02x",    "ROR $%02x%02x",    "",
 /* 7 */    "BVS %02x",      "ADC ($%02x), Y", "",           "", "",             "ADC $%02x, X",  "ROR $%02x, X", "", "SEI", "ADC $%02x%02x, Y", "",      "", "",                "ADC $%02x%02x, X", "ROR $%02x%02x, X", "",
 
 /* 8 */    "",              "STA ($%02x, X)", "",           "", "STY $%02x",    "STA $%02x",     "STX $%02x",    "", "DEY", "",                 "TXA",   "", "STY $%02x%02x",   "STA $%02x%02x",    "STX $%02x%02x",    "",
 /* 9 */    "BCC %02x",      "STA ($%02x) ,Y", "",           "", "STY $%02x, X", "STA $%02x, X",  "STX $%02x, Y", "", "TYA", "STA $%02x%02x, Y", "TXS",   "", "",                "STA $%02x%02x, X", "",                 "",
 /* A */    "LDY #$%02x",    "LDA ($%02x, X)", "LDX #$%02x", "", "LDY $%02x",    "LDA $%02x",     "LDX $%02x",    "", "TAY", "LDA #$%02x",       "TAX",   "", "LDY $%02x%02x",   "LDA $%02x%02x",    "LDX $%02x%02x",    "",
 /* B */    "BCS %02x",      "LDA ($%02x), Y", "",           "", "LDY $%02x, X", "LDA $%02x, X",  "LDX $%02x, Y", "", "CLV", "LDA $%02x%02x, Y", "TSX",   "", "",                "LDA $%02x%02x, X", "LDX $%02x%02x, Y", "",
 /* C */    "CPY #$%02x",    "CMP ($%02x, X)", "",           "", "CPY $%02x",    "CMP $%02x",     "DEC $%02x",    "", "INY", "CMP #$%02x",       "DEX",   "", "CPY $%02x%02x",   "CMP $%02x%02x",    "DEC $%02x%02x",    "",
 /* D */    "BNE %02x",      "CMP ($%02x), Y", "",           "", "",             "CMP $%02x, X",  "DEC $%02x, X", "", "CLD", "CMP $%02x%02x, Y", "",      "", "",                "CMP $%02x%02x, X", "DEC $%02x%02x, X", "",
 /* E */    "CPX #$%02x",    "SBC ($%02x, X)", "",           "", "CPX $%02x",    "SBC $%02x",     "INC $%02x",    "", "INX", "SBC #$%02x",       "NOP",   "", "CPX $%02x%02x",   "SBC $%02x%02x",    "INC $%02x%02x",    "",
 /* F */    "BEQ %02x",      "SBC ($%02x), Y", "",           "", "",             "SBC $%02x, X",  "INC $%02x, X", "", "SED", "SBC $%02x%02x, Y", "",      "", "",                "SBC $%02x%02x, X", "INC $%02x%02x, X", "",
};

const char opc_mnemonic_params[0x100] = {
    /*      0  1  2  3  4  5  6  7    8  9  a  b  c  d  e  f */

    /* 0 */ 0, 1, 0, 0, 0, 1, 1, 0,   0, 1, 0, 0, 0, 2, 2, 0,
    /* 1 */ 1, 1, 0, 0, 0, 1, 1, 0,   0, 2, 0, 0, 0, 2, 2, 0,
    /* 2 */ 2, 1, 0, 0, 1, 1, 1, 0,   0, 1, 0, 0, 2, 2, 2, 0,
    /* 3 */ 1, 1, 0, 0, 0, 1, 1, 0,   0, 2, 0, 0, 0, 2, 2, 0,
    /* 4 */ 0, 1, 0, 0, 0, 1, 1, 0,   0, 1, 0, 0, 2, 2, 2, 0,
    /* 5 */ 1, 1, 0, 0, 0, 1, 1, 0,   0, 2, 0, 0, 0, 2, 2, 0,
    /* 6 */ 0, 1, 0, 0, 0, 1, 1, 0,   0, 1, 0, 0, 2, 2, 2, 0,
    /* 7 */ 1, 1, 0, 0, 0, 1, 1, 0,   0, 2, 0, 0, 0, 2, 2, 0,

    /* 8 */ 0, 1, 0, 0, 1, 1, 1, 0,   0, 0, 0, 0, 2, 2, 2, 0,
    /* 9 */ 1, 1, 0, 0, 1, 1, 1, 0,   0, 2, 0, 0, 0, 2, 0, 0,
    /* a */ 1, 1, 1, 0, 1, 1, 1, 0,   0, 1, 0, 0, 2, 2, 2, 0,
    /* b */ 1, 1, 0, 0, 1, 1, 1, 0,   0, 2, 0, 0, 0, 2, 2, 0,
    /* c */ 1, 1, 0, 0, 1, 1, 1, 0,   0, 1, 0, 0, 2, 2, 2, 0,
    /* d */ 1, 1, 0, 0, 0, 1, 1, 0,   0, 2, 0, 0, 0, 2, 2, 0,
    /* e */ 1, 1, 0, 0, 1, 1, 1, 0,   0, 1, 0, 0, 2, 2, 2, 0,
    /* f */ 1, 1, 0, 0, 0, 1, 1, 0,   0, 2, 0, 0, 0, 2, 2, 0,
};

enum opc_AddressingMode {  // Additional information applied at runtime
    NONE  = 0,    // Nothing to be done here
    ABS   = 1,    // Use Label Lookup
    ABS_X = 2,
    ABS_Y = 3,
    IND   = 4,    // Read Runtime Address
    IND_X = 5,    // Read Runtime Address
    IND_Y = 6,
    REL   = 7,
    ZER   = 8,
    ZER_X = 9,
    ZER_Y = 10,
    IMD   = 11,
};

const char* opc_mnemonics__[0x100] = {
    //          0      1      2      3      4      5      6      7        8      9      a        b      c      d      e      f
    /* 0 */    "BRK", "ORA", "???", "???", "???", "ORA", "ASL", "???",   "PHP", "ORA", "ASL A", "???", "???", "ORA", "ASL", "???",
    /* 1 */    "BPL", "ORA", "???", "???", "???", "ORA", "ASL", "???",   "CLC", "ORA", "",      "???", "???", "ORA", "ASL", "???",
    /* 2 */    "JSR", "AND", "???", "???", "BIT", "AND", "ROL", "???",   "PLP", "AND", "ROL A", "???", "BIT", "AND", "ROL", "???",
    /* 3 */    "BMI", "AND", "???", "???", "???", "AND", "ROL", "???",   "SEC", "AND", "",      "???", "???", "AND", "ROL", "???",
    /* 4 */    "RTI", "EOR", "???", "???", "???", "EOR", "LSR", "???",   "PHA", "EOR", "LSR A", "???", "JMP", "EOR", "LSR", "???",
    /* 5 */    "BVC", "EOR", "???", "???", "???", "EOR", "LSR", "???",   "CLI", "EOR", "",      "???", "???", "EOR", "LSR", "???",
    /* 6 */    "RTS", "ADC", "???", "???", "???", "ADC", "ROR", "???",   "PLA", "ADC", "ROR A", "???", "JMP", "ADC", "ROR", "???",
    /* 7 */    "BVS", "ADC", "???", "???", "???", "ADC", "ROR", "???",   "SEI", "ADC", "",      "???", "???", "ADC", "ROR", "???",

    /* 8 */    "???", "STA", "???", "???", "STY", "STA", "STX", "???",   "DEY", "???", "TXA",   "???", "STY", "STA", "STX", "???",
    /* 9 */    "BCC", "STA", "???", "???", "STY", "STA", "STX", "???",   "TYA", "STA", "TXS",   "???", "???", "STA", "???", "???",
    /* A */    "LDY", "LDA", "LDX", "???", "LDY", "LDA", "LDX", "???",   "TAY", "LDA", "TAX",   "???", "LDY", "LDA", "LDX", "???",
    /* B */    "BCS", "LDA", "???", "???", "LDY", "LDA", "LDX", "???",   "CLV", "LDA", "TSX",   "???", "???", "LDA", "LDX", "???",
    /* C */    "CPY", "CMP", "???", "???", "CPY", "CMP", "DEC", "???",   "INY", "CMP", "DEX",   "???", "CPY", "CMP", "DEC", "???",
    /* D */    "BNE", "CMP", "???", "???", "???", "CMP", "DEC", "???",   "CLD", "CMP", "???",   "???", "???", "CMP", "DEC", "???",
    /* E */    "CPX", "SBC", "???", "???", "CPX", "SBC", "INC", "???",   "INX", "SBC", "NOP",   "???", "CPX", "SBC", "INC", "???",
    /* F */    "BEQ", "SBC", "???", "???", "???", "SBC", "INC", "???",   "SED", "SBC", "???",   "???", "???", "SBC", "INC", "???",
};

static opc_AddressingMode opc_addressingModes[0x100] = {
    NONE, IND_X, NONE, NONE, NONE,  ZER,   ZER,   NONE,    NONE, IMD,   NONE, NONE, NONE, ABS,   ABS,   NONE,
    REL,  IND_Y, NONE, NONE, NONE,  ZER_X, ZER_X, NONE,    NONE, ABS_Y, NONE, NONE, NONE, ABS_X, ABS_X, NONE,
    ABS,  IND_X, NONE, NONE, ZER,   ZER,   ZER,   NONE,    NONE, IMD,   NONE, NONE, ABS,  ABS,   ABS,   NONE,
    REL,  IND_Y, NONE, NONE, NONE,  ZER_X, ZER_X, NONE,    NONE, ABS_Y, NONE, NONE, NONE, ABS_X, ABS_X, NONE,
    NONE, IND_X, NONE, NONE, NONE,  ZER,   ZER,   NONE,    NONE, IMD,   NONE, NONE, ABS,  ABS,   ABS,   NONE,
    REL,  IND_Y, NONE, NONE, NONE,  ZER_X, ZER_X, NONE,    NONE, ABS_Y, NONE, NONE, NONE, ABS_X, ABS_X, NONE,
    NONE, IND_X, NONE, NONE, NONE,  ZER,   ZER,   NONE,    NONE, IMD,   NONE, NONE, IND,  ABS,   ABS,   NONE,
    REL,  IND_Y, NONE, NONE, NONE,  ZER_X, ZER_X, NONE,    NONE, ABS_Y, NONE, NONE, NONE, ABS_X, ABS_X, NONE,

    NONE, IND_X, NONE, NONE, ZER,   ZER,   ZER,   NONE,    NONE, NONE,  NONE, NONE, ABS,  ABS,   ABS,   NONE,
    REL,  IND_Y, NONE, NONE, ZER_X, ZER_X, ZER_Y, NONE,    NONE, ABS_Y, NONE, NONE, NONE, ABS_X, NONE,  NONE,
    IMD,  IND_X, IMD,  NONE, ZER,   ZER,   ZER,   NONE,    NONE, IMD,   NONE, NONE, ABS,  ABS,   ABS,   NONE,
    REL,  IND_Y, NONE, NONE, ZER_X, ZER_X, ZER_Y, NONE,    NONE, ABS_Y, NONE, NONE, NONE, ABS_X, ABS_Y, NONE,
    IMD,  IND_X, NONE, NONE, ZER,   ZER,   ZER,   NONE,    NONE, IMD,   NONE, NONE, ABS,  ABS,   ABS,   NONE,
    REL,  IND_Y, NONE, NONE, NONE,  ZER_X, ZER_X, NONE,    NONE, ABS_Y, NONE, NONE, NONE, ABS_X, ABS_X, NONE,
    IMD,  IND_X, NONE, NONE, ZER,   ZER,   ZER,   NONE,    NONE, IMD,   NONE, NONE, ABS,  ABS,   ABS,   NONE,
    REL,  IND_Y, NONE, NONE, NONE,  ZER_X, ZER_X, NONE,    NONE, ABS_Y, NONE, NONE, NONE, ABS_X, ABS_X, NONE,
};

/* Patterns for rendering params */
const char* ARG_ABS_PLAIN   = "$%02x%02x";
const char* ARG_ABS_X_PLAIN = "$%02x%02x, X";
const char* ARG_ABS_Y_PLAIN = "$%02x%02x, Y";
const char* ARG_IND_PLAIN   = "($%02x%02)";
const char* ARG_IND_X_PLAIN = "($%02x, X)";
const char* ARG_IND_Y_PLAIN = "($%02x), Y";
const char* ARG_REL_PLAIN   = "%d";
const char* ARG_ZER_PLAIN   = "$%02x";
const char* ARG_ZER_X_PLAIN = "$%02x, X";
const char* ARG_ZER_Y_PLAIN = "$%02x, Y";
const char* ARG_IMD_PLAIN   = "#$%02x";
