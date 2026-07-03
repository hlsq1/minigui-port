/* minios-compat <stdbool.h> - miniOS's stdbool.h defines bool/true/false but
 * omits the C99 feature macro __bool_true_false_are_defined. Code that guards
 * its own bool/true/false definitions with that macro (e.g. MiniGUI's
 * src/font/unicode-emoji.c) would otherwise redefine them and clash. Add the
 * standard macro on top of miniOS's header. */
#pragma once
#include_next <stdbool.h>
#ifndef __bool_true_false_are_defined
#define __bool_true_false_are_defined 1
#endif
