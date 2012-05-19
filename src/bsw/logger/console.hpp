#ifndef __BSW_LOGGER_CONSOLE_HPP__
#define __BSW_LOGGER_CONSOLE_HPP__

#include "bsw/bsw_api.hpp"

namespace bsw
{
  namespace logger
  {
    class device_c;
    class predicate_c;

    BSW_API device_c* console_device ();
    BSW_API device_c* console_device (predicate_c* p);
  } // ns logger
} // ns bsw

#endif
