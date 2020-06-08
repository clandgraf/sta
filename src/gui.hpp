#pragma once

class Emu;
class Disassembler;

namespace Gui {
    extern bool initUi(bool fullscreen = false);
    extern bool isWindowClosing();
    extern void pollEvents();
    extern void runUi(Emu&, Disassembler&);
    extern void teardownUi();
}
