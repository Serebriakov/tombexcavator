#include <iostream>

#include "bsw/io/ios.hpp"

namespace bsw
{
  std::ostream& operator << (std::ostream& os, whence_t whence)
  {
    switch (whence)
      {
      case eFROM_START:
	os << "FROM_START";
	break;
      case eFROM_CURRENT:
	os << "FROM_CURRENT";
	break;
      case eFROM_END:
	os << "FROM_END";
	break;
      }
    return os;
  }
} // ns bsw

