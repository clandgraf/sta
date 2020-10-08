#include <cstdlib>
#include <filesystem>
#include <iostream>

#include "core/util.hpp"
#include "core/gui/manager.hpp"
#include "defs.hpp"
#include "rom.hpp"
#include "mem.hpp"
#include "emu.hpp"
#include "disasm.hpp"

namespace fs = std::filesystem;
namespace cli = CliArguments;

void printUsage(const char* prog) {
    std::cout << prog << " [--rom <rom_file>] [--fullscreen] [--help]\n";
}

extern void createDisassembly(Gui::Manager<Emu>& manager);
extern void createPatternTable(Gui::Manager<Emu>& manager);
extern void createEmuState(Gui::Manager<Emu>& manager);
extern void createMemoryViewer(Gui::Manager<Emu>& manager);
extern void createOamViewer(Gui::Manager<Emu>& manager);
extern void createControls(Gui::Manager<Emu>& manager);
extern void createRomInfo(Gui::Manager<Emu>& manager);
extern void createSetupControllers(Gui::Manager<Emu>& manager);

void registerGuiElements(Gui::Manager<Emu>& manager) {
    createDisassembly(manager);
    createPatternTable(manager);
    createEmuState(manager);
    createMemoryViewer(manager);
    createOamViewer(manager);
    createControls(manager);
    createRomInfo(manager);
    createSetupControllers(manager);

    manager.action("File", "Reset", 
                   [](Emu& emu) -> void  { emu.reset(); });
    manager.checkbox("Debugger", "Log State", 
                     [](Emu& emu) -> bool& { return emu.m_breakOnInterrupt; });
    manager.checkbox("Debugger", "Break on Interrupts", 
                     [](Emu& emu) -> bool& { return emu.m_logState; });
    manager.action("Debugger", "Absolute Labels", 
                   [](Emu& emu) -> bool& { return emu.m_disassembler->m_showAbsoluteLabels; }, 
                   [](Emu& emu) -> void  { emu.m_disassembler->refresh(); }
    );
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
        emu.init(romPath);
    }

    Gui::Manager<Emu> manager;
    registerGuiElements(manager);

    if (!manager.init(emu, WINDOW_TITLE, fullscreen)) {
        return EXIT_FAILURE;
    }

    emu.setPixelFn(manager.getSetPixelFn());

    double previousTime = glfwGetTime();
    int frameCount = 0;
    char buffer[64];

    while (!manager.isWindowClosing()) {
        double currentTime = glfwGetTime();
        frameCount++;
        if (currentTime - previousTime >= 1.0)
        {
            snprintf(buffer, 64, "FPS: %d", frameCount);
            manager.setTitle(buffer);

            frameCount = 0;
            previousTime = currentTime;
        }
        
        Gui::pollEvents();

        if (emu.m_isStepping || !emu.isInitialized()) {
            manager.runUi(emu);
        } else {
            manager.runFrame(emu);
        }

        manager.swapBuffers();
    }

    manager.teardown(emu);
    emu.writeSettings();
    Settings::write();
    return EXIT_SUCCESS;
}
