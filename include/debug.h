#pragma once

#include <stdio.h>

#define RESET "\033[;0m"
#define BOLD "\033[0;1m"
#define BOLD_RED "\033[1;31m"
#define BOLD_GREEN "\033[1;32m"
#define BOLD_YELLOW "\033[1;33m"
#define BOLD_BLUE "\033[1;34m"

#define error(...)                                                             \
    do {                                                                       \
        fprintf(stderr,                                                        \
                BOLD "cinit: " BOLD_RED "error" RESET ": " __VA_ARGS__);       \
        fprintf(stderr, "\n");                                                 \
    } while (0)

#define perr(msg)                                                              \
    do {                                                                       \
        fprintf(stderr, BOLD "cinit: " BOLD_RED "error" RESET ": ");           \
        perror(msg);                                                           \
    } while (0)

#define warning(...)                                                           \
    do {                                                                       \
        fprintf(stdout,                                                        \
                BOLD "cinit: " BOLD_YELLOW "warning" RESET ": " __VA_ARGS__);  \
        fprintf(stdout, "\n");                                                 \
    } while (0)

#define log(...)                                                               \
    do {                                                                       \
        fprintf(stdout, BOLD "cinit: " RESET __VA_ARGS__);                     \
        fprintf(stdout, "\n");                                                 \
    } while (0)

#ifdef DEBUG
#define debug(...)                                                             \
    do {                                                                       \
        fprintf(stderr,                                                        \
                BOLD "cinit: " BOLD_BLUE "debug" RESET " [%s:%d]" RESET ": ",  \
                __FILE__, __LINE__);                                           \
        fprintf(stderr, __VA_ARGS__);                                          \
        fprintf(stderr, "\n");                                                 \
    } while (0)
#else
#define debug(...)                                                             \
    do {                                                                       \
    } while (0)
#endif

#define success(...)                                                           \
    do {                                                                       \
        fprintf(stdout,                                                        \
                BOLD "cinit: " BOLD_GREEN "success" RESET ": " __VA_ARGS__);   \
        fprintf(stdout, "\n");                                                 \
    } while (0)

#define MAYBE_UNUSED __attribute__((unused))
