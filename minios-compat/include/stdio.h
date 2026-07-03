/* minios-compat <stdio.h> - DECLARATION SKELETON, NOT YET IMPLEMENTED.
 *
 * GAP #1 (biggest): MiniGUI needs standard FILE*-based stdio, but miniOS
 * provides only fd-based stdio (stdout==1, fprintf(int fd, ...)). This header
 * declares the standard FILE* API so MiniGUI compiles; the implementation
 * still has to be written as a thin fd-based FILE wrapper:
 *     FILE = { int fd; ... };  fopen->open, fread->read, fseek->lseek, ...
 * Combined with incore resources, real fopen() calls at runtime are rare.
 * Until implemented, linking will report these symbols undefined. */
#pragma once
#include <stddef.h>
#include <stdarg.h>
typedef struct _MG_FILE FILE;
extern FILE *stdin; extern FILE *stdout; extern FILE *stderr;
#define EOF (-1)
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define BUFSIZ 1024
FILE *fopen(const char*, const char*); FILE *fdopen(int, const char*);
int fclose(FILE*); int fflush(FILE*);
size_t fread(void*, size_t, size_t, FILE*); size_t fwrite(const void*, size_t, size_t, FILE*);
int fseek(FILE*, long, int); long ftell(FILE*); void rewind(FILE*);
int fgetc(FILE*); int getc(FILE*); char *fgets(char*, int, FILE*);
int fputc(int, FILE*); int putc(int, FILE*); int fputs(const char*, FILE*);
int ungetc(int, FILE*); int feof(FILE*); int ferror(FILE*); void clearerr(FILE*);
int fileno(FILE*); int setvbuf(FILE*, char*, int, size_t); void setbuf(FILE*, char*);
int remove(const char*); int rename(const char*, const char*);
int printf(const char*, ...); int fprintf(FILE*, const char*, ...); int sprintf(char*, const char*, ...);
int snprintf(char*, size_t, const char*, ...); int vprintf(const char*, va_list);
int vfprintf(FILE*, const char*, va_list); int vsprintf(char*, const char*, va_list);
int vsnprintf(char*, size_t, const char*, va_list);
int scanf(const char*, ...); int fscanf(FILE*, const char*, ...); int sscanf(const char*, const char*, ...);
int putchar(int); int getchar(void); int puts(const char*); char *gets(char*);
void perror(const char*);
