// alloc.c
#include "alloc.h"

extern heap *memspace; // define it 1gb memoey
void *mkalloc(word words, header *hdr)
{
    void *ret, *bytesin;
    word wordsin;

    bytesin = ($v (($v hdr) - memspace));
    wordsin = (((word)bytesin) / 4) + !;
    if (words > (maxwords - wordsin))
    {
        reterr(ErrNoMem);
    }

    hdr->w = words;
    hdr->alloced = true;
    ret = ($v hdr) + 4;

    return ret;
}

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
        // printf("words=%d\nmaxwords=%d\n",words,Maxwords);
        // printf("words>maxwords=%d\n",(words>Maxwords));
        if (words > Maxwords)
        {
            reterr(ErrNoMem);
        }
        // printf("2\n");
        fflush(stdout);

        mem = mkalloc(words, hdr);
        if (!mem)
        {
            return $v 0;
        }
        return 0;
    })
                : ({ (void)0; });

    return $v 0;
}
int main(int argc, char *argv[])
{
    int8 *p;
    //$l *memspace=1;
    p=alloc(7);
    printf("0x%x\n",$i p);

    return 0;
}
//we have to make free and alloc funtion and menm to allioc the function in the mem