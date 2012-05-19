#ifndef __BSW_LOGGER_LOG_RECORD_LAYOUT_HPP__
#define __BSW_LOGGER_LOG_RECORD_LAYOUT_HPP__

#include "bsw/ptr/ptr.hpp"
#include "bsw/mt/mutex.hpp"
#include "bsw/logger/layout.hpp"

namespace bsw 
{
  namespace logger
  {

    class log_record_layout_c : public basic_layout_c <log_record_layout_c>
    {
    public:
      log_record_layout_c* append_level ();
    };

    typedef utils::shared_ptr_c <log_record_layout_c, mt::mutex_c> log_record_layout_ptr_t;
  } // ns logger
} // ns bsw

#endif
