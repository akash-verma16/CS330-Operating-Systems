## Lab 4: Virtual Memory Management

### Q1: New memory management API's in town... (`memMagic.c`)
Refactor an existing C program to replace all standard dynamic memory library calls (`malloc`, `free`, `realloc`) with their system call equivalents (`mmap`, `munmap`, `mremap`). The program functions must remain identical, but the underlying memory management must utilize anonymous memory mappings.

### Q2: What is my span! (`memspan.c`)
Implement a function to calculate the length of the continuous valid virtual address space containing a specific address. You must use `mmap` with address hints to probe the validity of memory pages without creating new allocations, returning the total size of the contiguous block or 0 if the initial address is invalid.

### Q3: Relocate Binary, for fun! (`relocate.c`)
Manually relocate the text segment of the running binary to a new dynamically allocated memory region. You must map a new executable area, copy the code, and ensure the `main` function and other dependencies function correctly at the new address by implementing the `relocate_textseg` function.