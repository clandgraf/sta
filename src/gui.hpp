#pragma once

class Emu;
class Disassembler;

extern bool initUi(bool fullscreen = false);
extern bool isWindowClosing();
extern void doUi(Emu&, Disassembler&);
extern void teardownUi();
