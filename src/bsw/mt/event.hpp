#ifndef __BSW_MT_EVENT_HPP__
#define __BSW_MT_EVENT_HPP__

#include "bsw/bsw_api.hpp"
#include "thirdparty/my_stdint.h"

namespace bsw
{
  namespace mt
  {
    struct event_impl_s;
    /// An Event is a synchronization object that
    /// allows one thread to signal one or more
    /// other threads that a certain event
    /// has happened.
    /// Usually, one thread signals an event,
    /// while one or more other threads wait
    /// for an event to become signalled.
    class BSW_API event_c
    {
    public:
      explicit event_c (bool auto_reset);
      ~event_c ();

      bool signal ();
      bool wait ();
      int wait (uint32_t timeout_ms);

      bool reset ();
    private:
      event_c (const event_c&);
      event_c& operator = (const event_c&);
    private:
      event_impl_s* m_pimpl;
    };
  } // ns mt
} // ns bsw

#endif
