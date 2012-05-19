#include <ostream>
#include "bsw/logger/level.hpp"

namespace bsw
{
  namespace logger
  {
    std::ostream& operator << (std::ostream& os, level_t level)
    {
      switch (level)
	{
	case eDEBUG:
	  os << "DEBUG";
	  break;
	case eINFO:
	  os << "INFO";
	  break;
	case eNOTICE:
	  os << "NOTICE";
	  break;
	case eWARNING:
	  os << "WARNING";
	  break;
	case eERROR:
	  os << "ERROR";
	  break;
	case eFATAL:
	  os << "FATAL";
	  break;
	}
      return os;
    }
  } // ns logger
} // ns bsw
