#ifndef __BSW_LOGGER_MULTI_SETTER_HPP__
#define __BSW_LOGGER_MULTI_SETTER_HPP__

#include <sstream>

#include "bsw/logger/record.hpp"
#include "bsw/bsw_api.hpp"

namespace bsw
{
  namespace logger
  {
    namespace priv
    {
      class BSW_API multi_setter_c
      {
      public:
	multi_setter_c (record_c& record)
	  : m_record (record)
	{
	}
#include "bsw/logger/multi_setter.incl"
      private:
	record_c& m_record;
      };
    } // ns priv
  } // ns logger
} // ns bsw


#endif
