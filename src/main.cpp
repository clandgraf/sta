#include <cstdlib>
#include <iostream>
#include "gui.hpp"
#include "rom.hpp"

int main(int ac, char ** av) {
    if (ac < 2) {
        std::cerr << "Specify a rom file on command line\n";
        return EXIT_FAILURE;
    }

    cart* _cart = new cart(av[1]);

    if (!initUi(false)) {
        return EXIT_FAILURE;
    }

    while (!isWindowClosing()) {
        doUi(_cart);
    }

    teardownUi();
    return EXIT_SUCCESS;
}
