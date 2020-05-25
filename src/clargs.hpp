#pragma once

int _cliIndex(int ac, char** av, const char* param);

bool cliSwitch(int ac, char** av, const char* param);

char* cliValue(int ac, char** av, const char* param);
