#pragma once

#ifdef __OPTIMIZE__
#define DEBUG 0
#else
#define DEBUG 1
#endif

#define RESET "\033[;0m"
#define BOLD "\033[0;1m"
#define BOLD_RED "\033[1;31m"
#define BOLD_GREEN "\033[1;32m"
#define BOLD_YELLOW "\033[1;33m"
#define BOLD_BLUE "\033[1;34m"

#define error(msg)                                                             \
    do {                                                                       \
        fprintf(stderr, "%scinit: %serror%s: %s\n", BOLD, BOLD_RED, RESET,     \
                msg);                                                          \
    } while (0)

#define perr(msg)                                                              \
    do {                                                                       \
        fprintf(stderr, "%scinit: %serror%s: ", BOLD, BOLD_RED, RESET);        \
        perror(msg);                                                           \
    } while (0)

#define warn(msg)                                                              \
    do {                                                                       \
        fprintf(stdout, "%scinit: %swarn%s: %s\n", BOLD, BOLD_YELLOW, RESET,   \
                msg);                                                          \
    } while (0)

#define info(msg)                                                              \
    do {                                                                       \
        if (DEBUG) {                                                           \
            fprintf(stdout, "%scinit: %sinfo%s: %s\n", BOLD, BOLD_BLUE, RESET, \
                    msg);                                                      \
        }                                                                      \
    } while (0)

#define success(msg)                                                           \
    do {                                                                       \
        fprintf(stdout, "%scinit: %ssuccess%s: %s\n", BOLD, BOLD_GREEN, RESET, \
                msg);                                                          \
    } while (0)

#define MAYBE_UNUSED __attribute__((unused))
