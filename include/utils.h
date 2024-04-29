#pragma once

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

bool is_correct_name(const char *name) {
    if (!isalpha(name[0])) {
        return false;
    }
    int len = strlen(name);
    for (int i = 0; i < len; i++) {
        if (!(isalnum(name[i]) || name[i] == '_')) {
            return false;
        }
    }
    return true;
}
