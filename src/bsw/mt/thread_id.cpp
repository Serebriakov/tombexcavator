#if defined(_WIN32)
#include <windows.h>
#endif

#include "bsw/mt/thread_id.hpp"

namespace bsw
{
  namespace mt
  {
    thread_id_t current_thread_id ()
    {
#if defined(_WIN32)
      return (int)GetCurrentThreadId();
#else
      return pthread_self();
#endif      
    }
  } // ns mt
} // ns bsw

