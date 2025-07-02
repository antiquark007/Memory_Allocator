// alloc.c - Simple memory allocator implementation
//there are comments for the guidance
#include "alloc.h"
#include <stddef.h>

// External heap memory space defined in assembly
extern heap *memspace;

// Maximum words available in the heap (1GB / 4 bytes per word)
#define Maxwords (1024*1024*1024/4)

// Error codes
#define ErrNoMem 1

// Error handling macro - returns NULL on memory allocation failure
#define reterr(err) return NULL

/**
 * mkalloc - Core allocation function that marks memory as allocated
 * @words: Number of 4-byte words to allocate
 * @hdr: Pointer to the header structure
 * 
 * Returns: Pointer to allocated memory or NULL on failure
 */
void *mkalloc(word words, header *hdr)
{
    void *ret;
    word old_words = hdr->w;

    // If this is a free block and larger than needed, split it
    if (old_words && old_words > words) {
        // Split: current header gets 'words', new header after it gets remaining
        hdr->w = words;
        hdr->alloced = true;
        // New header after allocated block
        header *next = (header *)((char *)hdr + sizeof(header) + words * 4);
        next->w = old_words - words - 1; // -1 for header
        next->alloced = false;
        next->reserved = 0;
    } else {
        // Normal allocation
        hdr->w = words;
        hdr->alloced = true;
    }
    ret = (void *)((char *)hdr + sizeof(header));
    return ret;
}

/**
 * alloc - Main allocation function (similar to malloc)
 * @bytes: Number of bytes to allocate
 * 
 * Returns: Pointer to allocated memory or NULL on failure
 */
void *alloc(int32 bytes)
{
    word words;
    header *hdr, *end;
    void *mem;

    // Convert bytes to words (round up if not divisible by 4)
    words = (!(bytes % 4)) ? bytes / 4 : (bytes / 4) + 1;

    // Get pointer to heap memory space
    mem = (void *)memspace;
    hdr = (header *)mem;
    end = (header *)((char *)memspace + Maxwords * 4);

    // Traverse heap to find a free block or the end
    while (hdr < end) {
        if (!hdr->w) {
            // Uninitialized header: allocate here if enough space
            if ((end - hdr) < (ptrdiff_t)(words + 1)) {
                reterr(ErrNoMem);
            }
            return mkalloc(words, hdr);
        }
        if (!hdr->alloced && hdr->w >= words) {
            // Free block large enough: reuse (with possible split)
            return mkalloc(words, hdr);
        }
        // Move to next block: header + block size
        hdr = (header *)((char *)hdr + sizeof(header) + hdr->w * 4);
    }

    // No space found
    return NULL;
}

/**
 * free_mem - Deallocate memory block and coalesce adjacent free blocks
 * @ptr: Pointer to memory to free
 */
void free_mem(void *ptr)
{
    if (!ptr) return;  // Handle NULL pointer

    // Get header by moving back sizeof(header) bytes from the user pointer
    header *hdr = (header *)((char *)ptr - sizeof(header));
    if (!hdr->alloced) {
        printf("Warning: Attempting to free unallocated memory\n");
        return;
    }
    hdr->alloced = false;

    // Coalesce with next block if it's free
    header *end = (header *)((char *)memspace + Maxwords * 4);
    header *next = (header *)((char *)hdr + sizeof(header) + hdr->w * 4);
    while (next < end && next->w && !next->alloced) {
        hdr->w += next->w + 1; // +1 for header
        next = (header *)((char *)hdr + sizeof(header) + hdr->w * 4);
    }

    // Coalesce with previous block if it's free
    // Traverse from heap start to find previous block
    header *scan = (header *)memspace;
    header *prev = NULL;
    while (scan < hdr) {
        if (!scan->alloced) prev = scan;
        scan = (header *)((char *)scan + sizeof(header) + scan->w * 4);
    }
    if (prev && !prev->alloced) {
        prev->w += hdr->w + 1; // +1 for header
    }

    printf("Freed memory\n");
}

/**
 * main - Test function for the allocator
 */
int main(int unused argc, char unused *argv[])
{
    int8 *p1, *p2, *p3;

    printf("Testing Memory Allocator\n");
    printf("========================\n");

    // Test allocation of 7 bytes
    printf("Allocating 7 bytes...\n");
    p1 = (int8 *)alloc(7);
    if (p1) {
        printf("Allocated memory at: 0x%p\n", (void *)p1);
        strcpy((char *)p1, "Hello");
        printf("Wrote 'Hello' to allocated memory\n");
        printf("Read back: '%s'\n", (char *)p1);
    } else {
        printf("Allocation failed!\n");
    }

    // Test another allocation
    printf("\nAllocating 16 bytes...\n");
    p2 = (int8 *)alloc(16);
    if (p2) {
        printf("Allocated memory at: 0x%p\n", (void *)p2);
    } else {
        printf("Allocation failed!\n");
    }

    // Test a third allocation
    printf("\nAllocating 12 bytes...\n");
    p3 = (int8 *)alloc(12);
    if (p3) {
        printf("Allocated memory at: 0x%p\n", (void *)p3);
    } else {
        printf("Allocation failed!\n");
    }

    // Test freeing memory
    printf("\nFreeing first allocation...\n");
    free_mem(p1);

    printf("\nFreeing second allocation...\n");
    free_mem(p2);

    printf("\nAllocating 20 bytes (should reuse freed space)...\n");
    int8 *p4 = (int8 *)alloc(20);
    if (p4) {
        printf("Allocated memory at: 0x%p\n", (void *)p4);
    } else {
        printf("Allocation failed!\n");
    }

    printf("\nFreeing third allocation...\n");
    free_mem(p3);

    printf("\nFreeing fourth allocation...\n");
    free_mem(p4);

    return 0;
}

// remaing Implement heap management for multiple allocations