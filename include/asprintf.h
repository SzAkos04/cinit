#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

int asprintf(char **strp, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int size = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (size < 0) {
        return -1;
    }

    *strp = malloc(size + 1);
    if (!*strp) {
        return -1;
    }

    va_start(args, fmt);
    vsnprintf(*strp, size + 1, fmt, args);
    va_end(args);

    return size;
}
