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

static size_t constexpr BUFLEN = 0xff;

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

const char* disasmOpcode(Emu& emu, uint16_t address, bool* end = nullptr, uint8_t* next = nullptr) {
    uint8_t opc = emu.getOpcode(address);
    uint8_t opc_ac = opc_mnemonic_params[opc];

    static char buf[BUFLEN];
    int bufIdx = 0;
    bufIdx += snprintf(&buf[bufIdx], BUFLEN - bufIdx, "%04x : ", address);

    static uint8_t args[2];
    static uint8_t arg0, arg1;
    switch (opc_ac) {
    case 0: {
        bufIdx += snprintf(&buf[bufIdx], BUFLEN - bufIdx, "%02x       : ", opc);
        bufIdx += snprintf(&buf[bufIdx], BUFLEN - bufIdx, opc_mnemonics[opc]);
        break;
    }
    case 1:
        arg0 = emu.getImmediateArg(address, 0);
        bufIdx += snprintf(&buf[bufIdx], BUFLEN - bufIdx, "%02x %02x    : ", opc, arg0);
        bufIdx += snprintf(&buf[bufIdx], BUFLEN - bufIdx, opc_mnemonics[opc], arg0);
        break;
    case 2:
        arg0 = emu.getImmediateArg(address, 0);
        arg1 = emu.getImmediateArg(address, 1);
        bufIdx += snprintf(&buf[bufIdx], BUFLEN - bufIdx, "%02x %02x %02x : ", opc, arg0, arg1);
        bufIdx += snprintf(&buf[bufIdx], BUFLEN - bufIdx, opc_mnemonics[opc], arg0, arg1);
        break;
    }

    if (next) {
        *next = opc_ac + 1;
    }
    
    if (isFlowBreaking(opc) && end) {
        *end = true;
    }

    return buf;
}

const char* disasmNextOpcode(Emu& emu, bool* end, uint8_t* next) {
    return disasmOpcode(emu, emu.getOpcodeAddress(), end, next);
}

std::map<uint16_t, std::shared_ptr<DisasmSegment>> disassembly;

std::shared_ptr<DisasmSegment> 
findSegment(uint16_t addr) {
    for (auto const& segment: disassembly) {
        uint16_t start = segment.first;
        uint16_t end = segment.first + segment.second->length;
        if (addr >= start && addr < end) {
            return segment.second;
        }
    }

    return nullptr;
}

std::shared_ptr<DisasmSegment> disasmSegment(Emu& emu, uint16_t addr) {
    std::shared_ptr<DisasmSegment> segment = findSegment(addr);
    if (segment) {
        return segment;
    }

    segment = std::shared_ptr<DisasmSegment>(new DisasmSegment());

    uint8_t next = 0;
    bool end = false;

    do {
        DisasmLine line;
        line.offset = addr += next;
        line.repr = disasmOpcode(emu, addr, &end, &next);
        segment->lines.insert({line.offset, line});
    } while (!end);

    segment->length = addr - segment->start;
    return segment;
}
