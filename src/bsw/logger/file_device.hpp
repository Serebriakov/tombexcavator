#ifndef __BSW_LOGGER_FILE_DEVICE_HPP__
#define __BSW_LOGGER_FILE_DEVICE_HPP__

#include "bsw/bsw_api.hpp"

namespace bsw
{
  namespace logger
  {
    class device_c;
    class predicate_c;

    BSW_API device_c* file_device (const char* path);
    BSW_API device_c* file_device (const char* path, predicate_c* predicate);
  } // ns logger
} // ns bsw

#endif
