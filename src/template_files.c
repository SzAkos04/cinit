#include "template_files.h"

#include "cli.h"
#include "debug.h"

#include <stdlib.h>
#include <string.h>

const char *main_c(void) {
    return "\
#include <stdio.h>\n\
\n\
int main(void) {\n\
    printf(\"Hello, World!\\n\");\n\
    return 0;\n\
}";
}

const char *main_cpp(void) {
    return "\
#include <iostream>\n\
\n\
int main() {\n\
    std::cout << \"Hello, World!\" << std::endl;\n\
    return 0;\n\
}";
}

// MUST BE FREED
static char *makefile_template(const char *compiler, const char *compiler_bin,
                               const char *project, const char *ext) {
    char *result = NULL;

    if (asprintf(&result, "\
%s := %s\n\
PROJECT := %s\n\
CFLAGS := -Wall -Wextra -Werror -Wpedantic\n\
LDFLAGS ?=\n\
INCLUDES := -Iinclude\n\
SRC_DIR := src\n\
SRC := $(wildcard $(SRC_DIR)/*.%s)\n\
ifeq ($(SRC),)\n\
$(error No source files found in $(SRC_DIR))\n\
endif\n\
DEP := $(OBJ:.o=.d)\n\
ifneq ($(MAKECMDGOALS),clean)\n\
\t-include $(wildcard $(DEP))\n\
endif\n\
BUILD_DIR := build\n\
BUILD_ARGS ?= -DDEBUG\n\
OBJ := $(SRC:$(SRC_DIR)/%%.%s=$(BUILD_DIR)/%%.o)\n\
\n\
GREEN := $(shell printf '\033[0;32m')\n\
CYAN := $(shell printf '\033[0;36m')\n\
RESET := $(shell printf '\033[0m')\n\
ECHO = @echo\n\
\n\
.PHONY: all build release run clean\n\
\n\
all: run\n\
\n\
build: $(BUILD_DIR)/$(PROJECT)\n\
\n\
$(BUILD_DIR)/%%.o: $(SRC_DIR)/%%.%s\n\
\t@mkdir -p $(@D)\n\
\t$(ECHO) \"$(CYAN)[BUILD]$(RESET) Compiling $<...\"\n\
\t@$(%s) $(CFLAGS) -MMD -MP -c \"$<\" -o \"$@\" $(INCLUDES) $(BUILD_ARGS)\n\
\n\
$(BUILD_DIR)/$(PROJECT): $(OBJ)\n\
\t$(ECHO) \"$(CYAN)[LINK]$(RESET) Creating binary at $@\"\n\
\t@$(%s) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS) $(BUILD_ARGS)\n\
\t$(ECHO) \"$(GREEN)[OK]$(RESET) Build complete: $@\"\n\
\n\
release:\n\
\t$(ECHO) \"$(CYAN)[RELEASE]$(RESET) Building release version...\"\n\
\t@$(MAKE) -B build BUILD_ARGS=-O3\n\
\n\
run: build\n\
\t@./$(BUILD_DIR)/$(PROJECT) $(ARGS)\n\
\n\
clean:\n\
\t$(ECHO) \"$(CYAN)[CLEAN]$(RESET) Removing build directory...\"\n\
\t@$(RM) -r $(BUILD_DIR)\n\
\t$(ECHO) \"$(GREEN)[OK]$(RESET) Clean complete.\"\n\
\n\
help:\n\
\t$(ECHO) \"$(CYAN)[HELP]$(RESET) Available targets:\"\n\
\t$(ECHO) \"$(CYAN)[HELP]$(RESET)   build     - Compile the project\"\n\
\t$(ECHO) \"$(CYAN)[HELP]$(RESET)   release   - Build with -O3 optimizations\"\n\
\t$(ECHO) \"$(CYAN)[HELP]$(RESET)   clean     - Remove build files\"\
",
                 compiler, compiler_bin, project, ext, ext, ext, compiler,
                 compiler) == -1) {
        perr("makefile_tempalte: failed to allocate memory for `Makefile`");
        return NULL;
    }

    return result;
}

// MUST BE FREED
char *generate_makefile(const char *project, lang_t lang) {
    const char *compiler, *compiler_bin, *ext;

    if (lang == LANG_C) {
        compiler = "CC";
        compiler_bin = "gcc";
        ext = "c";
    } else if (lang == LANG_CPP) {
        compiler = "CXX";
        compiler_bin = "g++";
        ext = "cpp";
    } else {
        error("unsupported language");
        return NULL;
    }

    return makefile_template(compiler, compiler_bin, project, ext);
}

const char *compile_flags(void) { return "-Iinclude"; }

char *debug_h(const char *project) {
    char *result = NULL;

    char *part1 = "\
#pragma once\n\
\n\
#include <stdbool.h>\n\
#include <stdio.h>\n\
\n\
extern bool use_color;\n\
extern bool silent;\n\
\n\
#define COLOR(x) (use_color ? x : \"\")\n\
\n\
#define RESET COLOR(\"\\033[;0m\")\n\
#define BOLD COLOR(\"\\033[1m\")\n\
#define DIM COLOR(\"\\033[2m\")\n\
#define UNDERLINE COLOR(\"\\033[4m\")\n\
#define INVERT COLOR(\"\\033[7m\")\n\
#define STRIKETHROUGH COLOR(\"\\033[9m\")\n\
\n\
#define BLACK COLOR(\"\\033[30m\")\n\
#define RED COLOR(\"\\033[31m\")\n\
#define GREEN COLOR(\"\\033[32m\")\n\
#define YELLOW COLOR(\"\\033[33m\")\n\
#define BLUE COLOR(\"\\033[34m\")\n\
#define MAGENTA COLOR(\"\\033[35m\")\n\
#define CYAN COLOR(\"\\033[36m\")\n\
#define WHITE COLOR(\"\\033[37m\")\n\
\n\
#define BRIGHT_BLACK COLOR(\"\\033[90m\")\n\
#define BRIGHT_RED COLOR(\"\\033[91m\")\n\
#define BRIGHT_GREEN COLOR(\"\\033[92m\")\n\
#define BRIGHT_YELLOW COLOR(\"\\033[93m\")\n\
#define BRIGHT_BLUE COLOR(\"\\033[94m\")\n\
#define BRIGHT_MAGENTA COLOR(\"\\033[95m\")\n\
#define BRIGHT_CYAN COLOR(\"\\033[96m\")\n\
#define BRIGHT_WHITE COLOR(\"\\033[97m\")\n\
\n";

    if (asprintf(&result, "%s\
#define error(...)                                                             \\\n\
    do {                                                                       \\\n\
        fprintf(stderr, \"%%s%s: %%serror%%s: \", BOLD, RED, RESET);             \\\n\
        fprintf(stderr, __VA_ARGS__);                                          \\\n\
        fprintf(stderr, \"\\n\");                                                 \\\n\
    } while (0)\n\
\n\
#define perr(...)                                                              \\\n\
    do {                                                                       \\\n\
        fprintf(stderr, \"%%s%s: %%serror%%s: \", BOLD, RED, RESET);             \\\n\
        fprintf(stderr, __VA_ARGS__);                                          \\\n\
        fprintf(stderr, \"\\n\");                                                 \\\n\
        perror(\"  ↳ system error\");                                            \\\n\
    } while (0)\n\
\n\
#define warning(...)                                                           \\\n\
    do {                                                                       \\\n\
        if (!silent) {                                                         \\\n\
            fprintf(stdout, \"%%s%s: %%swarning%%s: \", BOLD, YELLOW, RESET);    \\\n\
            fprintf(stdout, __VA_ARGS__);                                      \\\n\
            fprintf(stdout, \"\\n\");                                             \\\n\
        }                                                                      \\\n\
    } while (0)\n\
\n\
#define info(...)                                                              \\\n\
    do {                                                                       \\\n\
        if (!silent) {                                                         \\\n\
            fprintf(stdout, \"%%s%s: info%%s: \", BOLD, RESET);                 \\\n\
            fprintf(stdout, __VA_ARGS__);                                      \\\n\
            fprintf(stdout, \"\\n\");                                             \\\n\
        }                                                                      \\\n\
    } while (0)\n\
\n\
#ifdef DEBUG\n\
#define debug(...)                                                             \\\n\
    do {                                                                       \\\n\
        if (!silent) {                                                         \\\n\
            fprintf(stdout, \"%%s%s: %%sdebug%%s [%%s:%%d]%%s: \", BOLD, BLUE,      \\\n\
                    RESET, __FILE__, __LINE__, RESET);                         \\\n\
            fprintf(stdout, __VA_ARGS__);                                      \\\n\
            fprintf(stdout, \"\\n\");                                             \\\n\
        }                                                                      \\\n\
    } while (0)\n\
#else\n\
#define debug(...)                                                             \\\n\
    do {                                                                       \\\n\
    } while (0)\n\
#endif\n\
\n\
#define success(...)                                                           \\\n\
    do {                                                                       \\\n\
        if (!silent) {                                                         \\\n\
            fprintf(stdout, \"%%s%s: %%ssuccess%%s: \", BOLD, GREEN, RESET);     \\\n\
            fprintf(stdout, __VA_ARGS__);                                      \\\n\
            fprintf(stdout, \"\\n\");                                             \\\n\
        }                                                                      \\\n\
    } while (0)\n\
\n\
#define MAYBE_UNUSED __attribute__((unused))\n\
",
                 part1, project, project, project, project, project,
                 project) == -1) {
        perr("debug_h: failed to allocate memory for `debug.h`");
        return NULL;
    }

    return result;
}

char *debug_hpp(const char *project) {
    char *result = NULL;

    char *part1 = "\
#pragma once\n\
\n\
#include <cstring>\n\
#include <iostream>\n\
\n\
extern bool use_color;\n\
extern bool silent;\n\
\n\
#define COLOR(x) (use_color ? x : \"\")\n\
\n\
#define RESET COLOR(\"\\033[;0m\")\n\
#define BOLD COLOR(\"\\033[1m\")\n\
#define DIM COLOR(\"\\033[2m\")\n\
#define UNDERLINE COLOR(\"\\033[4m\")\n\
#define INVERT COLOR(\"\\033[7m\")\n\
#define STRIKETHROUGH COLOR(\"\\033[9m\")\n\
\n\
#define BLACK COLOR(\"\\033[30m\")\n\
#define RED COLOR(\"\\033[31m\")\n\
#define GREEN COLOR(\"\\033[32m\")\n\
#define YELLOW COLOR(\"\\033[33m\")\n\
#define BLUE COLOR(\"\\033[34m\")\n\
#define MAGENTA COLOR(\"\\033[35m\")\n\
#define CYAN COLOR(\"\\033[36m\")\n\
#define WHITE COLOR(\"\\033[37m\")\n\
\n\
#define BRIGHT_BLACK COLOR(\"\\033[90m\")\n\
#define BRIGHT_RED COLOR(\"\\033[91m\")\n\
#define BRIGHT_GREEN COLOR(\"\\033[92m\")\n\
#define BRIGHT_YELLOW COLOR(\"\\033[93m\")\n\
#define BRIGHT_BLUE COLOR(\"\\033[94m\")\n\
#define BRIGHT_MAGENTA COLOR(\"\\033[95m\")\n\
#define BRIGHT_CYAN COLOR(\"\\033[96m\")\n\
#define BRIGHT_WHITE COLOR(\"\\033[97m\")\n\
\n";

    if (asprintf(&result, "%s\
#define error(msg)                                                             \\\n\
    do {                                                                       \\\n\
        std::cerr << BOLD << \"%s: \" << RED << \"error\" << RESET << \": \"    \\\n\
                  << msg << std::endl;                                         \\\n\
    } while (0)\n\
\n\
#define perr(msg)                                                              \\\n\
    do {                                                                       \\\n\
        std::cerr << BOLD << \"%s: \" << RED << \"error\" << RESET << \": \"    \\\n\
                  << msg << std::endl;                                         \\\n\
        std::cerr << \"  \\u21B3 system error: \" << strerror(errno)              \\\n\
                  << std::endl;                                                \\\n\
    } while (0)\n\
\n\
#define warning(msg)                                                           \\\n\
    do {                                                                       \\\n\
        if (!silent) {                                                         \\\n\
            std::cout << BOLD << \"%s: \" << YELLOW << \"warning\" << RESET   \\\n\
                      << \": \" << msg << std::endl;                             \\\n\
        }                                                                      \\\n\
    } while (0)\n\
\n\
#define info(msg)                                                              \\\n\
    do {                                                                       \\\n\
        if (!silent) {                                                         \\\n\
            std::cout << BOLD << \"%s: info\" << RESET << \": \" << msg       \\\n\
                      << std::endl;                                            \\\n\
        }                                                                      \\\n\
    } while (0)\n\
\n\
#ifdef DEBUG\n\
#define debug(msg)                                                             \\\n\
    do {                                                                       \\\n\
        if (!silent) {                                                         \\\n\
            std::cout << BOLD << \"%s: \" << BLUE << \"[\" << __FILE__ << \":\" \\\n\
                      << __LINE__ << \"]\" << RESET << \": \" << msg << std::endl; \\\n\
        }                                                                      \\\n\
    } while (0)\n\
#else\n\
#define debug(msg)                                                             \\\n\
    do {                                                                       \\\n\
    } while (0)\n\
#endif\n\
\n\
#define success(msg)                                                           \\\n\
    do {                                                                       \\\n\
        if (!silent) {                                                         \\\n\
            std::cout << BOLD << \"%s: \" << GREEN << \"success\" << RESET    \\\n\
                      << \": \" << msg << std::endl;                             \\\n\
        }                                                                      \\\n\
    } while (0)\n\
\n\
#define MAYBE_UNUSED __attribute__((unused))",
                 part1, project, project, project, project, project,
                 project) == -1) {
        perr("debug_hpp: failed to allocate memory for `debug.hpp`");
        return NULL;
    }

    return result;
}
