#pragma once

#include <cstdint>

namespace Opcode {

    enum AddressingMode {  // Additional information applied at runtime
        Implicit = 0,    // Nothing to be done here
        Absolute = 1,    // Use Label Lookup
        AbsoluteX = 2,
        AbsoluteY = 3,
        Indirect   = 4,    // Read Runtime Address
        IndirectX = 5,    // Read Runtime Address
        IndirectY = 6,
        Relative   = 7,
        ZeroPage   = 8,
        ZeroPageX = 9,
        ZeroPageY = 10,
        Immediate = 11,
        Undefined = 12,
    };

    const uint8_t paramCount[] = {
        0, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1,
    };

    #define _X8   "%02X"
    #define _X16  "%04X"
    #define _S    "%s"

    const char* paramPatterns[][3] = {
        { nullptr,        nullptr,    nullptr        },
        { "$" _X8 _X8,    _S,         _X8 ":" _X16   },
        { "$" _X8 _X8 ", X", "%s, X",    _X8 ":" _X16 ", X" },
        { "$%02X%02X, Y", "%s, Y",    "%02X:%04X, Y" },
        { "($%02X%02X)",  "(" _S ")", "(%02X:%04X)"  },
        { "($%02X, X)",   nullptr,    nullptr        },
        { "($%02X), Y",   nullptr,    nullptr        },
        { "%d",           "$" _X16,   _X8 ":" _X16   },
        { "$%02X",        nullptr,    nullptr        },
        { "$%02X, X",     nullptr,    nullptr        },
        { "$%02X, Y",     nullptr,    nullptr        },
        { "#$%02X",       nullptr,    nullptr        },
    };

    const char* mnemonics[0x100] = {
        //          0       1      2       3      4      5       6      7         8      9       a        b       c      d      e      f
        /* 0 */    "BRK",  "ORA", "???", "*SLO", "*NOP", "ORA", "ASL", "*SLO",   "PHP", "ORA",  "ASL A", "???",  "*NOP","ORA", "ASL", "*SLO",
        /* 1 */    "BPL",  "ORA", "???", "*SLO", "*NOP", "ORA", "ASL", "*SLO",   "CLC", "ORA",  "*NOP",  "*SLO", "*NOP","ORA", "ASL", "*SLO",
        /* 2 */    "JSR",  "AND", "???", "*RLA", "BIT",  "AND", "ROL","*RLA",    "PLP", "AND",  "ROL A", "???",  "BIT", "AND", "ROL", "*RLA",
        /* 3 */    "BMI",  "AND", "???", "*RLA", "*NOP", "AND", "ROL","*RLA",    "SEC", "AND",  "*NOP",  "*RLA", "*NOP","AND", "ROL", "*RLA",
        /* 4 */    "RTI",  "EOR", "???", "*SRE", "*NOP", "EOR", "LSR", "*SRE",    "PHA", "EOR",  "LSR A", "???",  "JMP", "EOR", "LSR", "*SRE",
        /* 5 */    "BVC",  "EOR", "???", "*SRE", "*NOP", "EOR", "LSR", "*SRE",    "CLI", "EOR",  "*NOP",  "*SRE",  "*NOP","EOR", "LSR", "*SRE",
        /* 6 */    "RTS",  "ADC", "???", "*RRA", "*NOP", "ADC", "ROR", "*RRA",    "PLA", "ADC",  "ROR A", "???",  "JMP", "ADC", "ROR", "*RRA",
        /* 7 */    "BVS",  "ADC", "???", "*RRA", "*NOP", "ADC", "ROR", "*RRA",    "SEI", "ADC",  "*NOP",  "*RRA",  "*NOP","ADC", "ROR", "*RRA",

        /* 8 */    "*NOP", "STA", "*NOP","*AXS","STY",  "STA", "STX", "*AXS",   "DEY", "*NOP", "TXA",   "???",  "STY", "STA", "STX", "*AXS",
        /* 9 */    "BCC",  "STA", "???", "???", "STY",  "STA", "STX", "*AXS",   "TYA", "STA",  "TXS",   "???",  "???", "STA", "???", "???",
        /* A */    "LDY",  "LDA", "LDX", "*LAX","LDY",  "LDA", "LDX", "*LAX",   "TAY", "LDA",  "TAX",   "???",  "LDY", "LDA", "LDX", "*LAX",
        /* B */    "BCS",  "LDA", "???", "*LAX","LDY",  "LDA", "LDX", "*LAX",   "CLV", "LDA",  "TSX",   "???",  "LDY", "LDA", "LDX", "*LAX",
        /* C */    "CPY",  "CMP", "???", "*DCM","CPY",  "CMP", "DEC", "*DCM",   "INY", "CMP",  "DEX",   "???",  "CPY", "CMP", "DEC", "*DCM",
        /* D */    "BNE",  "CMP", "???", "*DCM","*NOP", "CMP", "DEC", "*DCM",   "CLD", "CMP",  "*NOP",  "*DCM", "*NOP","CMP", "DEC", "*DCM",
        /* E */    "CPX",  "SBC", "???", "*INS", "CPX",  "SBC", "INC", "*INS",   "INX", "SBC",  "NOP",   "*SBC", "CPX", "SBC", "INC", "*INS",
        /* F */    "BEQ",  "SBC", "???", "*INS", "*NOP", "SBC", "INC", "*INS",   "SED", "SBC",  "*NOP",  "*INS",  "*NOP","SBC","INC", "*INS",
    };

    static AddressingMode addressingModes[0x100] = {
        //          0         1         2         3          4         5         6          7             8        9          a         b         c        d            e        f
        /* 0 */  Implicit, IndirectX,Undefined,IndirectX, ZeroPage, ZeroPage, ZeroPage, ZeroPage,     Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Absolute,
        /* 1 */  Relative, IndirectY,Undefined,IndirectY, ZeroPageX,ZeroPageX,ZeroPageX,ZeroPageX,    Implicit,AbsoluteY, Implicit, AbsoluteY, AbsoluteX,AbsoluteX, AbsoluteX, AbsoluteX,
        /* 2 */  Absolute, IndirectX,Undefined,IndirectX, ZeroPage, ZeroPage, ZeroPage, ZeroPage,     Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Absolute,
        /* 3 */  Relative, IndirectY,Undefined,IndirectY, ZeroPageX,ZeroPageX,ZeroPageX,ZeroPageX,    Implicit,AbsoluteY, Implicit, AbsoluteY, AbsoluteX,AbsoluteX, AbsoluteX, AbsoluteX,
        /* 4 */  Implicit, IndirectX,Undefined,IndirectX, ZeroPage, ZeroPage, ZeroPage, ZeroPage,     Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Absolute,
        /* 5 */  Relative, IndirectY,Undefined,IndirectY, ZeroPageX,ZeroPageX,ZeroPageX,ZeroPageX,    Implicit,AbsoluteY, Implicit, AbsoluteY, AbsoluteX,AbsoluteX, AbsoluteX, AbsoluteX,
        /* 6 */  Implicit, IndirectX,Undefined,IndirectX, ZeroPage, ZeroPage, ZeroPage, ZeroPage,     Implicit,Immediate, Implicit, Undefined, Indirect, Absolute,  Absolute,  Absolute,
        /* 7 */  Relative, IndirectY,Undefined,IndirectY, ZeroPageX,ZeroPageX,ZeroPageX,ZeroPageX,    Implicit,AbsoluteY, Implicit, AbsoluteY, AbsoluteX,AbsoluteX, AbsoluteX, AbsoluteX,

        /* 8 */  Immediate,IndirectX,Immediate,IndirectX, ZeroPage, ZeroPage, ZeroPage, ZeroPage,     Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Absolute,
        /* 9 */  Relative, IndirectY,Undefined,Undefined, ZeroPageX,ZeroPageX,ZeroPageY,ZeroPageY,    Implicit,AbsoluteY, Implicit, Undefined, Undefined,AbsoluteX, Undefined, Undefined,
        /* A */  Immediate,IndirectX,Immediate,IndirectX, ZeroPage, ZeroPage, ZeroPage, ZeroPage,     Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Absolute,
        /* B */  Relative, IndirectY,Undefined,IndirectY, ZeroPageX,ZeroPageX,ZeroPageY,ZeroPageY,    Implicit,AbsoluteY, Implicit, Undefined, AbsoluteX,AbsoluteX, AbsoluteY, AbsoluteY,
        /* C */  Immediate,IndirectX,Immediate,IndirectX, ZeroPage, ZeroPage, ZeroPage, ZeroPage,     Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Absolute,
        /* D */  Relative, IndirectY,Undefined,IndirectY, ZeroPageX,ZeroPageX,ZeroPageX,ZeroPageX,    Implicit,AbsoluteY, Implicit, AbsoluteY, AbsoluteX,AbsoluteX, AbsoluteX, AbsoluteX,
        /* E */  Immediate,IndirectX,Immediate,IndirectX, ZeroPage, ZeroPage, ZeroPage, ZeroPage,     Implicit,Immediate, Implicit, Immediate, Absolute, Absolute,  Absolute,  Absolute,
        /* F */  Relative, IndirectY,Undefined,IndirectY, ZeroPageX,ZeroPageX,ZeroPageX,ZeroPageX,    Implicit,AbsoluteY, Implicit, AbsoluteY, AbsoluteX,AbsoluteX, AbsoluteX, AbsoluteX,
    };

};