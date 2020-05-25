#include <cstring>

#include "clargs.hpp"

int _cliIndex(int ac, char** av, const char* param) {
    for (int i = 1; i < ac; i++) {
        if (strcmp(av[i], param) == 0) {
            return i;
        }
    }

    return 0;
}

bool cliSwitch(int ac, char** av, const char* param) {
    return _cliIndex(ac, av, param) > 0;
}

char* cliValue(int ac, char** av, const char* param) {
    int i = _cliIndex(ac, av, param) + 1;
    if (i < ac) {
        return av[i];
    }

    return nullptr;
}
