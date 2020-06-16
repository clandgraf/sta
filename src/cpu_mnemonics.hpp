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

    const char* paramPatterns[][3] = {
        { nullptr,        nullptr, nullptr        },
        { "$%02x%02x",    "%s",    "%02x:%04x"    },
        { "$%02x%02x, X", "%s, X", "%02x:%04x, X" },
        { "$%02x%02x, Y", "%s, Y", "%02x:%04x, Y" },
        { "($%02x%02)",   "(%s)",  "(%02x:%04x)"  },
        { "($%02x, X)",   nullptr, nullptr        },
        { "($%02x), Y",   nullptr, nullptr        },
        { "%d",           nullptr, nullptr        },
        { "$%02x",        nullptr, nullptr        },
        { "$%02x, X",     nullptr, nullptr        },
        { "$%02x, Y",     nullptr, nullptr        },
        { "#$%02x",       nullptr, nullptr        },
    };

    const char* mnemonics[0x100] = {
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

    static AddressingMode addressingModes[0x100] = {
        Implicit, IndirectX,Undefined,Undefined, Undefined,ZeroPage, ZeroPage, Undefined,    Implicit,Immediate, Implicit, Undefined, Undefined,Absolute,  Absolute,  Undefined,
        Relative, IndirectY,Undefined,Undefined, Undefined,ZeroPageX,ZeroPageX,Undefined,    Implicit,AbsoluteY, Undefined,Undefined, Undefined,AbsoluteX, AbsoluteX, Undefined,
        Absolute, IndirectX,Undefined,Undefined, ZeroPage, ZeroPage, ZeroPage, Undefined,    Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Undefined,
        Relative, IndirectY,Undefined,Undefined, Undefined,ZeroPageX,ZeroPageX,Undefined,    Implicit,AbsoluteY, Undefined,Undefined, Undefined,AbsoluteX, AbsoluteX, Undefined,
        Implicit, IndirectX,Undefined,Undefined, Undefined,ZeroPage, ZeroPage, Undefined,    Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Undefined,
        Relative, IndirectY,Undefined,Undefined, Undefined,ZeroPageX,ZeroPageX,Undefined,    Implicit,AbsoluteY, Undefined,Undefined, Undefined,AbsoluteX, AbsoluteX, Undefined,
        Implicit, IndirectX,Undefined,Undefined, Undefined,ZeroPage, ZeroPage, Undefined,    Implicit,Immediate, Implicit, Undefined, Indirect, Absolute,  Absolute,  Undefined,
        Relative, IndirectY,Undefined,Undefined, Undefined,ZeroPageX,ZeroPageX,Undefined,    Implicit,AbsoluteY, Undefined,Undefined, Undefined,AbsoluteX, AbsoluteX, Undefined,

        Undefined,IndirectX,Undefined,Undefined, ZeroPage, ZeroPage, ZeroPage, Undefined,    Implicit,Undefined, Implicit, Undefined, Absolute, Absolute,  Absolute,  Undefined,
        Relative, IndirectY,Undefined,Undefined, ZeroPageX,ZeroPageX,ZeroPageY,Undefined,    Implicit,AbsoluteY, Implicit, Undefined, Undefined,AbsoluteX, Undefined, Undefined,
        Immediate,IndirectX,Immediate,Undefined, ZeroPage, ZeroPage, ZeroPage, Undefined,    Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Undefined,
        Relative, IndirectY,Undefined,Undefined, ZeroPageX,ZeroPageX,ZeroPageY,Undefined,    Implicit,AbsoluteY, Implicit, Undefined, AbsoluteX,AbsoluteX, AbsoluteY, Undefined,
        Immediate,IndirectX,Undefined,Undefined, ZeroPage, ZeroPage, ZeroPage, Undefined,    Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Undefined,
        Relative, IndirectY,Undefined,Undefined, Undefined,ZeroPageX,ZeroPageX,Undefined,    Implicit,AbsoluteY, Undefined,Undefined, Undefined,AbsoluteX, AbsoluteX, Undefined,
        Immediate,IndirectX,Undefined,Undefined, ZeroPage, ZeroPage, ZeroPage, Undefined,    Implicit,Immediate, Implicit, Undefined, Absolute, Absolute,  Absolute,  Undefined,
        Relative, IndirectY,Undefined,Undefined, Undefined,ZeroPageX,ZeroPageX,Undefined,    Implicit,AbsoluteY, Undefined,Undefined, Undefined,AbsoluteX, AbsoluteX, Undefined,
    };

};