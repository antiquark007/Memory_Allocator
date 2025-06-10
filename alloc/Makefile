# Compiler and linker flags
CFLAGS = -O2 -Wall -std=c2x
LDFLAGS = -lbu

# Targets
.PHONY:clean

all: clean alloc

alloc: alloc.o heap.o
	cc $(CFLAGS) $^ -o $@ $(LDFLAGS)

alloc.o: alloc.c alloc.h
	cc $(CFLAGS) -c $<

heap.o: heap.asm
	nasm -f elf $< -o $@

clean:
	rm -f *.o alloc
