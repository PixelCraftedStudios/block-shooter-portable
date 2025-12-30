# Compiler
CC = gcc

# Directories
SRC_DIR = src
BUILD_DIR = build
ASSETS_DIR = assets

# Source and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Output executable
TARGET = $(BUILD_DIR)/main.exe

# Compiler flags
CFLAGS = -std=c99 -Wall -O2

# Linker flags (MSYS2-installed Raylib)
LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm

# Default target
all: $(TARGET) copy_assets

# Build executable
$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build folder if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Copy assets folder into build
copy_assets: | $(BUILD_DIR)
	if [ -d $(ASSETS_DIR) ]; then \
		cp -r $(ASSETS_DIR) $(BUILD_DIR)/; \
	fi

# Clean everything
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean copy_assets
