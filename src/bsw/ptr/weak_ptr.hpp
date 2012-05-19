#ifndef __BSW_PTR_WEAK_PTR_HPP__
#define __BSW_PTR_WEAK_PTR_HPP__

#include "bsw/ptr/shared_ptr.hpp"
#include "bsw/guard/destructor.hpp"

namespace bsw
{
  namespace utils
  {
    /**
     * @class weak_ptr_c
     *
     * @brief This class implements support for a weak pointer that complements
     * shared_ptr_c.
     *
     * Unlike shared_ptr_c, assigning or copying instances of an
     * weak_ptr_c will not automatically increment the reference
     * count of the underlying object. What weak_ptr_c does is
     * preserve the knowledge that the object is in fact reference
     * counted, and thus provides an alternative to raw pointers where
     * non-ownership associations must be maintained. When the last
     * instance of an shared_ptr_c that references a particular
     * object is destroyed or overwritten, the corresponding
     * weak_ptr_c instances are set to NULL.
     */
    template <class X, class LOCK>
    class weak_ptr_c
    {
    public:
      /**
       * Constructor that initializes an weak_ptr_c to point to
       * the object p immediately.
       */
      explicit weak_ptr_c (X* p = 0);

      /**
       * Copy constructor binds @c this and @a r to the same object.
       */
      weak_ptr_c (const weak_ptr_c<X, LOCK>& r);
    
      /**
       * Constructor binds @c this and @a r to the same object.
       */
      weak_ptr_c (const shared_ptr_c<X, LOCK>& r);

      /**
       * Destructor.
       */
      ~weak_ptr_c (void);

      /**
       * Assignment operator that binds @c this and @a r to the same object.
       */
      void operator = (const weak_ptr_c<X, LOCK>& r);

      /**
       * Assignment operator that binds @c this and @a r to the same object.
       */
      void operator = (const shared_ptr_c<X, LOCK>& r);

      /**
       * Equality operator that returns @c true if both
       * weak_ptr_c objects point to the same underlying object.
       * @note It also returns @c true if both objects have just been
       *       instantiated and not used yet.
       */
      bool operator == (const weak_ptr_c<X, LOCK>& r) const;

      /**
       * Equality operator that returns @c true if the weak_ptr_c
       * and shared_ptr_c objects point to the same underlying
       * object.
       * @note It also returns @c true if both objects have just been
       *       instantiated and not used yet.
       */
      bool operator == (const shared_ptr_c<X, LOCK>& r) const;

      /**
       * Equality operator that returns @c true if the weak_ptr_c
       * and the raw pointer point to the same underlying object.
       */
      bool operator == (X* p) const;

      /**
       * Inequality operator, which is the opposite of equality.
       */
      bool operator != (const weak_ptr_c<X, LOCK>& r) const;

      /**
       * Inequality operator, which is the opposite of equality.
       */
      bool operator != (const shared_ptr_c<X, LOCK>& r) const;

      /**
       * Inequality operator, which is the opposite of equality.
       */
      bool operator != (X* p) const;

      /**
       * Redirection operator.
       *
       * It returns a temporary strong pointer and makes use of the
       * chaining properties of operator-> to ensure that the underlying
       * object does not disappear while you are using it.  If you are
       * certain of the lifetimes of the object, and do not want to incur
       * the locking overhead, then use the unsafe_get method instead.
       */
      shared_ptr_c<X, LOCK> operator-> (void) const;

      /**
       * Obtain a strong pointer corresponding to this weak pointer. This
       * function is useful to create a temporary strong pointer for
       * conversion to a reference.
       */
      shared_ptr_c<X, LOCK> strong (void) const;

      /**
       * Get the pointer value. Warning: this does not affect the
       * reference count of the underlying object, so it may disappear on
       * you while you are using it if you are not careful.
       */
      X* unsafe_get (void) const;

      /**
       * Resets the weak_ptr_c to refer to a different underlying
       * object.
       */
      void reset (X* p = 0);

      /**
       * Increment the reference count on the underlying object.
       *
       * Returns the new reference count on the object. This function may
       * be used to integrate the bound pointers into an external
       * reference counting mechanism such as those used by COM or CORBA
       * servants.
       */
      long add_ref (void);

      /**
       * Decrement the reference count on the underlying object, which is deleted
       * if the count has reached zero.
       *
       * Returns the new reference count on the object.  This function may
       * be used to integrate the bound pointers into an external
       * reference counting mechanism such as those used by COM or CORBA
       * servants.
       */
      long remove_ref (void);
    
      /**
       * Allows us to check for NULL on all weak_ptr_c objects.
       */
      bool null (void) const;

    private:
      void _dispose (X* ptr);
    private:
      typedef X X_t; // This indirection is for Borland C++.

      friend class shared_ptr_c<X, LOCK>;

      /// The ACE_Bound_Ptr_Counter type.
      typedef ptr_counter_c<LOCK> counter_t;

      /// The reference counter.
      counter_t* m_counter;

      /// The underlying object.
      X* m_ptr;
    };
  } // ns utils
} // ns bsw
// =============================================================
// Implementation
// =============================================================
namespace bsw
{
  namespace utils
  {
    template <class X, class LOCK>
    inline
    weak_ptr_c<X,LOCK>::weak_ptr_c (X* p)
      : m_counter (counter_t::create_weak ()),
	m_ptr (p)
    {
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    weak_ptr_c<X,LOCK>::weak_ptr_c (const weak_ptr_c<X, LOCK>& r)
      : m_counter (r.m_counter),
	m_ptr (r.m_ptr)
    {
      counter_t::attach_weak (this->m_counter);
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    weak_ptr_c<X,LOCK>::weak_ptr_c (const shared_ptr_c<X, LOCK>& r)
      : m_counter (r.m_counter),
	m_ptr     (r.m_ptr)
    {
      counter_t::attach_weak (this->counter_);
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    weak_ptr_c<X,LOCK>::~weak_ptr_c (void)
    {
      counter_t::detach_weak (this->m_counter);
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    void weak_ptr_c<X,LOCK>::operator = (const weak_ptr_c<X, LOCK>& rhs)
    {
      // This will work if &rhs == this, by first increasing the ref count
      counter_t *new_counter = rhs.m_counter;
      counter_t::attach_weak (new_counter);
      counter_t::detach_weak (this->m_counter);
      this->m_counter = new_counter;
      this->m_ptr = rhs.m_ptr;
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    void weak_ptr_c<X,LOCK>::operator = (const shared_ptr_c<X, LOCK>& rhs)
    {
      // This will work if &rhs == this, by first increasing the ref count
      counter_t *new_counter = rhs.m_counter;
      counter_t::attach_weak (new_counter);
      counter_t::detach_weak (this->m_counter);
      this->m_counter = new_counter;
      this->m_ptr = rhs.m_ptr;
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    bool weak_ptr_c<X,LOCK>::operator == (const weak_ptr_c<X, LOCK>& r) const
    {
      // A weak pointer must behave as though it is automatically set to null
      // if the underlying object has been deleted.
      if (counter_t::object_was_deleted (this->m_counter))
	{
	  return (r.m_ptr == 0);
	}

      return (this->m_ptr == r.m_ptr);
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    bool weak_ptr_c<X,LOCK>::operator == (const shared_ptr_c<X, LOCK>& r) const
    {
    
      // A weak pointer must behave as though it is automatically set to null
      // if the underlying object has been deleted.
      if (counter_t::object_was_deleted (this->m_counter))
	{
	  return (r.m_ptr == 0);
	}

      return (this->m_ptr == r.m_ptr);
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    bool weak_ptr_c<X,LOCK>::operator == (X* p) const
    {
      // A weak pointer must behave as though it is automatically set to null
      // if the underlying object has been deleted.
      if (counter_t::object_was_deleted (this->m_counter))
	{
	  return (p == 0);
	}
    
      return (this->m_ptr == p);

    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    bool weak_ptr_c<X,LOCK>::operator != (const weak_ptr_c<X, LOCK>& r) const
    {
      // A weak pointer must behave as though it is automatically set to null
      // if the underlying object has been deleted.
      if (counter_t::object_was_deleted (this->m_counter))
	{
	  return (r.ptr != 0);
	}

      return (this->m_ptr != r.m_ptr);
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    bool weak_ptr_c<X,LOCK>::operator != (const shared_ptr_c<X, LOCK>& r) const
    {
      // A weak pointer must behave as though it is automatically set to null
      // if the underlying object has been deleted.
      if (counter_t::object_was_deleted (this->m_counter))
	{
	  return (r.ptr != 0);
	}

      return (this->m_ptr != r.m_ptr);
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    bool weak_ptr_c<X,LOCK>::operator != (X* p) const
    {
      // A weak pointer must behave as though it is automatically set to null
      // if the underlying object has been deleted.
      if (counter_t::object_was_deleted (this->m_counter))
	{
	  return (p != 0);
	}

      return (this->m_ptr != p);
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    shared_ptr_c<X, LOCK> weak_ptr_c<X,LOCK>::operator-> (void) const
    {
      return shared_ptr_c<X, LOCK> (*this);
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    shared_ptr_c<X, LOCK> weak_ptr_c<X,LOCK>::strong (void) const
    {
      return shared_ptr_c<X, LOCK> (*this);
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    X* weak_ptr_c<X,LOCK>::unsafe_get (void) const
    {
      // We do not check if the object has been deleted, since this operation
      // is defined to be unsafe!
      return this->m_ptr;
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    void weak_ptr_c<X,LOCK>::reset (X* p)
    {
      counter_t *old_counter = this->m_counter;
      this->m_counter = counter_t::create_weak ();
      this->m_ptr = p;
      counter_t::detach_weak (old_counter);
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    long weak_ptr_c<X,LOCK>::add_ref (void)
    {
      return counter_t::attach_strong (m_counter);
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    long weak_ptr_c<X,LOCK>::remove_ref (void)
    {
      long new_obj_ref_count = counter_t::detach_strong (m_counter);
      if (new_obj_ref_count == 0)
	{
	  this->_dispose (this->m_ptr);
	  this->m_ptr = 0;
	}
      return new_obj_ref_count;
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    bool weak_ptr_c<X,LOCK>::null (void) const
    {
      // A weak pointer must behave as though it is automatically set to null
      // if the underlying object has been deleted.
      if (counter_t::object_was_deleted (this->m_counter))
	{
	  return true;
	}
      return (this->m_ptr == 0);
    }
    // ----------------------------------------------------------
    template <class X, class LOCK>
    inline
    void weak_ptr_c<X,LOCK>::_dispose (X* ptr)
    {
      destructor_c<X> d(ptr);
      d ();
    }
  } // ns utils
} // ns bsw
#endif
