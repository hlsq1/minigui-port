/* mg_libm.c - minimal libm for the miniOS MiniGUI port (GAP #2).
 *
 * miniOS has no libm. The only floating-point math MiniGUI actually links in
 * the standalone+commlcd config comes from core GDI (miarc/miwideline/mispans/
 * mifillarc, rotatebmp) and desktop geometry: sqrt sin cos fabs atan2 asin
 * acos hypot floor ceil modf. That is the full set implemented here.
 *
 * miniOS is 32-bit x86 built -mno-sse, so the FP unit is the x87 FPU. The
 * primitives map to single x87 instructions (fsqrt/fsin/fcos/fabs/fpatan);
 * asin/acos/hypot/floor/ceil/modf are derived from them. This keeps the code
 * tiny and hardware-accurate, and pulls in NO libgcc soft-float helpers (every
 * routine stays in double - no double->int conversions).
 *
 * pow/log/exp/tan/fmod are declared in <math.h> but intentionally NOT defined:
 * their only callers (gamma.c) are not part of libminigui.a in this config. */

/* --- x87 primitives (result stays on the FPU stack top, st(0)) --- */

double sqrt(double x)
{
    double r;
    __asm__ ("fsqrt" : "=t"(r) : "0"(x));
    return r;
}

double sin(double x)
{
    double r;
    __asm__ ("fsin" : "=t"(r) : "0"(x));
    return r;
}

double cos(double x)
{
    double r;
    __asm__ ("fcos" : "=t"(r) : "0"(x));
    return r;
}

double fabs(double x)
{
    double r;
    __asm__ ("fabs" : "=t"(r) : "0"(x));
    return r;
}

double atan2(double y, double x)
{
    double r;
    __asm__ ("fldl %1\n\t"   /* st0 = y            */
             "fldl %2\n\t"   /* st0 = x, st1 = y   */
             "fpatan"        /* st0 = atan(st1/st0) = atan2(y, x) */
             : "=t"(r) : "m"(y), "m"(x));
    return r;
}

/* --- derived, no extra FPU sequences needed --- */

double asin(double x) { return atan2(x, sqrt(1.0 - x * x)); }
double acos(double x) { return atan2(sqrt(1.0 - x * x), x); }
double hypot(double x, double y) { return sqrt(x * x + y * y); }

/* floor(x) is rint(x) or rint(x)-1; it is the latter exactly when rint(x) > x.
 * frndint uses the default round-to-nearest mode, so this holds for every
 * rounding tie without touching the x87 control word. ceil is symmetric. */
double floor(double x)
{
    double r;
    __asm__ ("frndint" : "=t"(r) : "0"(x));
    return (r > x) ? r - 1.0 : r;
}

double ceil(double x)
{
    double r;
    __asm__ ("frndint" : "=t"(r) : "0"(x));
    return (r < x) ? r + 1.0 : r;
}

double modf(double x, double *iptr)
{
    double t = (x < 0.0) ? ceil(x) : floor(x);  /* integer part, toward zero */
    *iptr = t;
    return x - t;
}
