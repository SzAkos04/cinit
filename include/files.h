#pragma once

#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

#define MAIN_C                                                                 \
    "#include <stdio.h>\n"                                                     \
    "\n"                                                                       \
    "int main(void) {\n"                                                       \
    "    printf(\"Hello, World!\\n\");\n"                                      \
    "    return 0;\n"                                                          \
    "}\n"

#define MAIN_CPP                                                               \
    "#include <iostream>\n"                                                    \
    "\n"                                                                       \
    "int main() {\n"                                                           \
    "    std::cout << \"Hello, World!\" << std::endl;"                         \
    "    return 0;\n"                                                          \
    "}\n"

#define COMPILE_FLAGS "-Iinclude"

#define FILE_LENGTH 1024

char *MAKEFILE_C(const char *name) {
    char *content = (char *)malloc(sizeof(char) * FILE_LENGTH);
    if (!content) {
        perr("failed to allocate memory");
        return NULL;
    }
#ifdef _WIN32
    snprintf(content, FILE_LENGTH, "\
CC := gcc\n\
PROJECT := %s\n\
CFLAGS := -Wall -Wextra -Werror -Wpedantic\n\
LDFLAGS :=\n\
INCLUDES := -Iinclude\n\
SRC_DIR := src\n\
SRC := $(wildcard $(SRC_DIR)/*.c)\n\
BUILD_DIR := build\n\
OBJ := $(SRC:$(SRC_DIR)/%%.c=$(BUILD_DIR)/%%.o)\n\
.PHONY: all build run clean\n\
\n\
all: run\n\
\n\
build: $(BUILD_DIR)/$(PROJECT)\n\
\n\
$(BUILD_DIR)/%%.o: $(SRC_DIR)/%%.c\n\
\t-@mkdir $(@D)\n\
\t$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES) $(BUILD_ARGS)\n\
\n\
$(BUILD_DIR)/$(PROJECT): $(OBJ)\n\
\t$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(BUILD_ARGS)\n\
\n\
release:\n\
\t$(MAKE) BUILD_ARGS=\"-O3\" -B\n\
\n\
run: build\n\
\t./$(BUILD_DIR)/$(PROJECT) $(ARGS)\n\
\n\
clean:\n\
\trmdir /s /q $(BUILD_DIR)",
             name);
#else
    snprintf(content, FILE_LENGTH, "\
CC := gcc\n\
PROJECT := %s\n\
CFLAGS := -Wall -Wextra -Werror -Wpedantic\n\
LDFLAGS :=\n\
INCLUDES := -Iinclude\n\
SRC_DIR := src\n\
SRC := $(wildcard $(SRC_DIR)/*.c)\n\
BUILD_DIR := build\n\
OBJ := $(SRC:$(SRC_DIR)/%%.c=$(BUILD_DIR)/%%.o)\n\
.PHONY: all build run clean\n\
\n\
all: run\n\
\n\
build: $(BUILD_DIR)/$(PROJECT)\n\
\n\
$(BUILD_DIR)/%%.o: $(SRC_DIR)/%%.c\n\
\t@mkdir -p $(@D)\n\
\t$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES) $(BUILD_ARGS)\n\
\n\
$(BUILD_DIR)/$(PROJECT): $(OBJ)\n\
\t$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(BUILD_ARGS)\n\
\n\
release:\n\
\t$(MAKE) BUILD_ARGS=\"-O3\" -B\n\
\n\
run: build\n\
\t./$(BUILD_DIR)/$(PROJECT) $(ARGS)\n\
\n\
clean:\n\
\trm -rf $(BUILD_DIR)",
             name);
#endif
    return content;
}

char *MAKEFILE_CPP(const char *name) {
    char *content = (char *)malloc(FILE_LENGTH);
    if (!content) {
        perr("failed to allocate memory");
        return NULL;
    }
#ifdef _WIN32
    snprintf(content, FILE_LENGTH, "\
CXX := g++\n\
PROJECT := %s\n\
CFLAGS := -Wall -Wextra -Werror -Wpedantic\n\
LDFLAGS :=\n\
INCLUDES := -Iinclude\n\
SRC_DIR := src\n\
SRC := $(wildcard $(SRC_DIR)/*.cpp)\n\
BUILD_DIR := build\n\
OBJ := $(SRC:$(SRC_DIR)/%%.cpp=$(BUILD_DIR)/%%.o)\n\
.PHONY: all build run clean\n\
\n\
all: run\n\
\n\
build: $(BUILD_DIR)/$(PROJECT)\n\
\n\
$(BUILD_DIR)/%%.o: $(SRC_DIR)/%%.cpp\n\
\t-@mkdir $(@D)\n\
\t$(CXX) $(CFLAGS) -c $< -o $@ $(INCLUDES) $(BUILD_ARGS)\n\
\n\
$(BUILD_DIR)/$(PROJECT): $(OBJ)\n\
\t$(CXX) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(BUILD_ARGS)\n\
\n\
release:\n\
\t$(MAKE) BUILD_ARGS=\"-O3\" -B\n\
\n\
run: build\n\
\t./$(BUILD_DIR)/$(PROJECT) $(ARGS)\n\
\n\
clean:\n\
\trmdir /s /q $(BUILD_DIR)",
             name);
#else
    snprintf(content, FILE_LENGTH, "\
CXX := g++\n\
PROJECT := %s\n\
CFLAGS := -Wall -Wextra -Werror -Wpedantic\n\
LDFLAGS :=\n\
INCLUDES := -Iinclude\n\
SRC_DIR := src\n\
SRC := $(wildcard $(SRC_DIR)/*.cpp)\n\
BUILD_DIR := build\n\
OBJ := $(SRC:$(SRC_DIR)/%%.cpp=$(BUILD_DIR)/%%.o)\n\
.PHONY: all build run clean\n\
\n\
all: run\n\
\n\
build: $(BUILD_DIR)/$(PROJECT)\n\
\n\
$(BUILD_DIR)/%%.o: $(SRC_DIR)/%%.cpp\n\
\t@mkdir -p $(@D)\n\
\t$(CXX) $(CFLAGS) -c $< -o $@ $(INCLUDES) $(BUILD_ARGS)\n\
\n\
$(BUILD_DIR)/$(PROJECT): $(OBJ)\n\
\t$(CXX) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(BUILD_ARGS)\n\
\n\
release:\n\
\t$(MAKE) BUILD_ARGS=\"-O3\" -B\n\
\n\
run: build\n\
\t./$(BUILD_DIR)/$(PROJECT) $(ARGS)\n\
\n\
clean:\n\
\trm -rf $(BUILD_DIR)",
             name);
#endif
    return content;
}
