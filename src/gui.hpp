#pragma once

class Emu;
class Inputs;
class Disassembler;

namespace Gui {
    extern bool initUi(bool fullscreen = false);
    extern bool isWindowClosing();
    extern void pollEvents();
    extern void setPixel(int x, int y, int v);
    extern void runFrame(Emu&);
    extern void runUi(Emu&);
    extern void swapBuffers();
    extern void teardownUi();
}
