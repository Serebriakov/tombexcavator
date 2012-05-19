#ifndef __BSW_MT_QUEUE_HPP__
#define __BSW_MT_QUEUE_HPP__

#include <queue>
#include "bsw/mt/mutex.hpp"

namespace bsw
{
  namespace mt
  {
    template <class T>
    class queue_c
    {
    public:
      queue_c ()
	: m_max (1024)
      {
      }

      queue_c (std::size_t capacity)
	: m_max (capacity)
      {
      }

      void push (T const& data)
      {
	lock_t lock (m_mutex);
	while (m_queue.size () >= m_max)
	  {
	    m_full_cnd.wait (m_mutex);
	  }
	m_queue.push (data);
	m_empty_cnd.broadcast ();
      }
      // -------------------------------------------
      bool empty() const
      {
	lock_t lock (m_mutex);
	return m_queue.empty ();
      }
      // -------------------------------------------
      void pop(T& popped_value)
      {
	lock_t lock (m_mutex);
	while (m_queue.empty ())
	  {
	    m_empty_cnd.wait (m_mutex);
	  }
	  
	popped_value= m_queue.front();
	m_queue.pop();
	m_full_cnd.broadcast ();
      }

    private:
      typedef mutex_c                mutex_t;
      typedef lock_guard_c <mutex_c> lock_t;
    private:
      std::queue <T>              m_queue;
      mutable mutex_t             m_mutex;
      conditional_variable_c      m_empty_cnd;
      conditional_variable_c      m_full_cnd;
      std::size_t                 m_max;
    };
  } // ns mt
} // ns bsw

#endif
