// alloc.c - Simple memory allocator implementation
#include "alloc.h"

// External heap memory space defined in assembly
extern heap *memspace;

// Maximum words available in the heap (1GB / 4 bytes per word)
#define Maxwords (1024*1024*1024/4)

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
    void *ret, *bytesin;
    word wordsin;

    // Calculate offset from start of heap in bytes
    bytesin = (void *)((char *)hdr - (char *)memspace);
    
    // Convert bytes to words and add 1 for header
    wordsin = (((word)(uintptr_t)bytesin) / 4) + 1;
    
    // Check if requested allocation exceeds available space
    if (words > (Maxwords - wordsin))
    {
        reterr(ErrNoMem);
    }

    // Set header fields
    hdr->w = words;           // Store number of words allocated
    hdr->alloced = true;      // Mark as allocated
    
    // Return pointer to usable memory (after header)
    ret = (void *)((char *)hdr + 4);

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
    header *hdr;
    void *mem;

    // Convert bytes to words (round up if not divisible by 4)
    words = (!(bytes % 4)) ? bytes / 4 : (bytes / 4) + 1;

    // Get pointer to heap memory space
    mem = (void *)memspace;
    hdr = (header *)mem;

    // Check if this is the first allocation (header not initialized)
    if (!(hdr->w)) 
    {
        // Debug prints for testing
        // printf("words=%d\nmaxwords=%d\n",words,Maxwords);
        // printf("words>maxwords=%d\n",(words>Maxwords));
        
        // Check if allocation size exceeds heap capacity
        if (words > Maxwords)
        {
            reterr(ErrNoMem);
        }
        
        fflush(stdout);

        // Perform the actual allocation
        mem = mkalloc(words, hdr);
        if (!mem)
        {
            return NULL;  // Return NULL on allocation failure
        }
        return mem;  // Return allocated memory pointer
    }

    // TODO: Implement logic for subsequent allocations
    // This would involve finding free blocks in the heap
    return NULL;
}

/**
 * main - Test function for the allocator
 */
int main(int argc, char *argv[])
{
    int8 *p;
    
    // Test allocation of 7 bytes
    p = (int8 *)alloc(7);
    printf("Allocated memory at: 0x%p\n", (void *)p);

    return 0;
}

// TODO: Implement free() function to deallocate memory
// TODO: Implement heap management for multiple allocations