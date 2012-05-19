#ifndef __BSW_MT_OS_CONFIG_HPP__
#define __BSW_MT_OS_CONFIG_HPP__

#include "thirdparty/predef.h"

#if !defined(PREDEF_OS_WINDOWS)
#define BSW_USE_PTHREAD
#endif

#if defined(PREDEF_OS_WINDOWS)
#define BSW_HAS_ATOMIC
#define BSW_HAS_ATOMIC_WINDOWS
#elif defined(PREDEF_OS_MACOSX)
#define BSW_HAS_ATOMIC
#define BSW_HAS_ATOMIC_MACOSX
#elif ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1) || __GNUC__ > 4) && (defined(__x86_64__) || defined(__i386__))
#if !defined(BSW_HAS_ATOMIC)
#define BSW_HAS_ATOMIC
#endif
#define BSW_HAS_ATOMIC_GCC
#endif

#endif
