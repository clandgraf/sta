#pragma once

class Emu;

extern bool initUi(bool fullscreen = false);
extern bool isWindowClosing();
extern void doUi(Emu&);
extern void teardownUi();
