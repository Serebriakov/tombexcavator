#ifndef __BSW_PTR_COUNTER_HPP__
#define __BSW_PTR_COUNTER_HPP__

#include <stdexcept>
#include "bsw/mt/mutex.hpp"
namespace bsw
{
  namespace utils
  {
    /**
     * @class ptr_counter_c
     *
     * @brief An ptr_counter_c <LOCK> object encapsulates an
     *        object reference count.
     *
     * Do not use this class directly, use shared_ptr_c or
     * weak_ptr instead.
     */
    template <class LOCK>
    class ptr_counter_c
    {
    public:
      typedef LOCK lock_t;
    public:
      ptr_counter_c (long init_obj_ref_count = 0);
      ~ptr_counter_c ();
    
      /**
       * Create a ptr_counter_c<LOCK> and initialize the
       * reference count to indicate ownership by a strong pointer.
       */
      static ptr_counter_c<LOCK>* create_strong ();

      /**
       * Increase both the object and counter reference counts and return
       * the new object reference count. A return value of -1 indicates
       * that the object has already been destroyed.
       */
      static long attach_strong (ptr_counter_c<LOCK>* counter);

      /**
       * Decreases both the object and counter reference counts and
       * deletes whichever has no more references. Returns the new object
       * reference count.
       */
      static long detach_strong (ptr_counter_c<LOCK>* counter);

      /**
       * create a ptr_counter_c<LOCK> and initialize the
       * reference count to indicate no ownership.
       */
      static ptr_counter_c<LOCK>* create_weak ();

      /**
       * Increase the counter reference count and return argument.
       */
      static void attach_weak (ptr_counter_c<LOCK>* counter);

      /**
       * Decreases the counter reference count and deletes the counter if
       * it has no more references.
       */
      static void detach_weak (ptr_counter_c<LOCK>* counter);
    
      /**
       * Determine whether the object has been deleted.
       */
      static bool object_was_deleted (ptr_counter_c<LOCK>* counter);
    
    private:
      /**
       * Allocate a new ptr_counter_c<LOCK> instance,
       * returning NULL if it cannot be created.
       */
      static ptr_counter_c<LOCK>* _internal_create (long init_obj_ref_count);
    
    private:
      /**
       * Reference count of underlying object. Is set to -1 once the
       * object has been destroyed to indicate to all weak pointers that
       * it is no longer valid.
       */
      long m_obj_ref_count;
    
      /**
       * Reference count of this counter.
       */
      long m_self_ref_count;
    
      /**
       * Mutex variable to synchronize access to the reference counts.
       */
      lock_t m_lock;
    };
 
  } // ns utils
} // ns bsw
// ======================================================================
// Implementation
// ======================================================================
namespace bsw
{
  namespace utils
  {
    template <class LOCK>
    ptr_counter_c<LOCK>::ptr_counter_c (long init_obj_ref_count)
      : m_obj_ref_count (init_obj_ref_count),
	m_self_ref_count (1)
    {
    }
    // --------------------------------------------------------------------
    template <class LOCK>
    ptr_counter_c<LOCK>::~ptr_counter_c ()
    {
    }
    // --------------------------------------------------------------------
    template <class LOCK>
    inline
    ptr_counter_c<LOCK>* ptr_counter_c<LOCK>::create_strong ()
    {
      // Set initial object reference count to 1.
      ptr_counter_c<LOCK>* temp = _internal_create (1);
      if (temp == 0)
	{
	  throw std::bad_alloc ();
	}
      return temp;
    }
    // --------------------------------------------------------------------
    template <class LOCK>
    inline
    long ptr_counter_c<LOCK>::attach_strong (ptr_counter_c<LOCK>* counter)
    {
      mt::lock_guard_c <LOCK> guard (counter->m_lock);

      // Can't attach a strong pointer to an object that 
      // has already been deleted.
      if (counter->m_obj_ref_count == -1)
	{
	  return -1;
	}

      long new_obj_ref_count = ++counter->m_obj_ref_count;
      ++counter->m_self_ref_count;

      return new_obj_ref_count;
    }
    // --------------------------------------------------------------------
    template <class LOCK>
    inline
    long ptr_counter_c<LOCK>::detach_strong (ptr_counter_c<LOCK>* counter)
    {
      ptr_counter_c<LOCK>* counter_del = 0;
      long new_obj_ref_count;

      {
	mt::lock_guard_c <LOCK> guard (counter->m_lock);

	if ((new_obj_ref_count = --counter->m_obj_ref_count) == 0)
	  // Change the object reference count to -1 to indicate that the
	  // object has been deleted, as opposed to a weak pointer that
	  // simply hasn't had any strong pointers created from it yet.
	  counter->m_obj_ref_count = -1;

	if (--counter->m_self_ref_count == 0)
	  // Since counter contains the lock held by the ACE_Guard, the
	  // guard needs to be released before freeing the memory holding
	  // the lock. So save the pointer to free, then release, then
	  // free.
	  counter_del = counter;

      }  // Release the lock
    
      delete counter_del;

      return new_obj_ref_count; 
    }
    // --------------------------------------------------------------------
    template <class LOCK>
    ptr_counter_c<LOCK>* ptr_counter_c<LOCK>::create_weak ()
    {
      // Set initial object reference count to 0.

      ptr_counter_c<LOCK>* temp = _internal_create (0);
      if (temp == 0)
	{
	  throw std::bad_alloc ();
	}
      return temp;
    }
    // --------------------------------------------------------------------
    template <class LOCK>
    inline
    void ptr_counter_c<LOCK>::attach_weak (ptr_counter_c<LOCK>* counter)
    {
      mt::lock_guard_c <LOCK> guard (counter->m_lock);
      ++counter->m_self_ref_count;
    }
    // --------------------------------------------------------------------
    template <class LOCK>
    void ptr_counter_c<LOCK>::detach_weak (ptr_counter_c<LOCK>* counter)
    {
      ptr_counter_c<LOCK>* counter_del = 0;

      {
	mt::lock_guard_c <LOCK> guard (counter->m_lock);

	if (--counter->m_self_ref_count == 0)
	  // Since counter contains the lock held by the Guard, the
	  // guard needs to be released before freeing the memory holding
	  // the lock. So save the pointer to free, then release, then
	  // free.
	  counter_del = counter;

      }  // Release the lock
    
      delete counter_del;
    }
    // --------------------------------------------------------------------
    template <class LOCK>
    bool ptr_counter_c<LOCK>::object_was_deleted (ptr_counter_c<LOCK>* counter)
    {
      mt::lock_guard_c <LOCK> guard (counter->m_lock);
      return (counter->m_obj_ref_count == -1);
    }
    // --------------------------------------------------------------------
    template <class LOCK>
    ptr_counter_c<LOCK>* ptr_counter_c<LOCK>::_internal_create (long obj_ref_cnt)
    {
      ptr_counter_c<LOCK>* temp = new ptr_counter_c<LOCK> (obj_ref_cnt);
      return temp;
    }

  } // ns utils
} // ns bsw
#endif
