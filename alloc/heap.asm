bits 32
global memspace
%define Heapsize (1024*1024*1024/4)


Section .heap alloc noexec write nobits
memspace:
    heapsize equ Heapsize
    resd heapsize
