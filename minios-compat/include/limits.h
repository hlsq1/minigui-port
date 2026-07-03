/* minios-compat <limits.h> - add standard SHRT/USHRT/INT names on top of
 * miniOS's (which uses I16_MAX/U16_MAX style). */
#pragma once
#include_next <limits.h>
#ifndef SHRT_MAX
#define SHRT_MAX  32767
#define SHRT_MIN  (-SHRT_MAX - 1)
#define USHRT_MAX 65535
#endif
#ifndef INT_MAX
#define INT_MAX  2147483647
#define INT_MIN  (-INT_MAX - 1)
#define UINT_MAX 4294967295U
#endif
