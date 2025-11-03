# Select-String Makefile for Windows

# Compiler - use gcc
CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -Wpedantic -O2 -DNDEBUG
LDFLAGS :=

# Directories
SRC_DIR := src
BIN_DIR := bin

# Target - NO .exe extension (but gcc will add .exe, so we rename)
TARGET := $(BIN_DIR)/Select-String
TARGET_EXE := $(BIN_DIR)/Select-String.exe
SRC := $(SRC_DIR)/main.c

# Installation directory
INSTALL_DIR := $(HOME)/bin

# Default target
.PHONY: all
all: $(TARGET)

# Create bin directory if it doesn't exist
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Build target - gcc adds .exe, so we rename it
$(TARGET): $(SRC) | $(BIN_DIR)
	@echo "Building Select-String..."
	@echo "Using compiler: $(CC)"
	$(CC) $(CFLAGS) -o $(TARGET_EXE) $< $(LDFLAGS)
	@mv $(TARGET_EXE) $(TARGET)
	@echo "Build complete: $(TARGET)"

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@rm -f $(BIN_DIR)/Select-String
	@rm -f $(BIN_DIR)/Select-String.exe
	@rm -f $(SRC_DIR)/*.obj
	@rm -f *.obj
	@echo "Clean complete"

# Install to ~/bin
.PHONY: install
install: $(TARGET)
	@echo "Installing to $(INSTALL_DIR)..."
	@mkdir -p "$(INSTALL_DIR)"
	@rm -f "$(INSTALL_DIR)/Select-String.exe"
	@cp $(TARGET) "$(INSTALL_DIR)/Select-String"
	@echo "Installation complete: $(INSTALL_DIR)/Select-String"

# Uninstall from ~/bin
.PHONY: uninstall
uninstall:
	@echo "Uninstalling from $(INSTALL_DIR)..."
	@rm -f "$(INSTALL_DIR)/Select-String"
	@rm -f "$(INSTALL_DIR)/Select-String.exe"
	@echo "Uninstall complete"

# Show help
.PHONY: help
help:
	@echo "Select-String Build System (Windows)"
	@echo ""
	@echo "Targets:"
	@echo "  make          - Build the Select-String binary"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make install  - Install to ~/bin"
	@echo "  make uninstall- Remove from ~/bin"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "Configuration:"
	@echo "  Compiler: $(CC)"
	@echo "  Install directory: $(INSTALL_DIR)"
