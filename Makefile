CC := gcc
PROJECT := cinit
CFLAGS := -Wall -Wextra -Werror -Wpedantic
LDFLAGS ?=
INCLUDES := -Iinclude
SRC_DIR := src
SRC := $(wildcard $(SRC_DIR)/*.c)
ifeq ($(SRC),)
$(error No source files found in $(SRC_DIR))
endif
DEP := $(OBJ:.o=.d)
ifneq ($(MAKECMDGOALS),clean)
	-include $(wildcard $(DEP))
endif

BUILD_DIR := build
BUILD_ARGS ?= -g -DDEBUG
OBJ := $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Versioning: Check if Git is available
ifeq ($(shell git rev-parse --is-inside-work-tree 2>/dev/null),true)
    VERSION := $(shell git describe --tags --always --dirty)
else
    VERSION := unknown
endif
BUILD_DATE := $(shell date +"%d/%m/%Y %H:%M:%S")
BUILD_INFO := -DVERSION='"$(VERSION)"' -DBUILD_DATE='"$(BUILD_DATE)"'

PREFIX ?= /usr/local
BIN_DIR ?= $(PREFIX)/bin
ifeq (,$(wildcard $(PROJECT).1))
$(error Man page $(PROJECT).1 not found)
endif
MAN_DIR ?= $(PREFIX)/man/man1

GREEN := $(shell printf '\033[0;32m')
CYAN := $(shell printf '\033[0;36m')
RESET := $(shell printf '\033[0m')
ECHO = @echo

.PHONY: all build release clean install uninstall help

all: build

build: $(BUILD_DIR)/$(PROJECT)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(ECHO) "$(CYAN)[BUILD]$(RESET) Compiling $<..."
	@$(CC) $(CFLAGS) -MMD -MP -c "$<" -o "$@" $(INCLUDES) $(BUILD_ARGS) $(BUILD_INFO)

$(BUILD_DIR)/$(PROJECT): $(OBJ)
	$(ECHO) "$(CYAN)[LINK]$(RESET) Creating binary at $@"
	@$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS) $(BUILD_ARGS)
	$(ECHO) "$(GREEN)[OK]$(RESET) Build complete: $@"

release:
	$(ECHO) "$(CYAN)[RELEASE]$(RESET) Building release version..."
	@$(MAKE) -B build BUILD_ARGS=-O3

clean:
	$(ECHO) "$(CYAN)[CLEAN]$(RESET) Removing build directory..."
	@$(RM) -r $(BUILD_DIR)
	$(ECHO) "$(GREEN)[OK]$(RESET) Clean complete."

ifeq ($(OS),Windows_NT)
install: release
	$(ECHO) "$(CYAN)[INFO]$(RESET) Add '$(BUILD_DIR)/' to your system's PATH to use '$(PROJECT)' globally."

uninstall: clean
	$(ECHO) "$(CYAN)[INFO]$(RESET) Remove '$(BUILD_DIR)/' from your system's PATH.
else
install: release
	$(ECHO) "$(CYAN)[INSTALL]$(RESET) Installing binary to $(BIN_DIR)..."
	@install -Dm755 $(BUILD_DIR)/$(PROJECT) $(BIN_DIR)/$(PROJECT)
	$(ECHO) "$(GREEN)[OK]$(RESET) Binary installed to $(BIN_DIR)"
	$(ECHO) "$(CYAN)[INSTALL]$(RESET) Installing man page to $(MAN_DIR)..."
	@mkdir -p $(MAN_DIR)
	@cp $(PROJECT).1 $(MAN_DIR)
	$(ECHO) "$(GREEN)[OK]$(RESET) Man page installed to $(MAN_DIR)"

uninstall: clean
	$(ECHO) "$(CYAN)[UNINSTALL]$(RESET) Removing binary from $(BIN_DIR)..."
	@$(RM) -r $(BIN_DIR)/$(PROJECT)
	$(ECHO) "$(GREEN)[OK]$(RESET) Binary removed from $(BIN_DIR)"
	$(ECHO) "$(CYAN)[INSTALL]$(RESET) Removing man page from $(MAN_DIR)..."
	@$(RM) -r $(MAN_DIR)/$(PROJECT).1
	$(ECHO) "$(GREEN)[OK]$(RESET) Man page removed from $(MAN_DIR)"
endif

help:
	$(ECHO) "$(CYAN)[HELP]$(RESET) Available targets:"
	$(ECHO) "$(CYAN)[HELP]$(RESET)   build     - Compile the project"
	$(ECHO) "$(CYAN)[HELP]$(RESET)   release   - Build with -O3 optimizations"
	$(ECHO) "$(CYAN)[HELP]$(RESET)   clean     - Remove build files"
	$(ECHO) "$(CYAN)[HELP]$(RESET)   install   - Install binary and man page"
	$(ECHO) "$(CYAN)[HELP]$(RESET)   uninstall - Remove installed files"
