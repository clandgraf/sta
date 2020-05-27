#pragma once

const char* opc_mnemonics[0x100] = {
 //          0                1                 2             3   4               5                6               7   8      9             a        b   c   d            e              f
 /* 0 */    "BRK",           "ORA ($%02x, X)", "",           "", "",             "ORA $%02x",     "ASL $%02x",    "", "PHP", "ORA #$%02x", "ASL A", "", "", "ORA $4400", "ASL $%02x00", "",
 /* 1 */    "BPL %02x",      "ORA ($%02x), Y", "",           "", "",             "ORA $%02x, X",  "ASL $%02x, X", "", "CLC", "", "", "", "", "", "", "",
 /* 2 */    "JSR $%02x%02x", "AND ($%02x, X)", "",           "", "BIT $%02x",    "AND $%02x",     "ROL $%02x",    "", "PLP", "", "", "", "", "", "", "",
 /* 3 */    "BMI %02x",      "AND ($%02x), Y", "",           "", "",             "AND $%02x, X",  "ROL $%02x, X", "", "SEC", "", "", "", "", "", "", "",
 /* 4 */    "RTI",           "EOR ($%02x, X)", "",           "", "",             "EOR $%02x",     "LSR $%02x",    "", "PHA", "", "", "", "", "", "", "",
 /* 5 */    "BVC %02x",      "EOR ($%02x), Y", "",           "", "",             "EOR $%02x, X",  "LSR $%02x, X", "", "CLI", "", "", "", "", "", "", "",
 /* 6 */    "RTS",           "ADC ($%02x, X)", "",           "", "",             "ADC $%02x",     "ROR $%02x",    "", "PLA", "", "", "", "", "", "", "",
 /* 7 */    "BVS %02x",      "ADC ($%02x), Y", "",           "", "",             "ADC $%02x, X",  "ROR $%02x, X", "", "SEI", "", "", "", "", "", "", "",
 /* 8 */    "",              "STA ($%02x, X)", "",           "", "STY $%02x",    "STA $%02x",     "STX $%02x",    "", "DEY", "", "", "", "", "", "", "",
 /* 9 */    "BCC %02x",      "STA ($%02x) ,Y", "",           "", "STY $%02x, X", "STA $%02x, X",  "STX $%02x, Y", "", "TYA", "", "", "", "", "", "", "",
 /* A */    "LDY #$%02x",    "LDA ($%02x, X)", "LDX #$%02x", "", "LDY $%02x",    "LDA $%02x",     "LDX $%02x",    "", "TAY", "", "", "", "", "", "", "",
 /* B */    "BCS %02x",      "LDA ($%02x), Y", "",           "", "LDY $%02x, X", "LDA $%02x, X",  "LDX $%02x, Y", "", "CLV", "", "", "", "", "", "", "",
 /* C */    "CPY #$%02x",    "CMP ($%02x, X)", "",           "", "CPY $%02x",    "CMP $%02x",     "DEC $%02x",    "", "INY", "", "", "", "", "", "", "",
 /* D */    "BNE %02x",      "CMP ($%02x), Y", "",           "", "",             "CMP $%02x, X",  "DEC $%02x, X", "", "CLD", "", "", "", "", "", "", "",
 /* E */    "CPX #$%02x",    "SBC ($%02x, X)", "",           "", "CPX $%02x",    "SBC $%02x",     "INC $%02x",    "", "INX", "", "", "", "", "", "", "",
 /* F */    "BEQ %02x",      "SBC ($%02x), Y", "",           "", "",             "SBC $%02x, X",  "INC $%02x, X", "", "SED", "", "", "", "", "", "", "",
};
