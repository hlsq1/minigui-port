/* minios-compat <signal.h> - miniOS's <signal.h> (ulib) only declares
 * kill()/signal() and its own Sigaction struct; it lacks the POSIX signal
 * numbers, sigset_t, struct sigaction and sigaction() that MiniGUI's
 * src/kernel/init-lite.c uses to install a SEGV/abort debug handler
 * (InstallSEGVHandler). miniOS delivers no such signals, so provide the
 * declarations plus a no-op sigaction() to satisfy the compile/link; the
 * handler simply never fires. */
#pragma once
#include_next <signal.h>

#ifndef SIGINT
#define SIGINT   2
#endif
#ifndef SIGABRT
#define SIGABRT  6
#endif
#ifndef SIGSEGV
#define SIGSEGV  11
#endif
#ifndef SIGPIPE
#define SIGPIPE  13
#endif
#ifndef SIGTERM
#define SIGTERM  15
#endif

typedef unsigned long sigset_t;

struct sigaction {
    void (*sa_handler)(int);
    unsigned long sa_flags;
    sigset_t sa_mask;
};

static inline int sigaction(int signum, const struct sigaction *act,
        struct sigaction *oldact)
{
    (void)signum; (void)act; (void)oldact;
    return 0;
}
