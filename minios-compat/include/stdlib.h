/* minios-compat <stdlib.h> - add div_t/ldiv_t + div/ldiv/labs on top of
 * miniOS's stdlib.h (which lacks them; MiniGUI look-and-feel code uses div_t). */
#pragma once
#include_next <stdlib.h>
#ifndef __MG_DIV_T_DEFINED
#define __MG_DIV_T_DEFINED
typedef struct { int quot; int rem; } div_t;
typedef struct { long quot; long rem; } ldiv_t;
div_t div(int, int);
ldiv_t ldiv(long, long);
long labs(long);
#endif
