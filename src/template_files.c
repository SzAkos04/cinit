#include "template_files.h"

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

// HAS TO BE FREED
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
        perr("failed to allocate memory for Makefile");
        return NULL;
    }

    return result;
}

// HAS TO BE FREED
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
        perr("unsupported language");
        return NULL;
    }

    return makefile_template(compiler, compiler_bin, project, ext);
}

const char *compile_flags(void) { return "-Iinclude"; }
