#include <sstream>
#include <iostream>

#include "bsw/logger/device.hpp"
#include "bsw/logger/predicate.hpp"
#include "bsw/logger/formatter.hpp"
#include "bsw/logger/record.hpp"

namespace bsw 
{
  namespace logger
  {
    struct device_impl_s
    {
      device_impl_s ()
	: m_next      (0),
	  m_is_opened (false)
      {
      }
      // --------------------------------------------------------------
      formatter_ptr_t m_formatter;
      predicate_ptr_t m_predicate;
      device_c*       m_next;
      bool            m_is_opened;
    };

    // =======================================================================
    device_c::device_c (formatter_ptr_t formatter, 
			predicate_ptr_t predicate)
    {
      m_pimpl = new device_impl_s;
      m_pimpl->m_formatter = formatter;
      m_pimpl->m_predicate = predicate;
      m_pimpl->m_next = 0;
    }
    // ------------------------------------------------------------------------
    device_c::~device_c (void)
    {
      delete m_pimpl;
    }
    // -----------------------------------------------------------------------
    bool device_c::open ()
    {
      if (_do_open ())
	{
	  m_pimpl->m_is_opened = true;
	  if (!m_pimpl->m_formatter.null ())
	    {
	      std::ostringstream ostream;
	      m_pimpl->m_formatter->start (ostream);
	      _do_print_string (ostream.str ());
	      return true;
	    }
	}
      return false;
    }
    // ------------------------------------------------------------------------
    bool device_c::print (shared_log_record_t record)
    {
      formatter_ptr_t fmt = _formatter ();
      if (fmt.null ())
	{
	  return false;
	}
      if (!_filter (record.get ()))
	{
	  return true;
	}
      return _do_print (record);
    }
    // ------------------------------------------------------------------------
    void device_c::close ()
    {
      if (!m_pimpl->m_is_opened)
	{
	  return;
	}
      if (!m_pimpl->m_formatter.null ())
	{
	  std::ostringstream ostream;
	  m_pimpl->m_formatter->done (ostream);
	  _do_print_string (ostream.str ());
	}
      _do_close ();
    }
    // ------------------------------------------------------------------------
    bool device_c::is_opened () const
    {
      return m_pimpl->m_is_opened;
    }
    // ------------------------------------------------------------------------
    device_c* device_c::next () const
    {
      return m_pimpl->m_next;
    }
    // ------------------------------------------------------------------------
    void device_c::next (device_c* device)
    {
      m_pimpl->m_next = device;
    }
    // ------------------------------------------------------------------------
    bool device_c::_filter (const record_c* record)
    {
      predicate_ptr_t pr = _predicate ();
      if (pr.null ())
	{
	  return true;
	}
      return pr->call (record);
    }
    // -----------------------------------------------------------------------
    formatter_ptr_t device_c::_formatter () const
    {
      return m_pimpl->m_formatter;
    }
    // -----------------------------------------------------------------------
    predicate_ptr_t device_c::_predicate () const
    {
      return m_pimpl->m_predicate;
    }
    // ------------------------------------------------------------------------
    bool device_c::_do_print (shared_log_record_t record)
    {
      const record_c* blr = record.get ();
      std::ostringstream ostream;
      _formatter ()->print (blr, ostream);
      return _do_print_string (ostream.str ());
    }
  } // ns logger
} // ns bsw

