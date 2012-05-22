#ifndef __WESTWOOD_API_HPP__
#define __WESTWOOD_API_HPP__

#include "bsw/proper_export.h"

#if defined(WESTWOOD_AS_STATIC_LIB)
#   define WESTWOOD_API
#else
#   if defined(WESTWOOD_CREATE_SHARED_LIBRARY)
#      define WESTWOOD_API PROPER_EXPORT
#   else
#      define WESTWOOD_API PROPER_IMPORT
#   endif
#endif

#endif
