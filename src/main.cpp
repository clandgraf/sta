#include <cstdlib>
#include <iostream>
#include "gui.hpp"
#include "rom.hpp"
#include "clargs.hpp"

int main(int ac, char ** av) {
    const char* romPath = cliValue(ac, av, "-rom");
    bool fullscreen = cliSwitch(ac, av, "-fullscreen");

    cart* _cart = romPath ? new cart(romPath) : nullptr;

    if (!initUi(fullscreen)) {
        return EXIT_FAILURE;
    }

    while (!isWindowClosing()) {
        doUi(&_cart);
    }

    teardownUi();
    return EXIT_SUCCESS;
}
