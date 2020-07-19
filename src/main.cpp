#include <cstdlib>
#include <filesystem>
#include <iostream>

#include "gui.hpp"
#include "rom.hpp"
#include "mem.hpp"
#include "emu.hpp"
#include "disasm.hpp"
#include "util.hpp"

PACK(union OamEntry {
    struct {
        uint8_t y;
        uint8_t tileIndex;
        union {
            uint8_t attributes;
            struct {
                uint8_t palette : 2;
                uint8_t __unused : 3;
                uint8_t priority : 1;
                uint8_t hflip : 1;
                uint8_t vflip : 1;
            };
        };
        uint8_t x;
    };
    uint8_t fields[4];
});

void test() {
    LOG_ERR << sizeof(OamEntry) << "\n";

    OamEntry e;
    e.fields[0];
}

namespace fs = std::filesystem;
namespace cli = CliArguments;

void printUsage(const char* prog) {
    std::cout << prog << " [--rom <rom_file>] [--fullscreen] [--help]\n";
}

int main(int ac, char ** av) {

    //test();
    //return 0;

    const char* romPath = cli::value(ac, av, "--rom");
    bool fullscreen = cli::flag(ac, av, "--fullscreen");
    bool help = cli::flag(ac, av, "--help");

    if (help) {
        printUsage(av[0]);
        return EXIT_SUCCESS;
    }

    Settings::read();

    Emu emu;
    if (romPath) {
        std::shared_ptr<Cart> cart = Cart::fromFile(fs::path{romPath});
        if (cart) {
            emu.init(cart);
        }
    }

    if (!Gui::initUi(fullscreen)) {
        return EXIT_FAILURE;
    }

    emu.setPixelFn(Gui::getSetPixelFn());

    while (!Gui::isWindowClosing()) {
        Gui::pollEvents();

        if (emu.m_isStepping || !emu.isInitialized()) {
            Gui::runUi(emu);
        } else {
            Gui::runFrame(emu);
        }

        Gui::swapBuffers();
    }

    Gui::teardownUi();
    emu.writeSettings();
    Settings::write();
    return EXIT_SUCCESS;
}
