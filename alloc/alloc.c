//alloc.c
#include "alloc.h"

extern heap *memspace;//define it 1gb memoey

int main(int argc,char *argv[]){
    int8 *p;

    p=memspace;
    *p='X';
    return 0;
}