/* minios-compat <sys/time.h> - struct timeval + gettimeofday.
 * struct timeval declared here; gettimeofday() has NO implementation in miniOS
 * and must be provided (wrap get_time()/getticks()). Part of GAP #3 (time). */
#pragma once
#include <time.h>
struct timeval { long tv_sec; long tv_usec; };
struct timezone { int tz_minuteswest; int tz_dsttime; };
int gettimeofday(struct timeval *tv, void *tz);
