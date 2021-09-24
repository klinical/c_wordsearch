#ifndef UTILITY_H_
#define UTILITY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum direction {N, NE, E, SE, S, SW, W, NW};

char* S_Gets(char* st, int maxlen);
FILE* T_FOpen(const char* dest, const char* mode);

#endif
