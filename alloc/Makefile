# Memory Allocator Makefile

# Compiler and assembler
CC = gcc
ASM = nasm

# Compiler flags
CFLAGS = -O2 -Wall -Wextra -std=c11 -g -m32
ASMFLAGS = -f elf32
LDFLAGS = -m32

# Target executable
TARGET = alloc

# Object files
OBJS = alloc.o heap.o

# Default target
.PHONY: all clean test

all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

# Compile C source file
alloc.o: alloc.c alloc.h
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble assembly source file
heap.o: heap.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# Test target to run the allocator
test: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	rm -f *.o $(TARGET)

# Show help
help:
	@echo "Available targets:"
	@echo "  all    - Build the allocator (default)"
	@echo "  test   - Build and run the allocator"
	@echo "  clean  - Remove build artifacts"
	@echo "  help   - Show this help message"
