/* mg_time.c - time() and mktime() for the miniOS MiniGUI port (GAP #3).
 *
 * miniOS userspace has getticks()/clock()/get_time()/localtime()/gmtime() but
 * NO standard time() and NO mktime() (mktime lives only in the kernel). The
 * standalone config drives all of MiniGUI's time use through src/misc/nposix.c:
 *   __mg_os_time   -> time()   (HAVE_TIME)
 *   __mg_os_mktime -> mktime() (HAVE_MKTIME)
 * Both names are absent from miniOS's user libs, so defining them here does NOT
 * clash with anything (unlike localtime, which miniOS already exports with a
 * non-standard 2-arg signature - HAVE_LOCALTIME is therefore left undef and
 * nposix.c uses its own fixed-date fallback).
 *
 * mktime uses the same Gauss algorithm as the miniOS kernel (kernel/time.c), so
 * time()/localtime() round-trip consistently: get_time() fills local wall-clock
 * fields plus __tm_gmtoff, and mktime() subtracts __tm_gmtoff to recover the
 * UTC epoch. */
#include <time.h>               /* time_t (compat), struct tm, __tm_gmtoff */

extern int get_time(struct tm *tm);   /* miniOS syscall wrapper (ulib/time.c) */

time_t mktime(struct tm *t)
{
    unsigned int mon  = (unsigned int)t->tm_mon + 1;
    unsigned int year = (unsigned int)t->tm_year + 1900;
    unsigned int day  = (unsigned int)t->tm_mday;
    unsigned int hour = (unsigned int)t->tm_hour;
    unsigned int min  = (unsigned int)t->tm_min;
    unsigned int sec  = (unsigned int)t->tm_sec;

    /* 1..12 -> 11,12,1..10, putting Feb (with its leap day) last */
    if ((int)(mon -= 2) <= 0) { mon += 12; year -= 1; }

    return (time_t)(
        ((((time_t)(year / 4 - year / 100 + year / 400 + 367 * mon / 12 + day)
           + (time_t)year * 365 - 719499) * 24 + hour) * 60 + min) * 60 + sec
        - t->__tm_gmtoff);
}

time_t time(time_t *tp)
{
    struct tm tm;
    get_time(&tm);              /* current local time; __tm_gmtoff = +8h */
    time_t t = mktime(&tm);     /* subtract __tm_gmtoff -> UTC epoch seconds */
    if (tp)
        *tp = t;
    return t;
}
