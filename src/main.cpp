﻿#include <cstdlib>
#include <iostream>
#include "gui.hpp"
#include "rom.hpp"
#include "mem.hpp"
#include "emu.hpp"
#include "clargs.hpp"

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

    emu _emu;
    if (romPath) {
        cart* _cart = cart::fromFile(romPath);
        if (_cart) {
            _emu.init(_cart);
        }
    }

    if (!initUi(fullscreen)) {
        return EXIT_FAILURE;
    }

    while (!isWindowClosing()) {
        doUi(_emu);
    }

    teardownUi();
    return EXIT_SUCCESS;
}
