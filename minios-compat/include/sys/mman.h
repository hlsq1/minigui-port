#pragma once
#include <io.h>
#ifndef PROT_READ
#define PROT_NONE 0
#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4
#define MAP_SHARED 8
#define MAP_PRIVATE 16
#define MAP_FIXED 32
#endif
#ifndef MAP_FAILED
#define MAP_FAILED ((void *)-1)
#endif

/* miniOS provides mmap (io.h) but no munmap; declared here, defined no-op in
 * minios-compat/src/mg_libc.c. See the comment there. */
int munmap(void *addr, size_t length);
