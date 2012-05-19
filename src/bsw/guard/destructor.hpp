#ifndef __BSW_GUARD_DESTRUCTOR_HPP__
#define __BSW_GUARD_DESTRUCTOR_HPP__

namespace bsw
{
  namespace utils
  {
    template <class T>
    class destructor_c
    {
    public:
      typedef T elem_t;
    public:
      destructor_c (elem_t* victim);
      virtual ~destructor_c ();
      virtual void operator () ();
    protected:
      elem_t* _victim ();
    private:
      elem_t* m_victim;
    };
    // -------------------------------------------------------------------
    template <class T>
    class array_destructor_c : public destructor_c <T>
    {
      typedef typename destructor_c <T>::elem_t elem_t;
    public:
      array_destructor_c (elem_t* victim);
      virtual void operator () ();
    };
    // -------------------------------------------------------------------
  } // ns utils
} // ns bsw
// ===========================================================================
// Implementation
// ===========================================================================
namespace bsw
{
  namespace utils
  {
    template <class T>
    inline
    destructor_c<T>::destructor_c (elem_t* victim)
      : m_victim (victim)
    {
    }
    // -----------------------------------------------------------------------
    template <class T>
    destructor_c<T>::~destructor_c ()
    {
    }
    // -----------------------------------------------------------------------
    template <class T>
    void destructor_c<T>::operator () ()
    {
      if (_victim ())
	{
	  delete _victim ();
	}
    }
    // -----------------------------------------------------------------------
    template <class T>
    inline
    typename destructor_c<T>::elem_t* destructor_c<T>::_victim ()
    {
      return m_victim;
    }
    // =======================================================================
    template <class T>
    inline
    array_destructor_c<T>::array_destructor_c (elem_t* victim)
      : destructor_c<T>(victim)
    {
    }
    // -----------------------------------------------------------------------
    template <class T>
    void array_destructor_c<T>::operator () ()
    {
      if (this->_victim ())
	{
	  delete [] this->_victim ();
	}
    }
  } // ns bsw
} // ns utils


#endif
