CC := gcc
PROJECT := cinit
CFLAGS := -Wall -Wextra -Wpedantic
# CFLAGS += -Werror
LDFLAGS :=
INCLUDES := -Iinclude
SRC_DIR := src
SRC := $(wildcard $(SRC_DIR)/*.c)
BUILD_DIR := build
OBJ := $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

RM := rm -rf
MKDIR := mkdir -p
ifeq ($(OS),Windows_NT)
    RM = rmdir /s /q
	MKDIR = md
endif

.PHONY: all build run clean

all: build

build: $(BUILD_DIR)/$(PROJECT)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	-@$(MKDIR) $(@D)
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES) $(BUILD_ARGS)

$(BUILD_DIR)/$(PROJECT): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(BUILD_ARGS)

release:
	$(MAKE) BUILD_ARGS="-O3" -B

run: build
	./$(BUILD_DIR)/$(PROJECT) $(ARGS)

clean:
	$(RM) $(BUILD_DIR)
