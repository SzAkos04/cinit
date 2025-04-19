CC := gcc
PROJECT := cinit
CFLAGS := -Wall -Wextra -Werror -Wpedantic
LDFLAGS ?=
INCLUDES := -Iinclude
SRC_DIR := src
SRC := $(wildcard $(SRC_DIR)/*.c)
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
    VERSION := "unknown"
endif
BUILD_DATE := $(shell date +"%Y-%m-%d %H:%M:%S")
BUILD_INFO := -DVERSION='"$(VERSION)"' -DBUILD_DATE='"$(BUILD_DATE)"'

.PHONY: all build clean

all: build

build: $(BUILD_DIR)/$(PROJECT)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ $(INCLUDES) $(BUILD_ARGS) $(BUILD_INFO)

$(BUILD_DIR)/$(PROJECT): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS) $(BUILD_ARGS) $(BUILD_INFO)

release:
	$(MAKE) BUILD_ARGS="-O3" -B

clean:
	rm -rf $(BUILD_DIR)
