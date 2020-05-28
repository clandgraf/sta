#include <cstdio>
#include <cassert>

#include "disasm.hpp"

#include "emu.hpp"
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

const char* disasmNextOpcode(Emu& emu) {
    static char buf[BUFLEN];
    static uint8_t args[2];
    
    uint16_t addr = emu.getOpcodeAddress();
    int bufIdx = 0;
    bufIdx += snprintf(&buf[bufIdx], BUFLEN - bufIdx, "%04x : ", addr);

    uint8_t opc = emu.getOpcode();

    static uint8_t arg0, arg1;
    switch (opc_mnemonic_params[opc]) {
    case 0: {
        bufIdx += snprintf(&buf[bufIdx], BUFLEN - bufIdx, "%02x       : ", opc);
        bufIdx += snprintf(&buf[bufIdx], BUFLEN - bufIdx, opc_mnemonics[opc]);
        break;
    }
    case 1:
        arg0 = emu.getImmediateArg(0);
        bufIdx += snprintf(&buf[bufIdx], BUFLEN - bufIdx, "%02x %02x    : ", opc, arg0);
        bufIdx += snprintf(&buf[bufIdx], BUFLEN - bufIdx, opc_mnemonics[opc], arg0);
        break;
    case 2:
        arg0 = emu.getImmediateArg(0);
        arg1 = emu.getImmediateArg(1);
        bufIdx += snprintf(&buf[bufIdx], BUFLEN - bufIdx, "%02x %02x %02x : ", opc, arg0, arg1);
        bufIdx += snprintf(&buf[bufIdx], BUFLEN - bufIdx, opc_mnemonics[opc], arg0, arg1);
        break;
    }

    return buf;
}
