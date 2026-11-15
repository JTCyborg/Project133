#pragma once

/*
 * Minimal POSIX string API compatibility for the native MSVC toolchain.
 * Keep this header safe to force-include before every project translation unit.
 */
#ifdef _MSC_VER
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
