#ifndef __BSW_LOGGER_RECORD_HPP__
#define __BSW_LOGGER_RECORD_HPP__

#include <string>

#include "bsw/logger/locus.hpp"
#include "bsw/logger/level.hpp"
#include "bsw/mt/thread_id.hpp"
#include "bsw/mt/mutex.hpp"
#include "bsw/ptr/ptr.hpp"
#include "bsw/time/time.hpp"
#include "bsw/bsw_api.hpp"

namespace bsw
{
  namespace logger
  {
    namespace priv
    {
      class multi_setter_c;
    }

    class BSW_API record_c
    {
      friend class priv::multi_setter_c;

    public:
      record_c (level_t level, const locus_c& locus);

      record_c (level_t level, const locus_c& locus, std::string& message);
      
      level_t level () const;
      const locus_c& locus () const;
      const std::string& message () const;

      const time_s& time () const;
      mt::thread_id_t thread_id () const;
    private:
      level_t         m_level;
      locus_c         m_locus;
#if defined(_MSC_VER)
#pragma warning ( push )
#pragma warning ( disable: 4251 )
#endif
      std::string     m_message;
#if defined(_MSC_VER)
#pragma warning ( pop )
#endif    
      time_s          m_time;
      mt::thread_id_t m_thread_id;
    };

    typedef utils::shared_ptr_c <record_c, mt::mutex_c> shared_log_record_t;

  } // ns logger
} // ns bsw

#endif
