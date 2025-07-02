// alloc.h - Header file for memory allocator
#define _GNU_SOURCE
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>  // Added for uintptr_t

// Type definitions for different integer sizes
typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;
typedef __int128 int128;
typedef void heap;
typedef int32 word;

// Compiler attributes
#define packed __attribute__((__packed__)) // Tells compiler to pack structure tightly
#define unused __attribute__((__unused__)) // Marks variable as intentionally unused

/**
 * Header structure for memory blocks
 * Each allocated block starts with this header
 */
struct packed s_header {
    word w:30;          // Number of words in this block (30 bits)
    bool alloced:1;     // Allocation status flag (1 bit)
    bool reserved:1;    // Reserved for future use (1 bit)
};

typedef struct packed s_header header;

// Type casting macros for convenience
#define $1 (int8 *)      // Cast to byte pointer
#define $2 (int16)       // Cast to 16-bit integer
#define $4 (int32)       // Cast to 32-bit integer
#define $8 (int64)       // Cast to 64-bit integer
#define $16 (int128)     // Cast to 128-bit integer
#define $c (char *)      // Cast to character pointer
#define $i (int)         // Cast to integer
#define $v (void *)      // Cast to void pointer
#define $h (header *)    // Cast to header pointer

// Function declarations
void *mkalloc(word, header*);  // Core allocation function
void *alloc(int32);           // Main allocation interface (like malloc)
int main(int, char**);        // Main function for testing