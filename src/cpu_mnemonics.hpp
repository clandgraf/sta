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
        { "$%02X%02X, X", "%s, X",    _X8 ":" _X16 ", X" },
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
        //          0       1      2       3      4      5       6      7        8      9       a        b      c      d      e      f
        /* 0 */    "BRK",  "ORA", "???",  "???", "*NOP", "ORA", "ASL", "???",   "PHP", "ORA",  "ASL A", "???", "*NOP","ORA", "ASL", "???",
        /* 1 */    "BPL",  "ORA", "???",  "???", "*NOP", "ORA", "ASL", "???",   "CLC", "ORA",  "*NOP",  "???", "*NOP","ORA", "ASL", "???",
        /* 2 */    "JSR",  "AND", "???",  "???", "BIT",  "AND", "ROL", "???",   "PLP", "AND",  "ROL A", "???", "BIT", "AND", "ROL", "???",
        /* 3 */    "BMI",  "AND", "???",  "???", "*NOP", "AND", "ROL", "???",   "SEC", "AND",  "*NOP",  "???", "*NOP","AND", "ROL", "???",
        /* 4 */    "RTI",  "EOR", "???",  "???", "*NOP", "EOR", "LSR", "???",   "PHA", "EOR",  "LSR A", "???", "JMP", "EOR", "LSR", "???",
        /* 5 */    "BVC",  "EOR", "???",  "???", "*NOP", "EOR", "LSR", "???",   "CLI", "EOR",  "*NOP",  "???", "*NOP","EOR", "LSR", "???",
        /* 6 */    "RTS",  "ADC", "???",  "???", "*NOP", "ADC", "ROR", "???",   "PLA", "ADC",  "ROR A", "???", "JMP", "ADC", "ROR", "???",
        /* 7 */    "BVS",  "ADC", "???",  "???", "*NOP", "ADC", "ROR", "???",   "SEI", "ADC",  "*NOP",  "???", "*NOP","ADC", "ROR", "???",

        /* 8 */    "*NOP", "STA", "*NOP", "???", "STY",  "STA", "STX", "???",   "DEY", "*NOP", "TXA",   "???", "STY", "STA", "STX", "???",
        /* 9 */    "BCC",  "STA", "???",  "???", "STY",  "STA", "STX", "???",   "TYA", "STA",  "TXS",   "???", "???", "STA", "???", "???",
        /* A */    "LDY",  "LDA", "LDX",  "???", "LDY",  "LDA", "LDX", "???",   "TAY", "LDA",  "TAX",   "???", "LDY", "LDA", "LDX", "???",
        /* B */    "BCS",  "LDA", "???",  "???", "LDY",  "LDA", "LDX", "???",   "CLV", "LDA",  "TSX",   "???", "LDY", "LDA", "LDX", "???",
        /* C */    "CPY",  "CMP", "???",  "???", "CPY",  "CMP", "DEC", "???",   "INY", "CMP",  "DEX",   "???", "CPY", "CMP", "DEC", "???",
        /* D */    "BNE",  "CMP", "???",  "???", "*NOP", "CMP", "DEC", "???",   "CLD", "CMP",  "*NOP",  "???", "*NOP","CMP", "DEC", "???",
        /* E */    "CPX",  "SBC", "???",  "???", "CPX",  "SBC", "INC", "???",   "INX", "SBC",  "NOP",   "???", "CPX", "SBC", "INC", "???",
        /* F */    "BEQ",  "SBC", "???",  "???", "*NOP",  "SBC", "INC", "???",   "SED", "SBC", "*NOP",  "???", "*NOP","SBC", "INC", "???",
    };

    static AddressingMode addressingModes[0x100] = {
        //          0         1         2         3          4         5         6          7             8        9          a         b         c        d            e        f
        /* 0 */  Implicit, IndirectX,Undefined,Undefined, ZeroPage, ZeroPage, ZeroPage, Undefined,    Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Undefined,
        /* 1 */  Relative, IndirectY,Undefined,Undefined, ZeroPageX,ZeroPageX,ZeroPageX,Undefined,    Implicit,AbsoluteY, Implicit, Undefined, AbsoluteX,AbsoluteX, AbsoluteX, Undefined,
        /* 2 */  Absolute, IndirectX,Undefined,Undefined, ZeroPage, ZeroPage, ZeroPage, Undefined,    Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Undefined,
        /* 3 */  Relative, IndirectY,Undefined,Undefined, ZeroPageX,ZeroPageX,ZeroPageX,Undefined,    Implicit,AbsoluteY, Implicit, Undefined, AbsoluteX,AbsoluteX, AbsoluteX, Undefined,
        /* 4 */  Implicit, IndirectX,Undefined,Undefined, ZeroPage, ZeroPage, ZeroPage, Undefined,    Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Undefined,
        /* 5 */  Relative, IndirectY,Undefined,Undefined, ZeroPageX,ZeroPageX,ZeroPageX,Undefined,    Implicit,AbsoluteY, Implicit, Undefined, AbsoluteX,AbsoluteX, AbsoluteX, Undefined,
        /* 6 */  Implicit, IndirectX,Undefined,Undefined, ZeroPage, ZeroPage, ZeroPage, Undefined,    Implicit,Immediate, Implicit, Undefined, Indirect, Absolute,  Absolute,  Undefined,
        /* 7 */  Relative, IndirectY,Undefined,Undefined, ZeroPageX,ZeroPageX,ZeroPageX,Undefined,    Implicit,AbsoluteY, Implicit, Undefined, AbsoluteX,AbsoluteX, AbsoluteX, Undefined,

        /* 8 */  Immediate,IndirectX,Immediate,Undefined, ZeroPage, ZeroPage, ZeroPage, Undefined,    Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Undefined,
        /* 9 */  Relative, IndirectY,Undefined,Undefined, ZeroPageX,ZeroPageX,ZeroPageY,Undefined,    Implicit,AbsoluteY, Implicit, Undefined, Undefined,AbsoluteX, Undefined, Undefined,
        /* A */  Immediate,IndirectX,Immediate,Undefined, ZeroPage, ZeroPage, ZeroPage, Undefined,    Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Undefined,
        /* B */  Relative, IndirectY,Undefined,Undefined, ZeroPageX,ZeroPageX,ZeroPageY,Undefined,    Implicit,AbsoluteY, Implicit, Undefined, AbsoluteX,AbsoluteX, AbsoluteY, Undefined,
        /* C */  Immediate,IndirectX,Immediate,Undefined, ZeroPage, ZeroPage, ZeroPage, Undefined,    Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Undefined,
        /* D */  Relative, IndirectY,Undefined,Undefined, ZeroPageX,ZeroPageX,ZeroPageX,Undefined,    Implicit,AbsoluteY, Implicit, Undefined, AbsoluteX,AbsoluteX, AbsoluteX, Undefined,
        /* E */  Immediate,IndirectX,Immediate,Undefined, ZeroPage, ZeroPage, ZeroPage, Undefined,    Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Undefined,
        /* F */  Relative, IndirectY,Undefined,Undefined, ZeroPageX,ZeroPageX,ZeroPageX,Undefined,    Implicit,AbsoluteY, Implicit, Undefined, AbsoluteX,AbsoluteX, AbsoluteX, Undefined,
    };

};