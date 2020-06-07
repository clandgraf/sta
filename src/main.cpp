#include <cstdlib>
#include <filesystem>
#include <iostream>
#include "gui.hpp"
#include "rom.hpp"
#include "mem.hpp"
#include "emu.hpp"
#include "disasm.hpp"
#include "clargs.hpp"

namespace fs = std::filesystem;

void printUsage(const char* prog) {
    std::cout << prog << " [--rom <rom_file>] [--fullscreen] [--help]\n";
}

int main(int ac, char ** av) {
    const char* romPath = cliValue(ac, av, "--rom");
    bool fullscreen = cliSwitch(ac, av, "--fullscreen");
    bool help = cliSwitch(ac, av, "--help");

    if (help) {
        printUsage(av[0]);
        return EXIT_SUCCESS;
    }

    Emu emu;
    Disassembler disasm(emu);
    if (romPath) {
        Cart* cart = Cart::fromFile(fs::path{romPath});
        if (cart) {
            emu.init(cart);
        }
    }

    if (!initUi(fullscreen)) {
        return EXIT_FAILURE;
    }

    while (!isWindowClosing()) {
        doUi(emu, disasm);
    }

    teardownUi();
    return EXIT_SUCCESS;
}
