#include <GLFW/glfw3.h>
#include <cstdlib>
#include <filesystem>
#include <iostream>

#include "gui.hpp"
#include "rom.hpp"
#include "mem.hpp"
#include "emu.hpp"
#include "disasm.hpp"
#include "util.hpp"

namespace fs = std::filesystem;
namespace cli = CliArguments;

void printUsage(const char* prog) {
    std::cout << prog << " [--rom <rom_file>] [--fullscreen] [--help]\n";
}

extern void createDisassembly();
extern void createPatternTable();
extern void createEmuState();
extern void createMemoryViewer();
extern void createOamViewer();
extern void createControls();
extern void createRomInfo();

void registerGuiElements() {
    createDisassembly();
    createPatternTable();
    createEmuState();
    createMemoryViewer();
    createOamViewer();
    createControls();
    createRomInfo();
}

int main(int ac, char ** av) {
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

    registerGuiElements();

    if (!Gui::initUi(emu, fullscreen)) {
        return EXIT_FAILURE;
    }

    emu.setPixelFn(Gui::getSetPixelFn());

    double previousTime = glfwGetTime();
    int frameCount = 0;
    char buffer[64];

    while (!Gui::isWindowClosing()) {
        double currentTime = glfwGetTime();
        frameCount++;
        if (currentTime - previousTime >= 1.0)
        {
            snprintf(buffer, 64, "FPS: %d", frameCount);
            Gui::setTitle(buffer);

            frameCount = 0;
            previousTime = currentTime;
        }
        
        Gui::pollEvents();

        if (emu.m_isStepping || !emu.isInitialized()) {
            Gui::runUi(emu);
        } else {
            Gui::runFrame(emu);
        }

        Gui::swapBuffers();
    }

    Gui::teardownUi(emu);
    emu.writeSettings();
    Settings::write();
    return EXIT_SUCCESS;
}
