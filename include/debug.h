#pragma once

#include <stdio.h>

#define RESET "\033[;0m"
#define BOLD "\033[1m"
#define DIM "\033[2m"
#define UNDERLINE "\033[4m"
#define INVERT "\033[7m"
#define STRIKETHROUGH "\033[9m"

#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

#define BRIGHT_BLACK "\033[90m"
#define BRIGHT_RED "\033[91m"
#define BRIGHT_GREEN "\033[92m"
#define BRIGHT_YELLOW "\033[93m"
#define BRIGHT_BLUE "\033[94m"
#define BRIGHT_MAGENTA "\033[95m"
#define BRIGHT_CYAN "\033[96m"
#define BRIGHT_WHITE "\033[97m"

#define error(...)                                                             \
    do {                                                                       \
        fprintf(stderr,                                                        \
                BOLD "cinit: " BOLD RED "error" RESET ": " __VA_ARGS__);       \
        fprintf(stderr, "\n");                                                 \
    } while (0)

#define perr(...)                                                              \
    do {                                                                       \
        fprintf(stderr,                                                        \
                BOLD "cinit: " BOLD RED "error" RESET ": " __VA_ARGS__);       \
        fprintf(stderr, "\n");                                                 \
        perror("  ↳ system error");                                            \
    } while (0)

#define warning(...)                                                           \
    do {                                                                       \
        fprintf(stdout,                                                        \
                BOLD "cinit: " BOLD YELLOW "warning" RESET ": " __VA_ARGS__);  \
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
                BOLD "cinit: " BOLD BLUE "debug" RESET " [%s:%d]" RESET ": ",  \
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
                BOLD "cinit: " BOLD GREEN "success" RESET ": " __VA_ARGS__);   \
        fprintf(stdout, "\n");                                                 \
    } while (0)

#define MAYBE_UNUSED __attribute__((unused))
