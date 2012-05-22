#ifndef __PICT_API_HPP__
#define __PICT_API_HPP__

#include "bsw/proper_export.h"

#if defined(PICT_AS_STATIC_LIB)
#   define PICT_API
#else
#   if defined(PICT_CREATE_SHARED_LIBRARY)
#      define PICT_API PROPER_EXPORT
#   else
#      define PICT_API PROPER_IMPORT
#   endif
#endif

#endif
