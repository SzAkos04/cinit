#pragma once

#include <stdbool.h>

typedef enum {
    FLAG_C = (1 << 0),
    FLAG_CPP = (1 << 1),
} flag_t;

typedef struct project_t {
    const char *name;
    char *path;
    unsigned int flags;

    int (*generate)(struct project_t *self);
} project_t;

project_t *project_new(void);
void project_free(project_t *project);

bool is_correct_name(const char *str);

void version(void);
void help(void);
