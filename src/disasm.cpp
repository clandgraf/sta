#include <cstdio>
#include <cassert>
#include <string>
#include <map>
#include <memory>

#include "disasm.hpp"

#include "emu.hpp"
#include "rom.hpp"
#include "mem.hpp"
#include "cpu_opcodes.hpp"
#include "cpu_mnemonics.hpp"


using DisasmSegmentSptr = std::shared_ptr<DisasmSegment>;

static size_t constexpr BUFLEN = 0xff;

static std::map<int, const char*> inbuiltLabels = {
    {0x2000, "PPUCTRL"},
    {0x2001, "PPUMASK"},
    {0x2002, "PPUSTATUS"},
    {0x2003, "OAMADDR"},
    {0x2004, "OAMDATA"},
    {0x2005, "PPUSCROLL"},
    {0x2006, "PPUADDR"},
    {0x2007, "PPUDATA"},
    {0x4014, "OAMDMA"},
};

// Branching, Jumps and Returns end an analysis segment
static uint8_t flowBreaking[13] = {
    OPC_BPL, OPC_JSR, OPC_BMI, OPC_RTI, 
    OPC_JMP, OPC_BVC, OPC_RTS, OPC_JMP_IND, 
    OPC_BVS, OPC_BCC, OPC_BCS, OPC_BNE,
    OPC_BEQ
};

static bool isFlowBreaking(uint8_t opcode) {
    for (int i = 0; i < 13; i++) {
        if (flowBreaking[i] == opcode) {
            return true;
        }
    }
    return false;
}

bool Disassembler::translateToCartSpace(uint16_t address) const { 
    return m_translateCartSpace && Memory::isCartSpace(address); 
}

#define _DISASM_APPEND_(...) (bufIdx += snprintf(&buf[bufIdx], BUFLEN - bufIdx, __VA_ARGS__))

const char* Disassembler::disasmOpcode(uint16_t address, bool* end, uint8_t* next) {
    static uint8_t cartBank;
    static uint16_t cartAddress;

    uint8_t opc = m_emu.getOpcode(address);
    Opcode::AddressingMode opc_addressingMode = Opcode::addressingModes[opc];
    uint8_t opc_ac = Opcode::paramCount[opc_addressingMode];

    static char buf[BUFLEN];
    int bufIdx = 0;
    if (translateToCartSpace(address)) {
        m_emu.m_cart->translate_cpu(address, cartBank, cartAddress);
        _DISASM_APPEND_("%02x:%04x : ", cartBank, cartAddress);
    } else {
        _DISASM_APPEND_("   %04x : ", address);
    }

    static uint8_t args[2];
    static uint8_t arg0, arg1;
    switch (opc_addressingMode) {
    case Opcode::Undefined:
    case Opcode::Implicit:
        _DISASM_APPEND_("%02x       : ", opc);
        _DISASM_APPEND_(Opcode::mnemonics[opc]);
        break;
    case Opcode::IndirectX:
    case Opcode::IndirectY:
    case Opcode::Relative:
    case Opcode::ZeroPage:
    case Opcode::ZeroPageX:
    case Opcode::ZeroPageY:
    case Opcode::Immediate:
        arg0 = m_emu.getImmediateArg(address, 0);
        _DISASM_APPEND_("%02x %02x    : ", opc, arg0);
        _DISASM_APPEND_(Opcode::mnemonics[opc]);
        _DISASM_APPEND_(" ");
        _DISASM_APPEND_(Opcode::paramPatterns[opc_addressingMode][0], arg0);
        break;
    case Opcode::Absolute:
    case Opcode::AbsoluteX:
    case Opcode::AbsoluteY:
    case Opcode::Indirect: {
        arg0 = m_emu.getImmediateArg(address, 0);
        arg1 = m_emu.getImmediateArg(address, 1);
        _DISASM_APPEND_("%02x %02x %02x : ", opc, arg0, arg1);
        _DISASM_APPEND_(Opcode::mnemonics[opc]);
        _DISASM_APPEND_(" ");
        uint16_t opcAddress = arg1 << 8 | arg0;
        if (m_showAbsoluteLabels && inbuiltLabels[opcAddress]) {
            _DISASM_APPEND_(Opcode::paramPatterns[opc_addressingMode][1], inbuiltLabels[opcAddress]);
        } else if (translateToCartSpace(opcAddress)) {
            m_emu.m_cart->translate_cpu(opcAddress, cartBank, cartAddress);
            _DISASM_APPEND_(Opcode::paramPatterns[opc_addressingMode][2], cartBank, cartAddress);
        } else {
            _DISASM_APPEND_(Opcode::paramPatterns[opc_addressingMode][0], arg1, arg0);
        }
        break;
    }
    }

    if (next) {
        *next = opc_ac + 1;
    }
    
    if (isFlowBreaking(opc) && end) {
        *end = true;
    }

    return buf;
}

const char* Disassembler::disasmNextOpcode(bool* end, uint8_t* next) {
    return disasmOpcode(m_emu.getOpcodeAddress(), end, next);
}

DisasmSegmentSptr Disassembler::findSegment(uint16_t addr, bool& adjacent) {
    for (auto const& segment: m_disassembly) {
        uint16_t start = segment.first;
        uint16_t end = start + segment.second->m_length;
        if (addr >= start && addr <= end) {
            adjacent = addr == end;
            return segment.second;
        }
    }

    return nullptr;
}

void Disassembler::mergeSegments(DisasmSegmentSptr segment, DisasmSegmentSptr other) {
    segment->m_lines.insert(other->m_lines.begin(), other->m_lines.end());
    segment->m_length += other->m_length;
}

DisasmSegmentSptr Disassembler::disasmSegment(uint16_t addr) {
    bool adjacent;
    DisasmSegmentSptr segment = findSegment(addr, adjacent);
    if (segment) {
        if (!adjacent) {
            return segment;
        }
        // If adjacent, addr starts directly after segment, 
        // so we simply enhance that one
    } else {
        segment = std::make_shared<DisasmSegment>(addr);
        m_disassembly.insert({ addr, segment });
    }
    
    uint8_t next = 0;
    bool end = false;

    do {
        DisasmLine line{
            addr, 
            disasmOpcode(addr, &end, &next) 
        };
        segment->m_lines.insert({line.offset, line});
        addr += next;

        // Check wether current segment is adjacent to an existing segment
        auto nextSegment = m_disassembly.find(addr);
        if (nextSegment != m_disassembly.end()) {
            mergeSegments(segment, nextSegment->second);
            m_disassembly.erase(addr);
            end = true;
        }

    } while (!end);

    segment->m_length = addr - segment->m_start;
    return segment;
}

DisasmSegmentSptr Disassembler::continueSegment(DisasmSegmentSptr segment) {
    return disasmSegment(segment->m_start + segment->m_length);
}

void Disassembler::refresh() {
    for (auto& segment: m_disassembly) {
        for (auto& line: segment.second->m_lines) {
            line.second.repr = disasmOpcode(line.second.offset);
        }
    }
}
