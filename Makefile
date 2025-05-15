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
BUILD_ARGS ?= -DDEBUG
OBJ := $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Versioning: Check if Git is available
ifeq ($(shell git rev-parse --is-inside-work-tree 2>/dev/null),true)
    VERSION := $(shell git describe --tags --always --dirty)
else
    VERSION := unknown
endif
BUILD_DATE := $(shell date +"%d/%m/%Y %H:%M:%S")
BUILD_INFO := -DVERSION='"$(VERSION)"' -DBUILD_DATE='"$(BUILD_DATE)"'

.PHONY: all build release linux_install clean

all: build

build: $(BUILD_DIR)/$(PROJECT)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c "$<" -o "$@" $(INCLUDES) $(BUILD_ARGS) $(BUILD_INFO)

$(BUILD_DIR)/$(PROJECT): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS) $(BUILD_ARGS)

release: BUILD_ARGS+=-O3
release: build

linux_install: release
	sudo cp $(BUILD_DIR)/$(PROJECT) /usr/local/bin
	sudo mkdir -p /usr/local/man/man1/
	sudo cp $(PROJECT).1 /usr/local/man/man1/

CLEAN := rm -rf $(BUILD_DIR)

ifeq ($(OS),Windows_NT)
	CLEAN = rmdir /s /q $(BUILD_DIR)
endif

clean:
	$(CLEAN)
