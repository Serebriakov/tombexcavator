#include "thirdparty/predef.h"

#include "bsw/logger/system.hpp"
#include "bsw/logger/device.hpp"
#include "bsw/logger/record.hpp"
#include "bsw/logger/console.hpp"

namespace bsw
{
  namespace logger
  {
#if defined(PREDEF_OS_WINDOWS)
//#pragma data_seg(".BSW_LOGGER")
#endif
    system_c* system_c::s_instance = 0;
#if defined(PREDEF_OS_WINDOWS)
//#pragma data_seg()
//#pragma comment(linker, "/section:.BSW_LOGGER,rws")
#endif
    // ------------------------------------------------------------
    system_c* system_c::instance ()
    {
      if (!s_instance)
	{
	  s_instance = new system_c;
	}
      return s_instance;
    }
    // ------------------------------------------------------------
    system_c::system_c ()
      : m_use_threads (false),
	m_devices_head (0),
	m_devices_tail (0),
    m_default_device (console_device ())
    {
    }
    // ------------------------------------------------------------
    system_c::~system_c ()
    {
      device_c* victim = m_devices_head; 
      while (victim) 
	{ 
	  device_c* next = victim->next ();
	  victim->close ();
	  delete victim;
	  victim = next;
	}
      delete m_default_device;
    }
    // -------------------------------------------------------------
    bool system_c::add_device (device_c* device)
    {
      if (!device->is_opened ())
	{
	  if (!device->open ())
	    {
	      return false;
	    }
	}
      if (!m_devices_head)
	{
	  m_devices_head = device;
	}
      else
	{
	  m_devices_tail->next (device);
	}
      m_devices_tail = device;
      return true;
    }
    // -------------------------------------------------------------
    void system_c::start (bool use_threads)
    {
      m_use_threads = use_threads;
      if (m_use_threads)
	{
	  _start_mt ();
	}
      else
	{
	  _start ();
	}
    }
    // -------------------------------------------------------------
    void system_c::join  ()
    {
      if (s_instance->m_use_threads)
	{
	  s_instance->_join_mt ();
	}
      else
	{
	  s_instance->_join ();
	}
      delete s_instance;
      s_instance = 0;
    }
    // -------------------------------------------------------------
    void system_c::post (record_c* record)
    {
      if (m_use_threads)
	{
	  _post_mt (record);
	}
      else
	{
	  _post (record);
	}
    }
    // -------------------------------------------------------------
    void system_c::_start ()
    {
    }
    // -------------------------------------------------------------
    void system_c::_start_mt ()
    {
    }
    // -------------------------------------------------------------
    void system_c::_join ()
    {
    }
    // -------------------------------------------------------------
    void system_c::_join_mt ()
    {
    }
    // -------------------------------------------------------------
    void system_c::_post (record_c* record)
    {
      shared_log_record_t rec (record);
      device_c* dev = m_devices_head; 
      bool at_least_one_printed = false;
      while (dev) 
	{ 
	  if (dev->print (rec))
      {
          at_least_one_printed = true;
      }
	  dev = dev->next ();
	}
      if (!at_least_one_printed)
      {
          m_default_device->print (rec);
      }
    }
    // -------------------------------------------------------------
    void system_c::_post_mt (record_c* /*record*/)
    {
    }
  } // ns logger
} // ns bsw

