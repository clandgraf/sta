#pragma once

class cart;

extern bool initUi(bool fullscreen = false);
extern bool isWindowClosing();
extern void doUi(cart*);
extern void teardownUi();
