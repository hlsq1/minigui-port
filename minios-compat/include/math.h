/* minios-compat <math.h> - DECLARATION SKELETON, NOT YET IMPLEMENTED.
 *
 * GAP #2: core GDI needs real floating-point libm - confirmed callers:
 *   miarc.c/miwideline.c (#include <math.h>): sin cos sqrt acos atan2
 *   rotatebmp.c: sqrt ; desktop.c: sqrt
 * miniOS has no libm. These are declared so MiniGUI compiles; a minimal libm
 * implementation still has to be provided (hand-rolled, or ported from
 * openlibm/newlib). _MGFIXED_MATH covers some paths with fixed-point, but the
 * float callers above remain. Until implemented, linking reports them undefined. */
#pragma once
double sin(double), cos(double), tan(double), asin(double), acos(double);
double atan(double), atan2(double,double), sqrt(double), pow(double,double);
double floor(double), ceil(double), fabs(double), exp(double), log(double), fmod(double,double);
double hypot(double,double);
float sqrtf(float), sinf(float), cosf(float);
#define M_PI 3.14159265358979323846
