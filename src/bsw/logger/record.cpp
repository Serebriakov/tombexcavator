#include "bsw/logger/record.hpp"

namespace bsw
{
  namespace logger
  {
    record_c::record_c (level_t level, const locus_c& locus)
      : m_level     (level),
	m_locus     (locus),
	m_time      (get_time ()),
	m_thread_id (mt::current_thread_id ())
    {
      
    }
    // ---------------------------------------------------------------
    record_c::record_c (level_t level, 
			const locus_c& locus, 
			std::string& message)
      : m_level     (level),
	m_locus     (locus),
	m_message   (message),
	m_time      (get_time ()),
	m_thread_id (mt::current_thread_id ())
    {
    }
    // ---------------------------------------------------------------
    level_t record_c::level () const
    {
      return m_level;
    }
    // ---------------------------------------------------------------
    const locus_c& record_c::locus () const
    {
      return m_locus;
    }
    // ---------------------------------------------------------------
    const std::string& record_c::message () const
    {
      return m_message;
    }
    // ---------------------------------------------------------------
    const time_s& record_c::time () const
    {
      return m_time;
    }
    // ---------------------------------------------------------------
    mt::thread_id_t record_c::thread_id () const
    {
      return m_thread_id;
    }
  } // ns logger
} // ns bsw

