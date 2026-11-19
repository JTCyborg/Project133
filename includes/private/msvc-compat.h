#pragma once

/*
 * Minimal POSIX string API compatibility for the native MSVC toolchain.
 * Keep this header safe to force-include before every project translation unit.
 */
#ifdef _MSC_VER
/* Request the standard math constants before any translation unit includes
 * <math.h> or <cmath>. MSVC keeps these behind _USE_MATH_DEFINES. */
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

/* MSVC's CRT declares off_t in sys/types.h; several POSIX-derived sources
 * use it directly for file offsets. */
#include <sys/types.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
/* Avoid Windows min/max macros breaking qualified C++ standard-library calls. */
#ifndef NOMINMAX
#define NOMINMAX
#endif

/* POSIX large-file positioning API compatibility. */
#ifndef fseeko64
#define fseeko64 _fseeki64
#endif
#ifndef ftello64
#define ftello64 _ftelli64
#endif

/* Load Winsock 2 before any Windows header can pull in legacy winsock.h. */
#ifdef _WIN32
#include <winsock2.h>
#endif
#include <string.h>

#ifndef strcasecmp
#define strcasecmp _stricmp
#endif

#ifndef strncasecmp
#define strncasecmp _strnicmp
#endif

#ifndef strdup
#define strdup _strdup
#endif
#endif
