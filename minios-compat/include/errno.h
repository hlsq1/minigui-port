/* minios-compat <errno.h> - miniOS errno.h has E* codes but no `errno` lvalue.
 * MiniGUI (fixedmath.h) does `errno = ERANGE`, so declare it. Runtime needs a
 * definition (provide one in the FILE/crt layer). */
#pragma once
#include_next <errno.h>
extern int errno;
