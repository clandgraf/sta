﻿#include <cstdlib>
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

    Settings::read();

    Emu emu;
    if (romPath) {
        Cart* cart = Cart::fromFile(fs::path{romPath});
        if (cart) {
            emu.init(cart);
        }
    }

    Disassembler disasm(emu);
    #ifdef LOG_EXECUTION
    emu.setDisassembler(&disasm);
    #endif

    if (!Gui::initUi(fullscreen)) {
        return EXIT_FAILURE;
    }

    while (!Gui::isWindowClosing()) {
        Gui::pollEvents();

        if (emu.m_isStepping || !emu.isInitialized()) {
            Gui::runUi(emu, disasm);
        } else {
            Gui::runFrame(emu);
        }

        Gui::swapBuffers();
    }

    Gui::teardownUi();
    emu.writeSettings();
    disasm.writeSettings();
    Settings::write();
    return EXIT_SUCCESS;
}
