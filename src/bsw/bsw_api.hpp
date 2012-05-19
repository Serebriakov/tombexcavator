#ifndef __BSW_API_HPP__
#define __BSW_API_HPP__

#include "bsw/proper_export.h"

#if defined(BSW_AS_STATIC_LIB)
#   define BSW_API
#else
#   if defined(BSW_CREATE_SHARED_LIBRARY)
#      define BSW_API PROPER_EXPORT
#   else
#      define BSW_API PROPER_IMPORT
#   endif
#endif

#endif
