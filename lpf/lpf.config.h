#ifndef __LPF_CONFIG__
#define __LPF_CONFIG__

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32)) && !defined(__CYGWIN__)
#define LPF_WIN_NATIVE
#endif

#if defined(__CYGWIN__)
#define LPF_CYGWIN
#endif

#if defined(LPF_WIN_NATIVE) || defined(LPF_CYGWIN)
#define LPF_WIN32
#endif

#if !defined(LPF_WIN_NATIVE)
#define LPF_POSIX
#endif

#if defined(_MSC_VER)
#define LPF_MSVC
#if !defined(_DEBUG) && !defined(__LPF_INLINE__)
#define __LPF_INLINE__
#endif
#define _CRT_SECURE_NO_WARNINGS
#endif

#if defined(__GNUC__)
#define LPF_GCC
#if __GNUC__ < 4
#define LPF_GCC3
#endif
#if !defined (__LPF_INLINE__)
#define __LPF_INLINE__
#endif
#endif

#if defined(LPF_WIN32)
#define LPF_EXPORT _declspec(dllexport)
#else
#define LPF_EXPORT
#endif

#if defined(__linux)
#elif defined(__sun)
#elif defined(__hpux)
#elif defined(__FreeBSD__)
#elif defined(__NetBSD__)
#elif defined(__DragonFly__)
#elif defined(__OpenBSD__)
#elif defined(__APPLE__)
#endif

#if defined(LPF_LACKS_INLINE_FUNCTIONS) && !defined(LPF_NO_INLINE)
#define LPF_NO_INLINE
#endif

#if defined(LPF_NO_INLINE)
#undef __LPF_INLINE__
#endif

#if defined(__LPF_INLINE__)
#define LPF_INLINE inline
#else
#define LPF_INLINE
#endif

#if defined(LPF_WIN32)
#include <windows.h>
#if defined(LPF_CYGWIN)
#include <sys/cygwin.h>
#endif
#else
#if !defined(LPF_LACKS_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if !defined(LPF_LACKS_SYS_STAT_H)
#include <sys/stat.h>
#endif
#if !defined(LPF_LACKS_SYS_SYSCTL_H)
#include <sys/sysctl.h>
#endif
#if !defined(LPF_LACKS_UNISTD_H)
#include <unistd.h>
#endif
#endif

#endif

