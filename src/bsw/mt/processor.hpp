#ifndef __BSW_MT_PROCESSOR_HPP__
#define __BSW_MT_PROCESSOR_HPP__

#include "bsw/mt/queue.hpp"

namespace bsw
{
  namespace mt
  {
    template <class T>
    class processor_c
    {
    public:
      typedef queue_c <T> queue_t;
    public:
      processor_c ()
      {
      }
      // ------------------------------------------------------
      virtual ~processor_c ()
      {
      }
      // ------------------------------------------------------
      virtual void svc () = 0;
      // ------------------------------------------------------
      static void execute (void* arg)
      {
	processor_c <T>* obj = (processor_c <T>*) arg;
	obj->svc ();
      }
      // ------------------------------------------------------
      queue_t& queue ()
      {
	return m_queue;
      }
    protected:
      queue_t m_queue;
    };
  } // ns mt
} // ns bsw

#endif
