#pragma once

/*
 * Minimal POSIX string API compatibility for the native MSVC toolchain.
 * Keep this header safe to force-include before every project translation unit.
 */
#ifdef _MSC_VER
/* Avoid Windows min/max macros breaking qualified C++ standard-library calls. */
#ifndef NOMINMAX
#define NOMINMAX

/* POSIX large-file positioning API compatibility. */
#ifndef fseeko64
#define fseeko64 _fseeki64
#endif
#ifndef ftello64
#define ftello64 _ftelli64
#endif

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
