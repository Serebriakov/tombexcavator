#ifndef __BSW_MT_THREAD_ID_HPP__
#define __BSW_MT_THREAD_ID_HPP__

#if !defined(_WIN32)
#include <pthread.h>
#endif

#include "bsw/bsw_api.hpp"

namespace bsw
{
  namespace mt
  {
#if defined(_WIN32)
    typedef int thread_id_t;
#else
    typedef pthread_t thread_id_t;
#endif
    BSW_API thread_id_t current_thread_id ();
  } // ns mt
} // ns bsw

#endif
