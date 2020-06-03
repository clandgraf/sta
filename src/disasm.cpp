#include <cstdio>
#include <cassert>
#include <string>
#include <map>
#include <memory>

#include "disasm.hpp"

#include "emu.hpp"
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
//
//typedef std::string (*MemoryLabel)(
//    Emu& emu, 
//    uint8_t opc, 
//    uint8_t arg0, uint8_t arg1, 
//    bool current
//);
//
//static std::string Absolute(Emu& emu, uint8_t opc, uint8_t arg0, uint8_t arg1, bool current) {
//     
//}

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

#define _DISASM_APPEND_(...) (bufIdx += snprintf(&buf[bufIdx], BUFLEN - bufIdx, __VA_ARGS__))

const char* Disassembler::disasmOpcode(uint16_t address, bool* end, uint8_t* next) {
    uint8_t opc = m_emu.getOpcode(address);
    uint8_t opc_addressingMode = opc_addressingModes[opc];
    uint8_t opc_ac = opc_mnemonic_params[opc];

    static char buf[BUFLEN];
    int bufIdx = 0;
    _DISASM_APPEND_("%04x : ", address);

    static uint8_t args[2];
    static uint8_t arg0, arg1;
    switch (opc_addressingMode) {
    case NONE:
        _DISASM_APPEND_("%02x       : ", opc);
        _DISASM_APPEND_(opc_mnemonics[opc]);
        break;
    case IND_X:
    case IND_Y:
    case REL:
    case ZER:
    case ZER_X:
    case ZER_Y:
    case IMD:
        arg0 = m_emu.getImmediateArg(address, 0);
        _DISASM_APPEND_("%02x %02x    : ", opc, arg0);
        _DISASM_APPEND_(opc_mnemonics[opc], arg0);
        break;
    case ABS:
    case ABS_X:
    case ABS_Y:
    case IND: {
        arg0 = m_emu.getImmediateArg(address, 0);
        arg1 = m_emu.getImmediateArg(address, 1);
        _DISASM_APPEND_("%02x %02x %02x : ", opc, arg0, arg1);
        _DISASM_APPEND_(opc_mnemonics__[opc]);
        _DISASM_APPEND_(" ");
        uint16_t addr = arg1 << 8 | arg0;
        if (m_showAbsoluteLabels && inbuiltLabels[addr]) {
            _DISASM_APPEND_(opc_paramPatterns[opc_addressingMode][1], inbuiltLabels[addr]);
        } else {
            _DISASM_APPEND_(opc_paramPatterns[opc_addressingMode][0], arg1, arg0);
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

DisasmSegmentSptr Disassembler::findSegment(uint16_t addr) {
    for (auto const& segment: m_disassembly) {
        uint16_t start = segment.first;
        uint16_t end = segment.first + segment.second->m_length;
        if (addr >= start && addr < end) {
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
    DisasmSegmentSptr segment = findSegment(addr);
    if (segment) {
        return segment;
    }

    segment = std::make_shared<DisasmSegment>(addr);
    m_disassembly.insert({addr, segment});

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
