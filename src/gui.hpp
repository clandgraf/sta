#pragma once

class emu;

extern bool initUi(bool fullscreen = false);
extern bool isWindowClosing();
extern void doUi(emu&);
extern void teardownUi();
