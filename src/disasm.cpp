#include <cstdio>

#include "disasm.hpp"

#include "emu.hpp"
#include "cpu_mnemonics.hpp"

const char* disasmNextOpcode(Emu& emu) {
    static char buffer[0xff];
    static uint8_t args[4];
    uint16_t addr = emu.getOpcodeAddress();
    uint8_t opc = emu.getOpcode();

    int i = 0;
    for (; opc_mnemonic_params[opc][i] != END; i++) {
        switch (opc_mnemonic_params[opc][i]) {
        case IMB:
            args[i] = emu.getImmediateArg(i);
            break;
        }
    }

    switch (i) {
    case 0:
        snprintf(buffer, 0xff - 1, opc_mnemonics[opc]);
        break;
    case 1:
        snprintf(buffer, 0xff - 1, opc_mnemonics[opc], args[0]);
        break;
    case 2:
        snprintf(buffer, 0xff - 1, opc_mnemonics[opc], args[0], args[1]);
        break;
    case 3:
        snprintf(buffer, 0xff - 1, opc_mnemonics[opc], args[0], args[1], args[2]);
        break;
    case 4:
        snprintf(buffer, 0xff - 1, opc_mnemonics[opc], args[0], args[1], args[2], args[3]);
        break;
    }

    return buffer;
}
