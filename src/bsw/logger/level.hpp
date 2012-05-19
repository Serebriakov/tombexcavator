#ifndef __BSW_LOGGER_LEVEL_HPP__
#define __BSW_LOGGER_LEVEL_HPP__

#include <iosfwd>

namespace bsw
{
  namespace logger
  {
    enum level_t
      {
	eDEBUG,
	eINFO,
	eNOTICE,
	eWARNING,
	eERROR,
	eFATAL
      };

    std::ostream& operator << (std::ostream& os, level_t level);

  } // ns logger
} // ns bsw


#endif
