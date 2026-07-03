/* minios-compat <sys/types.h> - add fd_set (miniOS has no select). standalone
 * MiniGUI doesn't actually select (HAVE_SELECT off) but customial.h references
 * fd_set in declarations. */
#pragma once
#include_next <sys/types.h>
#ifndef FD_SETSIZE
#define FD_SETSIZE 64
typedef struct { unsigned long __fds_bits[FD_SETSIZE/(8*sizeof(long))]; } fd_set;
#endif
