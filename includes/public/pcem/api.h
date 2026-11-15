#ifndef _PCEM_API_H_
#define _PCEM_API_H_

#if defined(_WIN32) && defined(PLUGIN_ENGINE)
#  if defined(pcem_plugin_api_EXPORTS)
#    define PCEM_API __declspec(dllexport)
#  else
#    define PCEM_API __declspec(dllimport)
#  endif
#else
#  define PCEM_API
#endif

#endif /* _PCEM_API_H_ */
