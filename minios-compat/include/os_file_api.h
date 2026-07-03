/* minios-compat <os_file_api.h> - MiniGUI's src/misc/misc-dt.c calls a "tp_"
 * file API (tp_fopen/tp_fread/... ) for reading and rewriting .etc config
 * files. Upstream this header comes from the ThreadX/embedded vendor port; here
 * the tp_ calls map straight onto the FILE* layer in mg_stdio.c. Declared so
 * the calls are prototyped rather than implicit-int. */
#pragma once
#include <stdio.h>

FILE  *tp_fopen(const char *path, const char *mode);
size_t tp_fread(void *ptr, size_t size, size_t nmemb, FILE *fp);
size_t tp_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *fp);
int    tp_fseek(FILE *fp, long off, int whence);
long   tp_ftell(FILE *fp);
long   tp_flength(FILE *fp);
int    tp_fexist(const char *path);
