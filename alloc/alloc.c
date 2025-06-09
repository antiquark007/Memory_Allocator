// alloc.c
#include "alloc.h"

extern heap *memspace; // define it 1gb memoey

// 2*4=8 for 7=>7/4=1+1=8/4
void *alloc(int32 bytes)
{
    word words;
    header *hdr;
    void *mem;

    words = (!(bytes % 4)) ? bytes / 4 : (bytes / 4) + 1;

    mem = $v memspace;
    hdr = $h mem;
    (!(hdr->w)) ? ({
        printf("empty\n");
        exit(0);
    })
                : ({
                      printf("bla\n");
                      exit(0);
                  });

    return $v 0;
}
int main(int argc, char *argv[])
{
    //$l memspace=l;
    alloc(7);
    return 0;
}