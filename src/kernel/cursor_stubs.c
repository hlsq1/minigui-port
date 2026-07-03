/* Stubs for stdio functions needed by MiniGUI cursor-comm.c.
 * These are compiled WITHOUT minios_compat.h to match the ABI of
 * cursor-sa.o which was compiled with system headers.
 * Uses the system's FILE type for ABI compatibility with cursor-sa.o.
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

/* System stdio.h declares FILE as an opaque type. We just need stubs.
 * stderr is declared as 'extern FILE *stderr' in system stdio.h.
 */
static FILE _mg_stderr_file;
FILE *stderr = &_mg_stderr_file;

FILE *fopen(const char *path, const char *mode)
{
    return NULL;
}

int fclose(FILE *fp)
{
    if (!fp) return -1;
    return 0;
}

int fseek(FILE *fp, long offset, int whence)
{
    if (!fp) return -1;
    return 0;
}

long ftell(FILE *fp)
{
    if (!fp) return -1;
    return 0;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *fp)
{
    if (!fp) return 0;
    return 0;
}

/* fprintf stub - cursor-comm.c calls fprintf(stderr, ...) */
int fprintf(FILE *fp, const char *fmt, ...)
{
    return 0;
}
