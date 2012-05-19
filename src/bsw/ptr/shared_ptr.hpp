#ifndef __BSW_PTR_SHARED_PTR_HPP__
#define __BSW_PTR_SHARED_PTR_HPP__

#include "bsw/ptr/ptr_counter.hpp"
#include "bsw/guard/destructor.hpp"

namespace bsw
{
  namespace utils
  {
    // Forward decl.
    template <class X, class LOCK> class weak_ptr_c;

    /**
     * @class shared_ptr_c
     *
     * @brief This class implements support for a reference counted
     *        pointer.
     *
     * Assigning or copying instances of an shared_ptr_c will
     * automatically increment the reference count of the underlying object.
     * When the last instance of an shared_ptr_c that references a
     * particular object is destroyed or overwritten, it will invoke delete
     * on its underlying pointer.
     */
    template <class X, class LOCK>
    class shared_ptr_c
    {
    public:
      typedef X    type_t;
      typedef LOCK lock_t;
    public:
      /**
       * Constructor that initializes an shared_ptr_c to point to the
       * object p immediately.
       */
      explicit shared_ptr_c (X* p = 0);


      /**
       * Copy constructor binds @c this and @a r to the same object.
       */
      shared_ptr_c (const shared_ptr_c<X, LOCK>& r);

      /**
       * Constructor binds @c this and @a r to the same object.
       */
      shared_ptr_c (const weak_ptr_c<X, LOCK>& r);

      /**
       * Copy constructor binds @c this and @a r to the same object if
       * Y* can be implicitly converted to X*.
       */
      template <class Y>
      shared_ptr_c (const shared_ptr_c<Y, LOCK>& r)
	: m_counter (r.m_counter),
	  m_ptr (dynamic_cast<X_t*>(r.m_ptr))
      {
	// This ctor is temporarily defined here to increase our chances
	// of being accepted by broken compilers.
	//
	counter_t::attach_strong (this->m_counter);
      }

      /**
       * Destructor.
       */
      ~shared_ptr_c ();

      /**
       * Assignment operator that binds @c this and @a r to the same object.
       */
      void operator = (const shared_ptr_c<X, LOCK>& r);
    
      /**
       * Assignment operator that binds @c this and @a r to the same object.
       */
      void operator = (const weak_ptr_c<X, LOCK>& r);
    
      /**
       * Assignment operator that binds @c this and @a r to the same object
       * if Y* can be implicitly converted to X*.
       */
      template <class Y>
      weak_ptr_c<X, LOCK>&
      operator= (const shared_ptr_c<Y, LOCK>& r)
      {
	// This operator is temporarily defined here to increase our chances
	// of being accepted by broken compilers.
	//

	// This will work if& r == this, by first increasing the ref count

	counter_t* new_counter = r.m_counter;
	X* new_ptr = dynamic_cast<X_t*> (r.m_ptr);
	counter_t::attach_strong (new_counter);
	if (counter_t::detach_strong (this->m_counter) == 0)
	  {
	    this->_dispose (m_ptr);
	  }
	this->m_counter = new_counter;
	this->m_ptr = new_ptr;

	return *this;
      }

      /**
       *Equality operator that returns @c true if both
       * shared_ptr_c instances point to the same underlying
       * object.
       * @note It also returns @c true if both objects have just been
       *       instantiated and not used yet.
       */
      bool operator == (const shared_ptr_c<X, LOCK>& r) const;

      /**
       * Equality operator that returns true if the shared_ptr_c
       * and weak_ptr_c objects point to the same underlying
       * object.
       * @note It also returns @c true if both objects have just been
       *       instantiated and not used yet.
       */
      bool operator == (const weak_ptr_c<X, LOCK>& r) const;

      /**
       * Equality operator that returns @c true if the
       * shared_ptr_c and the raw pointer point to the same
       * underlying object.
       */
      bool operator == (X* p) const;

      /**
       * Inequality operator, which is the opposite of equality.
       */
      bool operator != (const shared_ptr_c<X, LOCK>& r) const;

      /**
       * Inequality operator, which is the opposite of equality.
       */
      bool operator != (const weak_ptr_c<X, LOCK>& r) const;

      /**
       * Inequality operator, which is the opposite of equality.
       */
      bool operator != (X* p) const;

      /**
       * Redirection operator
       */
      X* operator-> (void) const;

      /**
       * Dereference operator
       */
      X& operator*  (void) const;

      /**
       * Get the pointer value.
       */
      X* get (void) const;

      /**
       * Resets the shared_ptr_c to refer to a different
       * underlying object.
       */
      void reset (X* p = 0);

      /**
       * Allows us to check for NULL on all shared_ptr_c
       * objects.
       */
      bool null (void) const;

    private:
      void _dispose (X* ptr);
    private:
      typedef X X_t; // This indirection is for Borland C++.

      friend class weak_ptr_c<X, LOCK>;

      template <class Y, class L>
      friend class shared_ptr_c;

      /**
       * The ptr_counter_c type.
       */
      typedef ptr_counter_c<LOCK> counter_t;

      /**
       * The reference counter.
       */
      counter_t* m_counter;

      /**
       * The underlying object.
       */
      X* m_ptr;
    };
  } // ns utils
} // ns bsw
// ==============================================================
// Implementation
// ==============================================================
namespace bsw
{
  namespace utils
  {
    template <class X, class LOCK>
    inline
    shared_ptr_c <X,LOCK>::shared_ptr_c (X* p)
      : m_counter (counter_t::create_strong ()),
	m_ptr (p)
    {
    }
    // ------------------------------------------------------------
    template <class X, class LOCK>
    inline
    shared_ptr_c <X,LOCK>::shared_ptr_c (const shared_ptr_c<X, LOCK>& r)
      : m_counter (r.m_counter),
	m_ptr (r.m_ptr)
    {
      counter_t::attach_strong (this->m_counter);
    }
    // ------------------------------------------------------------
    template <class X, class LOCK>
    shared_ptr_c <X,LOCK>::shared_ptr_c (const weak_ptr_c<X, LOCK>& r)
      : m_counter (r.m_counter),
	m_ptr     (r.m_ptr)
    {
      // When creating a strong pointer from a weak one we can't assume that the
      // underlying object still exists. Therefore we must check for a return 
      // value of -1, which indicates that the object has been destroyed.
      if (counter_t::attach_strong (this->m_counter) == -1)
	{
	  // Underlying object has already been deleted, 
	  // so set this pointer to null.
	  this->m_counter = counter_t::create_strong ();
	  this->m_ptr = 0;
	}
    }
    // ------------------------------------------------------------
    template <class X, class LOCK>
    inline
    shared_ptr_c <X,LOCK>::~shared_ptr_c ()
    {
      if (counter_t::detach_strong (this->m_counter) == 0)
	{
	  this->_dispose (this->m_ptr);
	}
    }
    // ------------------------------------------------------------
    template <class X, class LOCK>
    inline
    void shared_ptr_c <X,LOCK>::operator = (const shared_ptr_c<X, LOCK>& rhs)
    {
      // This will work if &r == this, by first increasing the ref count, but
      // why go through all that?
      if (&rhs == this)
	{
	  return;
	}

      counter_t* new_counter = rhs.m_counter;
      X_t* new_ptr = rhs.m_ptr;
      counter_t::attach_strong (new_counter);
      if (counter_t::detach_strong (this->m_counter) == 0)
	{
	  this->_dispose (this->m_ptr);
	}
      this->m_counter = new_counter;
      this->m_ptr     = new_ptr;
    }
    // ------------------------------------------------------------
    template <class X, class LOCK>
    inline
    void shared_ptr_c <X,LOCK>::operator = (const weak_ptr_c<X, LOCK>& rhs)
    {
      // This will work if &r == this, by first increasing the ref count, but
      // why go through all that?
      if (&rhs == this)
	{
	  return;
	}

      counter_t *new_counter = rhs.m_counter;
      X_t *new_ptr = rhs.m_ptr;

      // When creating a strong pointer from a weak one we can't assume that the
      // underlying object still exists. Therefore we must check for a 
      // return value of -1, which indicates that the object has been destroyed.
      if (counter_t::attach_strong (new_counter) == -1)
	{
	  // Underlying object has already been deleted, 
	  // so set this pointer to null.
	  new_counter = counter_t::create_strong ();

	  new_ptr = 0;
	}

      if (counter_t::detach_strong (this->m_counter) == 0)
	{
	  this->_dispose (this->m_ptr);
	}
      this->m_counter = new_counter;
      this->m_ptr = new_ptr;
    }
    // ------------------------------------------------------------
    template <class X, class LOCK>
    inline
    bool shared_ptr_c<X,LOCK>::operator == (const shared_ptr_c<X, LOCK>& r) const
    {
      return (this->m_ptr == r.m_ptr);
    }
    // ------------------------------------------------------------
    template <class X, class LOCK>
    inline
    bool shared_ptr_c <X,LOCK>::operator == (const weak_ptr_c<X, LOCK>& r) const
    {
      // Use the weak pointer's operator== since it will check for null.
      return (r == *this);
    }
    // ------------------------------------------------------------
    template <class X, class LOCK>
    inline
    bool shared_ptr_c <X,LOCK>::operator == (X* p) const
    {
      return this->m_ptr == p;
    }
    // ------------------------------------------------------------
    template <class X, class LOCK>
    inline
    bool shared_ptr_c<X,LOCK>::operator != (const shared_ptr_c<X,LOCK>& r) const
    {
      return (this->m_ptr != r.m_ptr);
    }
    // ------------------------------------------------------------
    template <class X, class LOCK>
    inline
    bool shared_ptr_c <X,LOCK>::operator != (const weak_ptr_c<X, LOCK>& r) const
    {
      // Use the weak pointer's operator!= since it will check for null.
      return (r != *this);
    }
    // ------------------------------------------------------------
    template <class X, class LOCK>
    inline
    bool shared_ptr_c <X,LOCK>::operator != (X* p) const
    {
      return (this->m_ptr != p);
    }
    // ------------------------------------------------------------
    template <class X, class LOCK>
    inline
    X* shared_ptr_c <X,LOCK>::operator-> (void) const
    {
      return this->m_ptr;
    }
    // ------------------------------------------------------------
    template <class X, class LOCK>
    inline
    X& shared_ptr_c <X,LOCK>::operator*  (void) const
    {
      return *this->m_ptr;
    }
    // ------------------------------------------------------------
    template <class X, class LOCK>
    inline
    X* shared_ptr_c <X,LOCK>::get (void) const
    {
      return this->m_ptr;
    }
    // ------------------------------------------------------------
    template <class X, class LOCK>
    inline
    void shared_ptr_c <X,LOCK>::reset (X* p)
    {
      counter_t *old_counter = this->m_counter;
      X_t *old_ptr = this->m_ptr;
      this->m_counter = counter_t::create_strong ();
      this->m_ptr = p;
      if (counter_t::detach_strong (old_counter) == 0)
	{
	  this->_dispose (old_ptr);
	}
    }
    // ------------------------------------------------------------
    template <class X, class LOCK>
    inline
    bool shared_ptr_c <X,LOCK>::null (void) const
    {
      return (this->m_ptr == 0);
    }
    // ------------------------------------------------------------
    template <class X, class LOCK>
    inline
    void shared_ptr_c <X,LOCK>::_dispose (X* ptr)
    {
      destructor_c<X> d(ptr);
      d ();
    }
  } // ns utiis
} // ns bsw
#endif
