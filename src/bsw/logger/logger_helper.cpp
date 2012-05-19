#include "bsw/logger/logger_helper.hpp"
#include "bsw/logger/system.hpp"

namespace bsw
{
  namespace logger
  {
    void post_record (record_c* record)
    {
      system_c::instance ()->post (record);
    }
  } // ns logger
} // ns bsw
