/* mgconfig.h for miniOS - minimal MiniGUI configuration
 *
 * MiniGUI standalone mode with commlcd GAL and comm IAL.
 * No external dependencies (no freetype, PNG, JPEG, pthread, zlib).
 * Uses autoconf-style: #define for enabled, / * #undef * / for disabled. */

#ifndef MG_MINIOS_CONFIG_H
#define MG_MINIOS_CONFIG_H

/* MiniGUI runtime mode: standalone (linked into app) */
#define _MGRM_STANDALONE       1

/* GAL engine: commlcd (linear framebuffer) */
#define _MGGAL_COMMLCD         1

/* IAL engine: comm (common input: keyboard + mouse) */
#define _MGIAL_COMM            1

/* Schema: auto detection */
#define _MGSCHEMA_AUTO         1

/* Font support: use internal RBF font only */
#define _MGFONT_RBF            1

/* Incore resource only */
#define _MGINCORE_RES          1

/* Fixed math */
#define _MGFIXED_MATH          1

/* OS definition */
#define __MINIOS__             1
#define __NOUNIX__             1

/* --- Available features (autoconf-style: defined = enabled) --- */
#define HAVE_TIME              1
#define HAVE_LOCALTIME         1
#define HAVE_SYS_TYPES_H       1
#define HAVE_SYS_STAT_H        1
#define HAVE_STDLIB_H          1
#define HAVE_STRING_H          1
#define HAVE_STDINT_H          1
#define HAVE_UNISTD_H          1
#define HAVE_FCNTL_H           1
#define HAVE_SYS_TIME_H        1
#define HAVE_STDARG_H          1

/* Misc */
#define SIZEOF_UNSIGNED_LONG   4
#define ETCFILENAME            "MiniGUI.cfg"
#define ABSFILEPATH            ""

/* --- Disabled features (autoconf-style: commented out = disabled) --- */
/* #undef HAVE_MKTIME */
/* #undef HAVE_STRDUP */
/* #undef HAVE_STRCASECMP */
/* #undef HAVE_STRNCASECMP */
/* #undef HAVE_CLOCK_GETTIME */
/* #undef HAVE_LIBM */
/* #undef HAVE_LIBPTHREAD */
/* #undef HAVE_LIBDL */
/* #undef HAVE_LIBJPEG */
/* #undef HAVE_LIBPNG */
/* #undef HAVE_LIBFREETYPE */
/* #undef HAVE_LIBZ */
/* #undef HAVE_SELECT */
/* #undef HAVE_POLL */
/* #undef HAVE_SIGNAL */
/* #undef HAVE_MEMMOVE */
/* #undef HAVE_GETPAGESIZE */
/* #undef HAVE_GETTIMEOFDAY */
/* #undef HAVE_NANOSLEEP */
/* #undef HAVE_ALLOCA */
/* #undef HAVE_ALLOCA_H */
/* #undef HAVE_MEMORY_H */
/* #undef HAVE_STRINGS_H */
/* #undef HAVE_INTTYPES_H */
/* #undef HAVE_LIMITS_H */
/* #undef HAVE_DIRENT_H */
/* #undef HAVE_DLFCN_H */
/* #undef HAVE_SIGNAL_H */
/* #undef HAVE_DECL_SHM_ANON */
/* #undef HAVE_ACCESS */
/* #undef HAVE_VPRINTF */

/* #undef _MGRM_PROCESSES */
/* #undef _MGRM_THREADS */
/* #undef _MGFONT_TTF */
/* #undef _MGFONT_FT2 */
/* #undef _MGFONT_UPF */
/* #undef _MGFONT_QPF */
/* #undef _MGIMAGE_PNG */
/* #undef _MGIMAGE_JPG */
/* #undef _MGIMAGE_GIF */
/* #undef _MGIMAGE_BMP */
/* #undef _MGIMAGE_SVG */
/* #undef _MGSUPPORT_CURSOR */
/* #undef _MG_ENABLE_SHMCHUNK */
/* #undef _MG_ENABLE_SHMCHUNK_CONNECTION */
/* #undef _MG_ENABLE_DOUBLE_BUFFER */
/* #undef _MG_ENABLE_DOUBLEFB */
/* #undef _MG_ENABLE_ADVANCEDGDI */
/* #undef _MGDUMPBMP */
/* #undef _MG_ENABLE_XMLRPC */
/* #undef _MG_ENABLE_SSL_CLIENT */
/* #undef _MISC_FILE_OPEN_MODE */
/* #undef _MGUSE_OWN_STDIO */

#endif /* MG_MINIOS_CONFIG_H */
