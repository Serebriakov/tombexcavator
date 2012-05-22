#ifndef __BSW_IO_IOS_HPP__
#define __BSW_IO_IOS_HPP__

#include <ios>
#include <iosfwd>

#include "bsw/bsw_api.hpp"

namespace bsw
{
  typedef std::streampos file_size_t;

  enum whence_t
    {
      eFROM_START,
      eFROM_CURRENT,
      eFROM_END
    };

  BSW_API std::ostream& operator << (std::ostream& os, whence_t whence);
} // ns bsw

#endif
