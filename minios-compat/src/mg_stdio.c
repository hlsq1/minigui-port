/* mg_stdio.c - fd-based FILE* layer for the miniOS MiniGUI port (GAP #1).
 *
 * miniOS provides only fd-based stdio; MiniGUI needs standard FILE*. This
 * implements the FILE* API declared in minios-compat/include/stdio.h as a thin
 * wrapper over miniOS syscalls (open/read/write/lseek/close). Unbuffered:
 * every op forwards straight to a syscall - simple and correct; buffering can
 * be added later if profiling shows it matters. With _MGINCORE_RES on, runtime
 * fopen() is rare, so this mostly satisfies linking.
 *
 * sprintf/vsprintf come from miniOS ulib (vsprintf.c.obj, standard signatures).
 * printf IS defined here on purpose: miniOS bundles printf+fprintf+vfprintf into
 * one object (printf.c.obj) whose fprintf/vfprintf take an int fd, not a FILE*.
 * If the linker pulled that object to satisfy printf, its fd-based fprintf/
 * vfprintf would clash with our FILE*-based ones. Defining printf here shadows
 * the whole object so it never gets pulled. vsnprintf is provided elsewhere
 * (task #90 / MiniGUI's own src/libc). */
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <malloc.h>   /* miniOS declares malloc/free here, not in stdlib.h */
#include <string.h>
#include <stdarg.h>

#define _F_EOF 1
#define _F_ERR 2

struct _MG_FILE { int fd; int flags; int ungot; };

static FILE _stdin_f  = { 0, 0, -1 };
static FILE _stdout_f = { 1, 0, -1 };
static FILE _stderr_f = { 2, 0, -1 };
FILE *stdin  = &_stdin_f;
FILE *stdout = &_stdout_f;
FILE *stderr = &_stderr_f;

static int is_std(FILE *f) { return f == &_stdin_f || f == &_stdout_f || f == &_stderr_f; }

static int parse_mode(const char *mode)
{
    int plus = (strchr(mode, '+') != NULL);
    switch (mode[0]) {
    case 'w': return (plus ? O_RDWR : O_WRONLY) | O_CREAT | O_TRUNC;
    case 'a': return (plus ? O_RDWR : O_WRONLY) | O_CREAT | O_APPEND;
    case 'r':
    default:  return plus ? O_RDWR : O_RDONLY;
    }
}

FILE *fopen(const char *path, const char *mode)
{
    int fd = open(path, parse_mode(mode), 0666);
    if (fd < 0)
        return NULL;
    FILE *f = malloc(sizeof *f);
    if (!f) { close(fd); return NULL; }
    f->fd = fd; f->flags = 0; f->ungot = -1;
    return f;
}

FILE *fdopen(int fd, const char *mode)
{
    (void)mode;
    FILE *f = malloc(sizeof *f);
    if (!f) return NULL;
    f->fd = fd; f->flags = 0; f->ungot = -1;
    return f;
}

int fclose(FILE *f)
{
    if (!f) return EOF;
    int r = close(f->fd);
    if (!is_std(f)) free(f);
    return r < 0 ? EOF : 0;
}

size_t fread(void *ptr, size_t size, size_t n, FILE *f)
{
    if (!f || size == 0) return 0;
    size_t total = size * n, done = 0;
    char *p = ptr;
    /* honour a pending ungetc first */
    if (f->ungot != -1 && total > 0) {
        p[done++] = (char)f->ungot;
        f->ungot = -1;
    }
    while (done < total) {
        ssize_t r = read(f->fd, p + done, total - done);
        if (r < 0)  { f->flags |= _F_ERR; break; }
        if (r == 0) { f->flags |= _F_EOF; break; }
        done += (size_t)r;
    }
    return done / size;
}

size_t fwrite(const void *ptr, size_t size, size_t n, FILE *f)
{
    if (!f || size == 0) return 0;
    size_t total = size * n, done = 0;
    const char *p = ptr;
    while (done < total) {
        ssize_t r = write(f->fd, p + done, total - done);
        if (r <= 0) { f->flags |= _F_ERR; break; }
        done += (size_t)r;
    }
    return done / size;
}

int fseek(FILE *f, long off, int whence)
{
    if (!f) return -1;
    if (lseek(f->fd, off, whence) < 0) return -1;
    f->flags &= ~_F_EOF;
    f->ungot = -1;
    return 0;
}

long ftell(FILE *f) { return f ? (long)lseek(f->fd, 0, SEEK_CUR) : -1; }

void rewind(FILE *f)
{
    if (!f) return;
    lseek(f->fd, 0, SEEK_SET);
    f->flags = 0;
    f->ungot = -1;
}

int fgetc(FILE *f)
{
    if (!f) return EOF;
    if (f->ungot != -1) { int c = f->ungot; f->ungot = -1; return c; }
    unsigned char c;
    ssize_t r = read(f->fd, &c, 1);
    if (r == 1) return c;
    f->flags |= (r == 0) ? _F_EOF : _F_ERR;
    return EOF;
}

int getc(FILE *f) { return fgetc(f); }

int ungetc(int c, FILE *f)
{
    if (!f || c == EOF) return EOF;
    f->ungot = (unsigned char)c;
    f->flags &= ~_F_EOF;
    return c;
}

char *fgets(char *s, int size, FILE *f)
{
    if (!f || size <= 0) return NULL;
    int i = 0;
    while (i < size - 1) {
        int c = fgetc(f);
        if (c == EOF) break;
        s[i++] = (char)c;
        if (c == '\n') break;
    }
    if (i == 0) return NULL;
    s[i] = '\0';
    return s;
}

int fputc(int c, FILE *f)
{
    unsigned char ch = (unsigned char)c;
    if (!f) return EOF;
    if (write(f->fd, &ch, 1) != 1) { f->flags |= _F_ERR; return EOF; }
    return c;
}

int putc(int c, FILE *f) { return fputc(c, f); }
int putchar(int c) { return fputc(c, stdout); }

int fputs(const char *s, FILE *f)
{
    size_t len = strlen(s);
    return fwrite(s, 1, len, f) == len ? 0 : EOF;
}

int puts(const char *s)
{
    if (fputs(s, stdout) == EOF) return EOF;
    return fputc('\n', stdout) == EOF ? EOF : 0;
}

int feof(FILE *f)   { return f ? (f->flags & _F_EOF) : 0; }
int ferror(FILE *f) { return f ? (f->flags & _F_ERR) : 0; }
void clearerr(FILE *f) { if (f) f->flags &= ~(_F_EOF | _F_ERR); }
int fileno(FILE *f) { return f ? f->fd : -1; }
int fflush(FILE *f) { (void)f; return 0; }   /* unbuffered: nothing to flush */
int setvbuf(FILE *f, char *buf, int mode, size_t size)
{ (void)f; (void)buf; (void)mode; (void)size; return 0; }
void setbuf(FILE *f, char *buf) { (void)f; (void)buf; }

int remove(const char *path) { return unlink(path); }
int rename(const char *o, const char *n) { (void)o; (void)n; return -1; }

int vfprintf(FILE *f, const char *fmt, va_list ap)
{
    char buf[1024];
    int n = vsnprintf(buf, sizeof buf, fmt, ap);
    if (n < 0) return -1;
    int w = (n < (int)sizeof buf) ? n : (int)sizeof buf - 1;
    return (int)fwrite(buf, 1, (size_t)w, f);
}

int fprintf(FILE *f, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = vfprintf(f, fmt, ap);
    va_end(ap);
    return n;
}

int printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = vfprintf(stdout, fmt, ap);
    va_end(ap);
    return n;
}

void perror(const char *s)
{
    if (s && *s) { fputs(s, stderr); fputs(": ", stderr); }
    fputs("error\n", stderr);
}
