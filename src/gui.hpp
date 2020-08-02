#pragma once

#include <string>

class Emu;
class Inputs;
class Disassembler;

typedef void(*SetPixelFn)(unsigned int x, unsigned int y, unsigned int v);

namespace Gui {
    extern bool initUi(Emu& emu, bool fullscreen = false);
    extern void setTitle(const char* title);
    extern bool isWindowClosing();
    extern void pollEvents();
    extern SetPixelFn getSetPixelFn();
    extern void runFrame(Emu&);
    extern void runUi(Emu&);
    extern void swapBuffers();
    extern void teardownUi(Emu& emu);

    extern void pushMonoFont();
    extern void pushHighlightText();

    extern void addNotification(const std::string&);
}
