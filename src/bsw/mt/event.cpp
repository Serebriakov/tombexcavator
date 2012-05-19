#include <stdexcept>
#include "bsw/mt/event.hpp"
#include "bsw/mt/os_config.hpp"

#if !defined(BSW_USE_PTHREAD)
#include <windows.h>
#else
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#endif

namespace bsw
{
  namespace mt
  {
#if defined(BSW_USE_PTHREAD)
    struct event_impl_s
    {
      event_impl_s (bool auto_reset);		
      ~event_impl_s ();
      bool set () ;

      bool wait ();
      int wait (uint32_t milliseconds);
      bool reset ();

    private:
      bool            m_auto;
      volatile bool   m_state;
      pthread_mutex_t m_mutex;
      pthread_cond_t  m_cond;
    };
    // -----------------------------------------------------------        
    inline bool event_impl_s::set ()
    {
      if (pthread_mutex_lock (&m_mutex))	
	{
	  return false;
	}
      m_state = true;
      if (pthread_cond_broadcast (&m_cond))
	{
	  pthread_mutex_unlock (&m_mutex);
	  return false;
	}
      pthread_mutex_unlock (&m_mutex);
      return true;
    }
    // ---------------------------------------------------------
    inline bool event_impl_s::reset ()
    {
      if (pthread_mutex_lock (&m_mutex))
	{
	  return false;
	}
      m_state = false;
      pthread_mutex_unlock (&m_mutex);
      return true;
    }
    // ---------------------------------------------------------
    event_impl_s::event_impl_s (bool auto_reset)
      : m_auto(auto_reset), 
	m_state(false)
    {
      if (pthread_mutex_init (&m_mutex, NULL))
	{
	  throw std::runtime_error ("cannot create event (mutex)");
	}
      if (pthread_cond_init (&m_cond, NULL))
	{
	  throw std::runtime_error ("cannot create event (condition)");
	}
    }
    // ---------------------------------------------------------
    event_impl_s::~event_impl_s ()
    {
      pthread_cond_destroy (&m_cond);
      pthread_mutex_destroy (&m_mutex);
    }
    // ---------------------------------------------------------
    bool event_impl_s::wait ()
    {
      if (pthread_mutex_lock (&m_mutex))
	{
	  return false;
	}
      while (!m_state) 
	{
	  if (pthread_cond_wait (&m_cond, &m_mutex))
	    {
	      pthread_mutex_unlock (&m_mutex);
	      return false;
	    }
	}
      if (m_auto)
	{
	  m_state = false;
	}
      pthread_mutex_unlock (&m_mutex);
      return true;
    }
    // ----------------------------------------------------------
    int event_impl_s::wait (uint32_t milliseconds)
    {
      int rc = 0;
      struct timespec abstime;

      struct timeval tv;
      gettimeofday(&tv, NULL);
      abstime.tv_sec  = tv.tv_sec + milliseconds / 1000;
      abstime.tv_nsec = tv.tv_usec*1000 + (milliseconds % 1000)*1000000;
      if (abstime.tv_nsec >= 1000000000)
	{
	  abstime.tv_nsec -= 1000000000;
	  abstime.tv_sec++;
	}

      if (pthread_mutex_lock (&m_mutex) != 0)
	{
	  return -1;
	}
      while (!m_state) 
	{
	  if ((rc = pthread_cond_timedwait (&m_cond, &m_mutex, &abstime)))
	    {
	      if (rc == ETIMEDOUT) break;
	      pthread_mutex_unlock(&m_mutex);
	      return -1;
	    }
	}
      if (rc == 0 && m_auto) 
	{
	  m_state = false;
	}
      pthread_mutex_unlock(&m_mutex);
      return (rc == 0) ? 1 : 0;
    }

#else
    struct event_impl_s
    {
        
      event_impl_s (bool autoReset);		
      ~event_impl_s ();

      bool set ();
      bool wait ();
      int wait (uint32_t milliseconds);
      bool reset ();
        
      HANDLE m_event;
    };
    // ----------------------------------------------------------------------
    inline bool event_impl_s::set ()
    {
      return (SetEvent (m_event) == TRUE);
    }
    // ----------------------------------------------------------------------
    inline bool event_impl_s::reset ()
    {
      return (ResetEvent (m_event) == TRUE);
    }
    // ----------------------------------------------------------------------
    event_impl_s::event_impl_s (bool autoReset)
    {
      m_event = CreateEvent (NULL, autoReset ? FALSE : TRUE, FALSE, NULL);
      if (!m_event)
	{
	  throw std::runtime_error ("cannot create event");
	}
    }
    // ----------------------------------------------------------------------
    event_impl_s::~event_impl_s()
    {
      CloseHandle(m_event);
    }
    // ----------------------------------------------------------------------
    bool event_impl_s::wait ()
    {
      switch (WaitForSingleObject (m_event, INFINITE))
	{
	case WAIT_OBJECT_0:
	  return true;
	}
      return false;
    }
    // ----------------------------------------------------------------------
    int event_impl_s::wait (uint32_t milliseconds)
    {
      switch (WaitForSingleObject (m_event, milliseconds + 1))
	{
	case WAIT_TIMEOUT:
	  return 0;
	case WAIT_OBJECT_0:
	  return 1;
	}
      return -1;
    }
#endif
    event_c::event_c (bool auto_reset)
    {
      m_pimpl = new event_impl_s (auto_reset);
    }
    // -------------------------------------------------------------            
    event_c::~event_c ()
    {
      delete m_pimpl;
    }
    // -------------------------------------------------------------
    bool event_c::signal ()
    {
      return m_pimpl->set ();
    }
    // -------------------------------------------------------------
    bool event_c::wait ()
    {
      return m_pimpl->wait ();
    }
    // -------------------------------------------------------------
    int event_c::wait (uint32_t timeout_ms)
    {
      return m_pimpl->wait (timeout_ms);
    }
    // -------------------------------------------------------------
    bool event_c::reset ()
    {
      return m_pimpl->reset ();
    }
  } // ns mt
} // ns bsw


