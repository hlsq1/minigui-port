/* minios-compat <time.h> - PARTIAL: reuses miniOS struct tm/clock_t, adds the
 * standard bits miniOS lacks.
 *
 * GAP #3: miniOS has no time_t, and its localtime/gmtime use a NON-standard
 * signature: localtime(uint32_t, struct tm*) instead of localtime(const time_t*).
 * time() and mktime() are missing entirely. This header declares the standard
 * signatures so MiniGUI compiles; the implementation must wrap miniOS's
 * get_time()/getticks() to provide time()/mktime()/localtime()/gmtime().
 * Until implemented, linking reports time/mktime/localtime/gmtime undefined. */
#pragma once
#include <uapi/minios/time.h>   /* struct tm, clock_t, CLOCKS_PER_SEC */
typedef long time_t;
time_t time(time_t*);
struct tm *localtime(const time_t*); struct tm *gmtime(const time_t*);
time_t mktime(struct tm*); double difftime(time_t, time_t);
size_t strftime(char*, size_t, const char*, const struct tm*);
char *asctime(const struct tm*); char *ctime(const time_t*);
clock_t clock(void);
