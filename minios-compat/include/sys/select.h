/* minios-compat <sys/select.h> - stub. No real select in miniOS. */
#pragma once
#include <sys/types.h>
#include <sys/time.h>
#define FD_ZERO(s)     __builtin_memset((s), 0, sizeof(*(s)))
#define FD_SET(fd,s)   ((s)->__fds_bits[(fd)/(8*sizeof(long))] |=  (1UL<<((fd)%(8*sizeof(long)))))
#define FD_CLR(fd,s)   ((s)->__fds_bits[(fd)/(8*sizeof(long))] &= ~(1UL<<((fd)%(8*sizeof(long)))))
#define FD_ISSET(fd,s) (((s)->__fds_bits[(fd)/(8*sizeof(long))] >> ((fd)%(8*sizeof(long)))) & 1)
int select(int, fd_set*, fd_set*, fd_set*, struct timeval*);
