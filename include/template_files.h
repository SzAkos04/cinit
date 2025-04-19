#pragma once

#include "debug.h"

#include <stdlib.h>

#define FILE_SIZE 1024

static const char *main_c() {
    return "\
#include <stdio.h>\n\
\n\
int main(void) {\n\
    printf(\"Hello, World!\\n\");\n\
}";
}

static const char *main_cpp() {
    return "\
#include <iostream>\n\
\n\
int main() {\n\
    std::cout << \"Hello, World!\" << std::endl;\n\
}";
}

// HAS TO BE FREED
static char *makefile_c(const char *name) {
    char *str = (char *)malloc(FILE_SIZE);
    if (!str) {
        perr("failed to allocate memory");
        return NULL;
    }

#ifdef _WIN32
    snprintf(str, FILE_SIZE, "\
CC := gcc\n\
PROJECT := %s\n\
CFLAGS := -Wall -Wextra -Werror -Wpedantic\n\
LDFLAGS ?=\n\
INCLUDES := -Iinclude\n\
SRC_DIR := src\n\
SRC := $(wildcard $(SRC_DIR)/*.c)\n\
DEP := $(OBJ:.o=.d)\n\
ifneq ($(MAKECMDGOALS),clean)\n\
	-include $(wildcard $(DEP))\n\
endif\n\
BUILD_DIR := build\n\
BUILD_ARGS ?= -DDEBUG\n\
OBJ := $(SRC:$(SRC_DIR)/%%.c=$(BUILD_DIR)/%%.o)\n\
.PHONY: all build run clean\n\
\n\
all: run\n\
\n\
build: $(BUILD_DIR)/$(PROJECT)\n\
\n\
$(BUILD_DIR)/%%.o: $(SRC_DIR)/%%.c\n\
\t-@mkdir $(@D)\n\
\t$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ $(INCLUDES) $(BUILD_ARGS)\n\
\n\
$(BUILD_DIR)/$(PROJECT): $(OBJ)\n\
\t$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS) $(BUILD_ARGS)\n\
\n\
release:\n\
\t$(MAKE) BUILD_ARGS=\"-O3\" -B\n\
\n\
run: build\n\
\t@./$(BUILD_DIR)/$(PROJECT) $(ARGS)\n\
\n\
clean:\n\
\trmdir /s /q $(BUILD_DIR)",
             name);
#else
    snprintf(str, FILE_SIZE, "\
CC := gcc\n\
PROJECT := %s\n\
CFLAGS := -Wall -Wextra -Werror -Wpedantic\n\
LDFLAGS ?=\n\
INCLUDES := -Iinclude\n\
SRC_DIR := src\n\
SRC := $(wildcard $(SRC_DIR)/*.c)\n\
DEP := $(OBJ:.o=.d)\n\
ifneq ($(MAKECMDGOALS),clean)\n\
	-include $(wildcard $(DEP))\n\
endif\n\
BUILD_DIR := build\n\
BUILD_ARGS ?= -DDEBUG\n\
OBJ := $(SRC:$(SRC_DIR)/%%.c=$(BUILD_DIR)/%%.o)\n\
.PHONY: all build run clean\n\
\n\
all: run\n\
\n\
build: $(BUILD_DIR)/$(PROJECT)\n\
\n\
$(BUILD_DIR)/%%.o: $(SRC_DIR)/%%.c\n\
\t@mkdir -p $(@D)\n\
\t$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ $(INCLUDES) $(BUILD_ARGS)\n\
\n\
$(BUILD_DIR)/$(PROJECT): $(OBJ)\n\
\t$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS) $(BUILD_ARGS)\n\
\n\
release:\n\
\t$(MAKE) BUILD_ARGS=\"-O3\" -B\n\
\n\
run: build\n\
\t@./$(BUILD_DIR)/$(PROJECT) $(ARGS)\n\
\n\
clean:\n\
\trm -rf $(BUILD_DIR)",
             name);
#endif

    return str;
}

// HAS TO BE FREED
static char *makefile_cpp(const char *name) {
    char *str = (char *)malloc(FILE_SIZE);
    if (!str) {
        perr("failed to allocate memory");
        return NULL;
    }

#ifdef _WIN32
    snprintf(str, FILE_SIZE, "\
CXX := g++\n\
PROJECT := %s\n\
CFLAGS := -Wall -Wextra -Werror -Wpedantic\n\
LDFLAGS ?=\n\
INCLUDES := -Iinclude\n\
SRC_DIR := src\n\
SRC := $(wildcard $(SRC_DIR)/*.cpp)\n\
DEP := $(OBJ:.o=.d)\n\
ifneq ($(MAKECMDGOALS),clean)\n\
	-include $(wildcard $(DEP))\n\
endif\n\
BUILD_DIR := build\n\
BUILD_ARGS ?= -DDEBUG\n\
OBJ := $(SRC:$(SRC_DIR)/%%.cpp=$(BUILD_DIR)/%%.o)\n\
.PHONY: all build run clean\n\
\n\
all: run\n\
\n\
build: $(BUILD_DIR)/$(PROJECT)\n\
\n\
$(BUILD_DIR)/%%.o: $(SRC_DIR)/%%.cpp\n\
\t-@mkdir $(@D)\n\
\t$(CXX) $(CFLAGS) -MMD -MP -c $< -o $@ $(INCLUDES) $(BUILD_ARGS)\n\
\n\
$(BUILD_DIR)/$(PROJECT): $(OBJ)\n\
\t$(CXX) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS) $(BUILD_ARGS)\n\
\n\
release:\n\
\t$(MAKE) BUILD_ARGS=\"-O3\" -B\n\
\n\
run: build\n\
\t@./$(BUILD_DIR)/$(PROJECT) $(ARGS)\n\
\n\
clean:\n\
\trmdir /s /q $(BUILD_DIR)",
             name);
#else
    snprintf(str, FILE_SIZE, "\
CXX := g++\n\
PROJECT := %s\n\
CFLAGS := -Wall -Wextra -Werror -Wpedantic\n\
LDFLAGS ?=\n\
INCLUDES := -Iinclude\n\
SRC_DIR := src\n\
SRC := $(wildcard $(SRC_DIR)/*.cpp)\n\
DEP := $(OBJ:.o=.d)\n\
ifneq ($(MAKECMDGOALS),clean)\n\
	-include $(wildcard $(DEP))\n\
endif\n\
BUILD_DIR := build\n\
BUILD_ARGS ?= -DDEBUG\n\
OBJ := $(SRC:$(SRC_DIR)/%%.cpp=$(BUILD_DIR)/%%.o)\n\
.PHONY: all build run clean\n\
\n\
all: run\n\
\n\
build: $(BUILD_DIR)/$(PROJECT)\n\
\n\
$(BUILD_DIR)/%%.o: $(SRC_DIR)/%%.cpp\n\
\t@mkdir -p $(@D)\n\
\t$(CXX) $(CFLAGS) -MMD -MP -c $< -o $@ $(INCLUDES) $(BUILD_ARGS)\n\
\n\
$(BUILD_DIR)/$(PROJECT): $(OBJ)\n\
\t$(CXX) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS) $(BUILD_ARGS)\n\
\n\
release:\n\
\t$(MAKE) BUILD_ARGS=\"-O3\" -B\n\
\n\
run: build\n\
\t@./$(BUILD_DIR)/$(PROJECT) $(ARGS)\n\
\n\
clean:\n\
\trm -rf $(BUILD_DIR)",
             name);
#endif

    return str;
}

static const char *compile_flags() { return "-Iinclude"; }
