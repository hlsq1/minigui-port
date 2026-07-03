/* minios-compat <sys/termios.h> - miniOS has no termios. MiniGUI's
 * src/kernel/init-lite.c declares `static struct termios savedtermio;` outside
 * the __NOUNIX__ guard, so the type needs a complete definition even though the
 * tcgetattr/tcsetattr calls that would use it are compiled out. Provide the
 * standard layout as opaque storage. */
#pragma once

typedef unsigned int  tcflag_t;
typedef unsigned char cc_t;
typedef unsigned int  speed_t;

#define NCCS 32

struct termios {
    tcflag_t c_iflag;
    tcflag_t c_oflag;
    tcflag_t c_cflag;
    tcflag_t c_lflag;
    cc_t     c_line;
    cc_t     c_cc[NCCS];
    speed_t  c_ispeed;
    speed_t  c_ospeed;
};
