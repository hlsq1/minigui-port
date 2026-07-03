/* mg_libc.c - small libc gaps for the miniOS MiniGUI port (GAP #4, misc).
 *
 * miniOS's user libs provide malloc/free, sprintf/vsprintf, the E* codes and
 * ctype, but a few standard pieces MiniGUI links are missing. This file fills
 * exactly those, reusing miniOS primitives so behaviour stays consistent:
 *
 *   errno            - miniOS declares the E* macros but never defines the
 *                      `errno` object; MiniGUI (fixedmath.h) uses it as an
 *                      lvalue. Single global is fine (standalone, single-thread).
 *   calloc/realloc   - miniOS has only malloc/free. realloc needs the old block
 *                      size to copy: miniOS records every allocation in the
 *                      global `mal` table (malloc.h), so we look it up there.
 *   abs/div/ldiv     - plain stdlib arithmetic MiniGUI look-and-feel code uses.
 *   snprintf/vsnprintf - miniOS's own bounded formatter is vnstrfmt() (fmt.h,
 *                      the same stb-based core behind sprintf/printf); wrap it
 *                      so format handling is identical to the rest of the system.
 *
 * strcasecmp/strncasecmp are NOT here: HAVE_STRCASECMP/HAVE_STRNCASECMP are left
 * undef so src/misc/nposix.c compiles its own (built on miniOS tolower()). */
#include <stdlib.h>
#include <malloc.h>    /* malloc/free + the global `mal` allocation table */
#include <string.h>
#include <stdarg.h>
#include <fmt.h>       /* miniOS vnstrfmt(buf, n, fmt, ap) */

/* miniOS's allocation record table (defined in ulib/malloc.c, type in malloc.h
 * but not declared there). Each entry is {vaddr, size} for a live malloc. */
extern malloced mal;

int errno = 0;

void *calloc(size_t nmemb, size_t size)
{
    size_t total = nmemb * size;
    if (nmemb != 0 && total / nmemb != size)   /* multiplication overflow */
        return NULL;
    void *p = malloc(total);
    if (p)
        memset(p, 0, total);
    return p;
}

void *realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
        return malloc(size);
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    /* recover the old block size from miniOS's allocation table */
    size_t old = 0;
    for (uint32_t i = 0; i < mal.count; i++) {
        if (mal.mtable[i].vaddr == (uint32_t)(uintptr_t)ptr) {
            old = mal.mtable[i].size;
            break;
        }
    }

    void *np = malloc(size);
    if (!np)
        return NULL;                 /* old block left intact, per C standard */
    memcpy(np, ptr, old < size ? old : size);
    free(ptr);
    return np;
}

int abs(int n) { return n < 0 ? -n : n; }

div_t div(int num, int den)
{
    div_t r;
    r.quot = num / den;
    r.rem  = num % den;
    return r;
}

ldiv_t ldiv(long num, long den)
{
    ldiv_t r;
    r.quot = num / den;
    r.rem  = num % den;
    return r;
}

int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap)
{
    return vnstrfmt(buf, (int)size, fmt, ap);
}

int snprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = vnstrfmt(buf, (int)size, fmt, ap);
    va_end(ap);
    return n;
}
