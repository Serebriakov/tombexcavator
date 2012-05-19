#ifndef __BSW_TIME_HPP__
#define __BSW_TIME_HPP__

#include "bsw/bsw_api.hpp"

#include "thirdparty/predef.h"

#if defined(PREDEF_PLATFORM_UNIX) || defined(PREDEF_PLATFORM_LINUX)
#define HAVE_LOCALTIME_R
#endif

namespace bsw
{
  struct BSW_API time_s
  {
    time_s ();

    long seconds;
    long useconds;
  };

  time_s get_time ();
} // ns bsw



#if !defined(HAVE_LOCALTIME_R)
#include <time.h>

BSW_API struct tm * localtime_r(const time_t *timep, struct tm *result);

#endif

#endif
