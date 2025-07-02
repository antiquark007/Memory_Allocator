bits 32
global memspace

; Define heap size as 1GB divided by 4 (for 32-bit words)
%define Heapsize (1024*1024*1024/4)

; BSS section for uninitialized data
section .bss
memspace:
    resd Heapsize

; GNU stack note section
section .note.GNU-stack noalloc noexec nowrite progbits