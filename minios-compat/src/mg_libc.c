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
#include <ctype.h>     /* isspace for sscanf */
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

/* atexit: miniOS's exit() knows nothing of registered handlers, so a real
 * registry would never be invoked. MiniGUI registers cleanup terminators here
 * (init-lite.c); on this embedded target the process does not cleanly exit and
 * TerminateGUI tears down explicitly, so a no-op that reports success is safe. */
int atexit(void (*func)(void))
{
    (void)func;
    return 0;
}

char *strncat(char *dst, const char *src, size_t n)
{
    char *d = dst;
    while (*d)
        d++;
    while (n-- && *src)
        *d++ = *src++;
    *d = '\0';
    return dst;
}

void *bsearch(const void *key, const void *base, size_t nmemb, size_t size,
        int (*compar)(const void *, const void *))
{
    size_t lo = 0, hi = nmemb;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        const void *elem = (const char *)base + mid * size;
        int c = compar(key, elem);
        if (c < 0)
            hi = mid;
        else if (c > 0)
            lo = mid + 1;
        else
            return (void *)elem;
    }
    return NULL;
}

/* Minimal sscanf: integer conversions (d/i/u/x/o), %c, %s, literal and
 * whitespace matching. All current callers (spinbox in combobox.c) scan a
 * single int with an integer format, so float support is intentionally
 * omitted (miniOS has no strtod). Returns the number of items assigned. */
static int mg_vsscanf(const char *s, const char *fmt, va_list ap)
{
    int count = 0;

    while (*fmt) {
        if (isspace((unsigned char)*fmt)) {
            while (isspace((unsigned char)*s))
                s++;
            fmt++;
            continue;
        }
        if (*fmt != '%') {
            if (*s != *fmt)
                break;
            s++;
            fmt++;
            continue;
        }

        fmt++;                          /* consume '%' */
        if (*fmt == '%') {
            if (*s != '%')
                break;
            s++;
            fmt++;
            continue;
        }

        int suppress = 0;
        if (*fmt == '*') {
            suppress = 1;
            fmt++;
        }
        int width = 0;
        while (*fmt >= '0' && *fmt <= '9')
            width = width * 10 + (*fmt++ - '0');
        while (*fmt == 'l' || *fmt == 'h' || *fmt == 'L' || *fmt == 'z')
            fmt++;                      /* ignore length modifiers */

        char conv = *fmt++;
        if (conv == '\0')
            break;

        if (conv != 'c')                /* skip leading spaces except for %c */
            while (isspace((unsigned char)*s))
                s++;

        switch (conv) {
        case 'd': case 'i': case 'u': case 'x': case 'o': {
            int base = (conv == 'x') ? 16 : (conv == 'o') ? 8 :
                       (conv == 'i') ? 0 : 10;
            char *end;
            long v = strtol(s, &end, base);
            if (end == s)
                return count;
            if (!suppress)
                *va_arg(ap, int *) = (int)v;
            s = end;
            if (!suppress)
                count++;
            break;
        }
        case 's': {
            if (!width)
                width = 0x7fffffff;
            char *out = suppress ? NULL : va_arg(ap, char *);
            if (!*s)
                return count;
            while (*s && !isspace((unsigned char)*s) && width--) {
                if (out)
                    *out++ = *s;
                s++;
            }
            if (out)
                *out = '\0';
            if (!suppress)
                count++;
            break;
        }
        case 'c': {
            if (!width)
                width = 1;
            char *out = suppress ? NULL : va_arg(ap, char *);
            if (!*s)
                return count;
            while (*s && width--) {
                if (out)
                    *out++ = *s;
                s++;
            }
            if (!suppress)
                count++;
            break;
        }
        default:
            return count;               /* unsupported conversion */
        }
    }

    return count;
}

int sscanf(const char *str, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = mg_vsscanf(str, fmt, ap);
    va_end(ap);
    return n;
}

/* munmap: miniOS has mmap but no unmap syscall. The only caller is
 * src/font/varbitmap.c, releasing a file-mapped bitmap font; this port uses
 * in-core fonts, so that path is effectively unused. A no-op that reports
 * success leaks the mapping at worst, which is harmless on this target. */
int munmap(void *addr, size_t length)
{
    (void)addr;
    (void)length;
    return 0;
}
