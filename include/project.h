#pragma once

typedef enum {
    CMD_CREATE,
    CMD_INIT,

    CMD_NONE = -1,
} CMD;

typedef enum {
    FLAG_C = (1 << 0),
    FLAG_CPP = (1 << 1),
} Flag;

typedef struct Project {
    CMD command;
    unsigned int flags;
    const char *name;

    int (*create)(struct Project *);
} Project;

Project project_new(CMD command, unsigned int flags, const char *name);
void help(void);
void version(void);
