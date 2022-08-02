#ifndef FLECS_OS_API_STDCPP_H
#define FLECS_OS_API_STDCPP_H

/* This generated file contains includes for project dependencies */
#include <flecs.h>

/* Convenience macro for exporting symbols */
#ifndef flecs_os_api_stdcpp_STATIC
#if flecs_os_api_stdcpp_EXPORTS && (defined(_MSC_VER) || defined(__MINGW32__))
  #define FLECS_OS_API_STDCPP_API __declspec(dllexport)
#elif flecs_os_api_stdcpp_EXPORTS
  #define FLECS_OS_API_STDCPP_API __attribute__((__visibility__("default")))
#elif defined _MSC_VER
  #define FLECS_OS_API_STDCPP_API __declspec(dllimport)
#else
  #define FLECS_OS_API_STDCPP_API
#endif
#else
  #define FLECS_OS_API_STDCPP_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

FLECS_OS_API_STDCPP_API
extern void stdcpp_set_os_api(void);

#ifdef __cplusplus
}
#endif

#endif