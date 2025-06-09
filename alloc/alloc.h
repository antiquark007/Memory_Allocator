// alloc.h
#define _GNU_SOURCE
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include <assert.h>
#include <errno.h>
//#include <birchutils.h>

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;
typedef __int128 int128;
typedef void heap;
typedef int32 word;

#define packed __attribute__((__packed__)) // attribute tells the compiler how it gonna hold diff things
#define unused __attribute__((__unused__))

struct packed s_header {
    word w:30;     // keep track of the number of blocks
    bool alloced:1;
    bool reserved:1;
};

typedef struct packed s_header header;

#define $1 (int8 *)
#define $2 (int16)
#define $4 (int32)
#define $8 (int64)
#define $16 (int128)
#define $c (char *)
#define $i (int)
#define $v (void *)
#define $h (header *)

void *alloc(int32);
int main(int, char**);